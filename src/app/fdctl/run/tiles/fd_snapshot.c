/* Gossip verify tile sits before the gossip (dedup?) tile to verify incoming
   gossip packets */
#define _GNU_SOURCE

#include "../../../../disco/tiles.h"

#include "../../../../disco/topo/fd_pod_format.h"
#include "../../../../funk/fd_funk.h"
#include "../../../../flamenco/runtime/fd_txncache.h"
#include "../../../../flamenco/runtime/fd_runtime.h"
#include "../../../../flamenco/snapshot/fd_snapshot_create.h"

#include "generated/snaps_seccomp.h"

#define SCRATCH_MAX    (1024UL /*MiB*/ << 21)
#define SCRATCH_DEPTH  (128UL) /* 128 scratch frames */
#define TPOOL_WORKER_MEM_SZ (1UL<<30UL) /* 256MB */


struct fd_snapshot_tile_ctx {
  ulong           interval;
  char const    * out_dir;
  fd_funk_t     * funk;
  fd_txncache_t * status_cache;
  fd_wksp_t     * status_cache_wksp;
  ulong         * is_constipated;

  int             tmp_fd;
  int             snapshot_fd;

  uchar           tpool_mem[FD_TPOOL_FOOTPRINT( FD_TILE_MAX )] __attribute__( ( aligned( FD_TPOOL_ALIGN ) ) );
  fd_tpool_t *    tpool;
};
typedef struct fd_snapshot_tile_ctx fd_snapshot_tile_ctx_t;

void FD_FN_UNUSED
tpool_snap_boot( fd_topo_t * topo, ulong total_thread_count ) {
  ushort tile_to_cpu[ FD_TILE_MAX ] = { 0 };
  ulong thread_count = 0;
  ulong main_thread_seen = 0;

  for( ulong i=0UL; i<topo->tile_cnt; i++ ) {
    if( strcmp( topo->tiles[i].name, "thread" ) == 0 ) {
      tile_to_cpu[ 1+thread_count ] = (ushort)topo->tiles[i].cpu_idx;
      thread_count++;
    }
    if( strcmp( topo->tiles[i].name, "snaps" ) == 0 ) {
      tile_to_cpu[ 0 ] = (ushort)topo->tiles[i].cpu_idx;
      main_thread_seen = 1;
    }
  }

  if( main_thread_seen ) {
    thread_count++;
  }

  if( thread_count != total_thread_count )
    FD_LOG_WARNING(( "thread count mismatch thread_count=%lu total_thread_count=%lu main_thread_seen=%lu", thread_count, total_thread_count, main_thread_seen ));

  fd_tile_private_map_boot( tile_to_cpu, thread_count );
}

FD_FN_CONST static inline ulong
scratch_align( void ) {
  return 128UL;
}

FD_FN_PURE static inline ulong
scratch_footprint( fd_topo_tile_t const * tile FD_PARAM_UNUSED ) {
  ulong l = FD_LAYOUT_INIT;
  l = FD_LAYOUT_APPEND( l, alignof(fd_snapshot_tile_ctx_t), sizeof(fd_snapshot_tile_ctx_t) );
  l = FD_LAYOUT_APPEND( l, FD_SCRATCH_ALIGN_DEFAULT, tile->snaps.tpool_thread_count * TPOOL_WORKER_MEM_SZ );
  l = FD_LAYOUT_APPEND( l, fd_scratch_smem_align(), fd_scratch_smem_footprint( SCRATCH_MAX   ) );
  l = FD_LAYOUT_APPEND( l, fd_scratch_fmem_align(), fd_scratch_fmem_footprint( SCRATCH_DEPTH ) );
  return FD_LAYOUT_FINI( l, scratch_align() );
}

static void
privileged_init( fd_topo_t      * topo FD_PARAM_UNUSED,
                 fd_topo_tile_t * tile FD_PARAM_UNUSED ) {

  /* First open the relevant files here. */
  FD_LOG_WARNING(("DONE HERE 1"));

  char tmp_dir_buf[ FD_SNAPSHOT_DIR_MAX ];
  int err = snprintf( tmp_dir_buf, FD_SNAPSHOT_DIR_MAX, "%s/%s", tile->snaps.out_dir, FD_SNAPSHOT_TMP_ARCHIVE );
  if( FD_UNLIKELY( err<0 ) ) {
    FD_LOG_ERR(( "Failed to format directory string" ));
  }

  char zstd_dir_buf[ FD_SNAPSHOT_DIR_MAX ];
  err = snprintf( zstd_dir_buf, FD_SNAPSHOT_DIR_MAX, "%s/%s", tile->snaps.out_dir, FD_SNAPSHOT_TMP_ARCHIVE_ZSTD );
  if( FD_UNLIKELY( err<0 ) ) {
    FD_LOG_ERR(( "Failed to format directory string" ));
  }

  /* Create and open the relevant files for snapshots. */

  tile->snaps.tmp_fd = open( tmp_dir_buf, O_CREAT | O_RDWR | O_TRUNC, 0644 );
  if( FD_UNLIKELY( tile->snaps.tmp_fd==-1 ) ) {
    FD_LOG_ERR(( "Failed to open and create tarball for file=%s (%i-%s)", tmp_dir_buf, errno, fd_io_strerror( errno ) ));
  }

  tile->snaps.snapshot_fd = open( zstd_dir_buf, O_RDWR | O_CREAT | O_TRUNC, 0644 );
  if( FD_UNLIKELY( tile->snaps.snapshot_fd==-1 ) ) {
    FD_LOG_WARNING(( "Failed to open the snapshot file (%i-%s)", errno, fd_io_strerror( errno ) ));
  }

  return;
}

static void
unprivileged_init( fd_topo_t      * topo FD_PARAM_UNUSED,
                   fd_topo_tile_t * tile FD_PARAM_UNUSED ) {


  void * scratch = fd_topo_obj_laddr( topo, tile->tile_obj_id );

  /**********************************************************************/
  /* scratch (bump)-allocate memory owned by the replay tile            */
  /**********************************************************************/

  /* Do not modify order! This is join-order in unprivileged_init. */

  FD_SCRATCH_ALLOC_INIT( l, scratch );
  fd_snapshot_tile_ctx_t * ctx = FD_SCRATCH_ALLOC_APPEND( l, alignof(fd_snapshot_tile_ctx_t), sizeof(fd_snapshot_tile_ctx_t) );
  memset( ctx, 0, sizeof(fd_snapshot_tile_ctx_t) );
  void * tpool_worker_mem    = FD_SCRATCH_ALLOC_APPEND( l, FD_SCRATCH_ALIGN_DEFAULT, tile->snaps.tpool_thread_count * TPOOL_WORKER_MEM_SZ );
  void * scratch_smem        = FD_SCRATCH_ALLOC_APPEND( l, fd_scratch_smem_align(), fd_scratch_smem_footprint( SCRATCH_MAX    ) );
  void * scratch_fmem        = FD_SCRATCH_ALLOC_APPEND( l, fd_scratch_fmem_align(), fd_scratch_fmem_footprint( SCRATCH_DEPTH ) );
  ulong  scratch_alloc_mem   = FD_SCRATCH_ALLOC_FINI  ( l, scratch_align() );

  ctx->interval    = tile->snaps.interval;
  ctx->out_dir     = tile->snaps.out_dir;
  ctx->tmp_fd      = tile->snaps.tmp_fd;
  ctx->snapshot_fd = tile->snaps.snapshot_fd;

  /**********************************************************************/
  /* tpool                                                              */
  /**********************************************************************/

  FD_LOG_WARNING(("NUM THREADS: %lu", tile->snaps.tpool_thread_count));

  if( FD_LIKELY( tile->snaps.tpool_thread_count > 1 ) ) {
    tpool_snap_boot( topo, tile->snaps.tpool_thread_count );
  }
  ctx->tpool = fd_tpool_init( ctx->tpool_mem, tile->snaps.tpool_thread_count );

  if( FD_LIKELY( tile->snaps.tpool_thread_count > 1 ) ) {
    /* start the tpool workers */
    for( ulong i = 1UL; i<tile->snaps.tpool_thread_count; i++ ) {
      if( fd_tpool_worker_push( ctx->tpool, i, (uchar *)tpool_worker_mem + TPOOL_WORKER_MEM_SZ*(i - 1U), TPOOL_WORKER_MEM_SZ ) == NULL ) {
        FD_LOG_ERR(( "failed to launch worker" ));
      }
    }
  }

  if( ctx->tpool == NULL ) {
    FD_LOG_ERR(("failed to create thread pool"));
  }

  /**********************************************************************/
  /* funk                                                               */
  /**********************************************************************/

  ulong funk_obj_id = fd_pod_queryf_ulong( topo->props, ULONG_MAX, "funk" );
  FD_TEST( funk_obj_id!=ULONG_MAX );
  ctx->funk = fd_funk_join( fd_topo_obj_laddr( topo, funk_obj_id ) );
  if( ctx->funk==NULL ) {
    FD_LOG_ERR(( "no funk" ));
  }

  /**********************************************************************/
  /* status cache                                                       */
  /**********************************************************************/

  ulong status_cache_obj_id = fd_pod_queryf_ulong( topo->props, ULONG_MAX, "txncache" );
  FD_TEST( status_cache_obj_id!=ULONG_MAX );
  ctx->status_cache = fd_txncache_join( fd_topo_obj_laddr( topo, status_cache_obj_id ) );
  if( ctx->funk==NULL ) {
    FD_LOG_ERR(( "no funk" ));
  }

  /**********************************************************************/
  /* scratch                                                            */
  /**********************************************************************/

  fd_scratch_attach( scratch_smem, scratch_fmem, SCRATCH_MAX, SCRATCH_DEPTH );

  if( FD_UNLIKELY( scratch_alloc_mem != ( (ulong)scratch + scratch_footprint( tile ) ) ) ) {
    FD_LOG_ERR( ( "scratch_alloc_mem did not match scratch_footprint diff: %lu alloc: %lu footprint: %lu",
          scratch_alloc_mem - (ulong)scratch - scratch_footprint( tile ),
          scratch_alloc_mem,
          (ulong)scratch + scratch_footprint( tile ) ) );
  }

  /**********************************************************************/
  /*  constipated fseq                                                  */
  /**********************************************************************/

  ulong constipated_obj_id = fd_pod_queryf_ulong( topo->props, ULONG_MAX, "constipate" );
  FD_TEST( constipated_obj_id!=ULONG_MAX );
  ctx->is_constipated = fd_fseq_join( fd_topo_obj_laddr( topo, constipated_obj_id ) );
  if( FD_UNLIKELY( !ctx->is_constipated ) ) {
    FD_LOG_ERR(( "replay tile has no constipated fseq" ));
  }
  fd_fseq_update( ctx->is_constipated, 0UL );
  FD_TEST( 0UL==fd_fseq_query( ctx->is_constipated ) );

}

static void
after_credit( fd_snapshot_tile_ctx_t * ctx         FD_PARAM_UNUSED,
              fd_stem_context_t *      stem        FD_PARAM_UNUSED,
              int *                    opt_poll_in FD_PARAM_UNUSED,
              int *                    charge_busy FD_PARAM_UNUSED ) {
    
  ulong is_constipated = fd_fseq_query( ctx->is_constipated );

  if( FD_UNLIKELY( is_constipated ) ) {

    FD_LOG_WARNING(("STARTING TO CREATE A NEW SNAPSHOT"));

    uchar * mem = fd_valloc_malloc( fd_scratch_virtual(), FD_ACC_MGR_ALIGN, FD_ACC_MGR_FOOTPRINT );

    FD_TEST( ctx->tpool );

    fd_snapshot_ctx_t snapshot_ctx = {
      .slot           = is_constipated,
      .out_dir        = ctx->out_dir,
      .is_incremental = 0,
      .valloc         = fd_scratch_virtual(),
      .acc_mgr        = fd_acc_mgr_new( mem, ctx->funk ),
      .status_cache   = ctx->status_cache,
      .tmp_fd         = ctx->tmp_fd,
      .snapshot_fd    = ctx->snapshot_fd,
      .tpool          = ctx->tpool
    };

    /* If this isn't the first snapshot that this tile is creating, the
       permissions should be made to not acessible by users and should be
       renamed to the constant file that is expected. */

    char prev_filename[ FD_SNAPSHOT_DIR_MAX ];
    snprintf( prev_filename, FD_SNAPSHOT_DIR_MAX, "/proc/self/fd/%d", ctx->snapshot_fd );
    long len = readlink( prev_filename, prev_filename, FD_SNAPSHOT_DIR_MAX );
    if( FD_UNLIKELY( len==-1L ) ) {
      FD_LOG_ERR(( "Failed to readlink the snapshot file" ));
    }
    prev_filename[ len ] = '\0';

    char new_filename[ FD_SNAPSHOT_DIR_MAX ];
    snprintf( new_filename, FD_SNAPSHOT_DIR_MAX, "%s/%s", ctx->out_dir, FD_SNAPSHOT_TMP_ARCHIVE_ZSTD );

    rename( prev_filename, new_filename );

    if( FD_UNLIKELY( fd_snapshot_create_new_snapshot( &snapshot_ctx ) ) ) {
      FD_LOG_ERR(( "Failed to create a new snapshot" ));
    }

    FD_LOG_NOTICE(( "Done creating a snapshot" ));

    fd_fseq_update( ctx->is_constipated, 0UL );

  }

}

static ulong
populate_allowed_seccomp( fd_topo_t const *      topo,
                          fd_topo_tile_t const * tile,
                          ulong                  out_cnt,
                          struct sock_filter *   out ) {
  (void)topo;

  FD_LOG_WARNING(("DONE HERE 4"));

  populate_sock_filter_policy_snaps( out_cnt, out, (uint)fd_log_private_logfile_fd(), (uint)tile->snaps.tmp_fd, (uint)tile->snaps.snapshot_fd );
  return sock_filter_policy_snaps_instr_cnt;
}

static ulong
populate_allowed_fds( fd_topo_t const *      topo,
                      fd_topo_tile_t const * tile,
                      ulong                  out_fds_cnt,
                      int *                  out_fds ) {
  (void)topo;
  (void)tile;

  FD_LOG_WARNING(("DONE HERE 5"));

  if( FD_UNLIKELY( out_fds_cnt<2UL ) ) {
    FD_LOG_ERR(( "out_fds_cnt %lu", out_fds_cnt ));
  }

  ulong out_cnt = 0UL;
  out_fds[ out_cnt++ ] = 2; /* stderr */
  if( FD_LIKELY( -1!=fd_log_private_logfile_fd() ) )
    out_fds[ out_cnt++ ] = fd_log_private_logfile_fd(); /* logfile */

  out_fds[ out_cnt++ ] = tile->snaps.tmp_fd;
  out_fds[ out_cnt++ ] = tile->snaps.snapshot_fd;
  return out_cnt;
}

#define STEM_BURST (1UL)

#define STEM_CALLBACK_CONTEXT_TYPE          fd_snapshot_tile_ctx_t
#define STEM_CALLBACK_CONTEXT_ALIGN alignof(fd_snapshot_tile_ctx_t)

#define STEM_CALLBACK_AFTER_CREDIT        after_credit

#include "../../../../disco/stem/fd_stem.c"

fd_topo_run_tile_t fd_tile_snaps = {
  .name                     = "snaps",
  .populate_allowed_seccomp = populate_allowed_seccomp,
  .populate_allowed_fds     = populate_allowed_fds,
  .scratch_align            = scratch_align,
  .scratch_footprint        = scratch_footprint,
  .privileged_init          = privileged_init,
  .unprivileged_init        = unprivileged_init,
  .run                      = stem_run,
};
