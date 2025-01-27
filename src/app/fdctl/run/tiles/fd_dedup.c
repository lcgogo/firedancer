#include "../../../../disco/tiles.h"
#include "fd_verify.h"

#include "generated/dedup_seccomp.h"

#include "../../../../disco/metrics/fd_metrics.h"

#include <linux/unistd.h>

/* fd_dedup provides services to deduplicate multiple streams of input
   fragments and present them to a mix of reliable and unreliable
   consumers as though they were generated by a single multi-stream
   producer.

   The dedup tile is simply a wrapper around the mux tile, that also
   checks the transaction signature field for duplicates and filters
   them out. */

#define GOSSIP_IN_IDX (0UL) /* Frankendancer and Firedancer */
#define VOTER_IN_IDX  (1UL) /* Firedancer only */

/* fd_dedup_in_ctx_t is a context object for each in (producer) mcache
   connected to the dedup tile. */

typedef struct {
  fd_wksp_t * mem;
  ulong       chunk0;
  ulong       wmark;
} fd_dedup_in_ctx_t;

/* fd_dedup_ctx_t is the context object provided to callbacks from the
   mux tile, and contains all state needed to progress the tile. */

typedef struct {
  ulong   tcache_depth;   /* == fd_tcache_depth( tcache ), depth of this dedups's tcache (const) */
  ulong   tcache_map_cnt; /* == fd_tcache_map_cnt( tcache ), number of slots to use for tcache map (const) */
  ulong * tcache_sync;    /* == fd_tcache_oldest_laddr( tcache ), local join to the oldest key in the tcache */
  ulong * tcache_ring;
  ulong * tcache_map;

  /* The first unparsed_in_cnt in links do not have the parsed fd_txn_t
     in the payload trailer. */
  ulong             unparsed_in_cnt;
  fd_dedup_in_ctx_t in[ 64UL ];

  fd_wksp_t * out_mem;
  ulong       out_chunk0;
  ulong       out_wmark;
  ulong       out_chunk;

  ulong       hashmap_seed;

  struct {
    ulong dedup_fail_cnt;
  } metrics;
} fd_dedup_ctx_t;

FD_FN_CONST static inline ulong
scratch_align( void ) {
  return alignof( fd_dedup_ctx_t );
}

FD_FN_PURE static inline ulong
scratch_footprint( fd_topo_tile_t const * tile ) {
  ulong l = FD_LAYOUT_INIT;
  l = FD_LAYOUT_APPEND( l, alignof( fd_dedup_ctx_t ), sizeof( fd_dedup_ctx_t ) );
  l = FD_LAYOUT_APPEND( l, fd_tcache_align(), fd_tcache_footprint( tile->dedup.tcache_depth, 0UL ) );
  return FD_LAYOUT_FINI( l, scratch_align() );
}

static inline void
metrics_write( fd_dedup_ctx_t * ctx ) {
  FD_MCNT_SET( DEDUP, TRANSACTION_DEDUP_FAILURE,  ctx->metrics.dedup_fail_cnt );
}

/* during_frag is called between pairs for sequence number checks, as
   we are reading incoming frags.  We don't actually need to copy the
   fragment here, flow control prevents it getting overrun, and
   downstream consumers could reuse the same chunk and workspace to
   improve performance.

   The bounds checking and copying here are defensive measures,

    * In a functioning system, the bounds checking should never fail,
      but we want to prevent an attacker with code execution on a producer
      tile from trivially being able to jump to a consumer tile with
      out of bounds chunks.

    * For security reasons, we have chosen to isolate all workspaces from
      one another, so for example, if the QUIC tile is compromised with
      RCE, it cannot wait until the sigverify tile has verified a transaction,
      and then overwrite the transaction while it's being processed by the
      banking stage. */

static inline void
during_frag( fd_dedup_ctx_t * ctx,
             ulong            in_idx,
             ulong            seq,
             ulong            sig,
             ulong            chunk,
             ulong            sz ) {
  (void)seq;
  (void)sig;

  if( FD_UNLIKELY( chunk<ctx->in[ in_idx ].chunk0 || chunk>ctx->in[ in_idx ].wmark || sz > FD_TPU_DCACHE_MTU ) )
    FD_LOG_ERR(( "chunk %lu %lu corrupt, not in range [%lu,%lu]", chunk, sz, ctx->in[ in_idx ].chunk0, ctx->in[ in_idx ].wmark ));

  uchar * src = (uchar *)fd_chunk_to_laddr( ctx->in[in_idx].mem, chunk );
  uchar * dst = (uchar *)fd_chunk_to_laddr( ctx->out_mem, ctx->out_chunk );

  fd_memcpy( dst, src, sz );
}

/* After the transaction has been fully received, and we know we were
   not overrun while reading it, check if it's a duplicate of a prior
   transaction.

   If the transaction came in from the gossip link, then it hasn't been
   parsed by us.  So parse it here if necessary. */

static inline void
after_frag( fd_dedup_ctx_t *    ctx,
            ulong               in_idx,
            ulong               seq,
            ulong               sig,
            ulong               sz,
            ulong               tsorig,
            fd_stem_context_t * stem ) {
  (void)seq;
  (void)sig;

  /* Transactions coming from verify tile, already parsed.
     We need to reconstruct fd_txn_t * txn, because we need the
     tx signature to compute the dedup tag.
     To find the position of fd_txn_t * txn, we need the (udp)
     payload_sz that's stored as ushort at the end of the
     dcache_entry. */
  uchar    * dcache_entry = fd_chunk_to_laddr( ctx->out_mem, ctx->out_chunk );
  ushort * payload_sz_p = (ushort *)(dcache_entry + sz - sizeof(ushort));
  ulong payload_sz = *payload_sz_p;
  ulong txn_off = fd_ulong_align_up( payload_sz, 2UL );
  fd_txn_t * txn = (fd_txn_t *)(dcache_entry + txn_off);

  if ( FD_UNLIKELY( in_idx < ctx->unparsed_in_cnt ) ) {
    /* Transactions coming in from these links are not parsed.

       We'll need to parse it so it's ready for downstream consumers.
       Equally importantly, we need to parse to extract the signature
       for dedup.  Just parse it right into the output dcache. */

    /* Here, *opt_sz is the size of udp payload, as the tx has not
       been parsed yet. Code here is similar to the verify tile. */
    ulong payload_sz = sz;
    ulong txn_off = fd_ulong_align_up( payload_sz, 2UL );

    /* Ensure sufficient trailing space for parsing. */
    if( FD_UNLIKELY( txn_off>FD_TPU_DCACHE_MTU - FD_TXN_MAX_SZ - sizeof(ushort)) ) {
      FD_LOG_ERR(( "got malformed txn (sz %lu) insufficient space left in dcache for fd_txn_t", payload_sz ));
    }

    txn = (fd_txn_t *)(dcache_entry + txn_off);
    ulong txn_t_sz = fd_txn_parse( dcache_entry, payload_sz, txn, NULL );
    if( FD_UNLIKELY( !txn_t_sz ) ) FD_LOG_ERR(( "fd_txn_parse failed for vote transactions that should have been sigverified" ));

    /* Increment on GOSSIP_IN_IDX but not VOTER_IN_IDX */
    FD_MCNT_INC( DEDUP, GOSSIPED_VOTES_RECEIVED, 1UL - in_idx );

    /* Write payload_sz into trailer.
       fd_txn_parse always returns a multiple of 2 so this sz is
       correctly aligned. */
    payload_sz_p = (ushort *)((ulong)txn + txn_t_sz);
    *payload_sz_p = (ushort)payload_sz;

    /* End of parsed message. */

    /* Paranoia post parsing. */
    ulong new_sz = ( (ulong)payload_sz_p + sizeof(ushort) ) - (ulong)dcache_entry;
    if( FD_UNLIKELY( new_sz>FD_TPU_DCACHE_MTU ) ) {
      FD_LOG_CRIT(( "memory corruption detected (txn_sz=%lu txn_t_sz=%lu)",
                    payload_sz, txn_t_sz ));
    }

    /* Write new size for mcache. */
    sz = new_sz;
  }

  /* Compute fd_hash(signature) for dedup. */
  ulong ha_dedup_tag = fd_hash( ctx->hashmap_seed, dcache_entry + txn->signature_off, 64UL );

  int is_dup;
  FD_TCACHE_INSERT( is_dup, *ctx->tcache_sync, ctx->tcache_ring, ctx->tcache_depth, ctx->tcache_map, ctx->tcache_map_cnt, ha_dedup_tag );
  if( FD_LIKELY( is_dup ) ) {
    ctx->metrics.dedup_fail_cnt++;
  } else {
    fd_stem_publish( stem, 0UL, 0, ctx->out_chunk, sz, 0UL, tsorig, 0UL );
    ctx->out_chunk = fd_dcache_compact_next( ctx->out_chunk, sz, ctx->out_chunk0, ctx->out_wmark );
  }
}

static void
privileged_init( fd_topo_t *      topo,
                 fd_topo_tile_t * tile ) {
  void * scratch = fd_topo_obj_laddr( topo, tile->tile_obj_id );

  FD_SCRATCH_ALLOC_INIT( l, scratch );
  fd_dedup_ctx_t * ctx = FD_SCRATCH_ALLOC_APPEND( l, alignof( fd_dedup_ctx_t ), sizeof( fd_dedup_ctx_t ) );
  FD_TEST( fd_rng_secure( &ctx->hashmap_seed, 8U ) );
}

static void
unprivileged_init( fd_topo_t *      topo,
                   fd_topo_tile_t * tile ) {
  void * scratch = fd_topo_obj_laddr( topo, tile->tile_obj_id );

  /* Frankendancer has gossip_dedup, verify_dedup+
     Firedancer has gossip_dedup, voter_dedup, verify_dedup+ */
  ulong unparsed_in_cnt = 1;
  if( FD_UNLIKELY( tile->in_cnt<2UL ) ) {
    FD_LOG_ERR(( "dedup tile needs at least two input links, got %lu", tile->in_cnt ));
  } else if( FD_UNLIKELY( strcmp( topo->links[ tile->in_link_id[ GOSSIP_IN_IDX ] ].name, "gossip_dedup" ) ) ) {
    /* We have one link for gossip messages... */
    FD_LOG_ERR(( "dedup tile has unexpected input links %lu %lu %s",
                 tile->in_cnt, GOSSIP_IN_IDX, topo->links[ tile->in_link_id[ GOSSIP_IN_IDX ] ].name ));
  } else {
    /* ...followed by a voter_dedup link if it were the Firedancer topology */
    ulong voter_dedup_idx = fd_topo_find_tile_in_link( topo, tile, "voter_dedup", 0 );
    if( voter_dedup_idx!=ULONG_MAX ) {
      FD_TEST( voter_dedup_idx == VOTER_IN_IDX );
      unparsed_in_cnt = 2;
    } else {
      unparsed_in_cnt = 1;
    }

    /* ...followed by a sequence of verify_dedup links */
    for( ulong i=unparsed_in_cnt; i<tile->in_cnt; i++ ) {
      if( FD_UNLIKELY( strcmp( topo->links[ tile->in_link_id[ i ] ].name, "verify_dedup" ) ) ) {
        FD_LOG_ERR(( "dedup tile has unexpected input links %lu %lu %s",
                     tile->in_cnt, i, topo->links[ tile->in_link_id[ i ] ].name ));
      }
    }
  }

  FD_SCRATCH_ALLOC_INIT( l, scratch );
  fd_dedup_ctx_t * ctx = FD_SCRATCH_ALLOC_APPEND( l, alignof( fd_dedup_ctx_t ), sizeof( fd_dedup_ctx_t ) );
  fd_tcache_t * tcache = fd_tcache_join( fd_tcache_new( FD_SCRATCH_ALLOC_APPEND( l, FD_TCACHE_ALIGN, fd_tcache_footprint( tile->dedup.tcache_depth, 0) ), tile->dedup.tcache_depth, 0 ) );
  if( FD_UNLIKELY( !tcache ) ) FD_LOG_ERR(( "fd_tcache_new failed" ));

  ctx->tcache_depth   = fd_tcache_depth       ( tcache );
  ctx->tcache_map_cnt = fd_tcache_map_cnt     ( tcache );
  ctx->tcache_sync    = fd_tcache_oldest_laddr( tcache );
  ctx->tcache_ring    = fd_tcache_ring_laddr  ( tcache );
  ctx->tcache_map     = fd_tcache_map_laddr   ( tcache );

  FD_TEST( tile->in_cnt<=sizeof( ctx->in )/sizeof( ctx->in[ 0 ] ) );
  ctx->unparsed_in_cnt = unparsed_in_cnt;
  for( ulong i=0; i<tile->in_cnt; i++ ) {
    fd_topo_link_t * link = &topo->links[ tile->in_link_id[ i ] ];
    fd_topo_wksp_t * link_wksp = &topo->workspaces[ topo->objs[ link->dcache_obj_id ].wksp_id ];

    ctx->in[i].mem    = link_wksp->wksp;
    ctx->in[i].chunk0 = fd_dcache_compact_chunk0( ctx->in[i].mem, link->dcache );
    ctx->in[i].wmark  = fd_dcache_compact_wmark ( ctx->in[i].mem, link->dcache, link->mtu );
  }

  ctx->out_mem    = topo->workspaces[ topo->objs[ topo->links[ tile->out_link_id[ 0 ] ].dcache_obj_id ].wksp_id ].wksp;
  ctx->out_chunk0 = fd_dcache_compact_chunk0( ctx->out_mem, topo->links[ tile->out_link_id[ 0 ] ].dcache );
  ctx->out_wmark  = fd_dcache_compact_wmark ( ctx->out_mem, topo->links[ tile->out_link_id[ 0 ] ].dcache, topo->links[ tile->out_link_id[ 0 ] ].mtu );
  ctx->out_chunk  = ctx->out_chunk0;

  ulong scratch_top = FD_SCRATCH_ALLOC_FINI( l, 1UL );
  if( FD_UNLIKELY( scratch_top > (ulong)scratch + scratch_footprint( tile ) ) )
    FD_LOG_ERR(( "scratch overflow %lu %lu %lu", scratch_top - (ulong)scratch - scratch_footprint( tile ), scratch_top, (ulong)scratch + scratch_footprint( tile ) ));
}

static ulong
populate_allowed_seccomp( fd_topo_t const *      topo,
                          fd_topo_tile_t const * tile,
                          ulong                  out_cnt,
                          struct sock_filter *   out ) {
  (void)topo;
  (void)tile;

  populate_sock_filter_policy_dedup( out_cnt, out, (uint)fd_log_private_logfile_fd() );
  return sock_filter_policy_dedup_instr_cnt;
}

static ulong
populate_allowed_fds( fd_topo_t const *      topo,
                      fd_topo_tile_t const * tile,
                      ulong                  out_fds_cnt,
                      int *                  out_fds ) {
  (void)topo;
  (void)tile;

  if( FD_UNLIKELY( out_fds_cnt<2UL ) ) FD_LOG_ERR(( "out_fds_cnt %lu", out_fds_cnt ));

  ulong out_cnt = 0UL;
  out_fds[ out_cnt++ ] = 2; /* stderr */
  if( FD_LIKELY( -1!=fd_log_private_logfile_fd() ) )
    out_fds[ out_cnt++ ] = fd_log_private_logfile_fd(); /* logfile */
  return out_cnt;
}

#define STEM_BURST (1UL)

#define STEM_CALLBACK_CONTEXT_TYPE  fd_dedup_ctx_t
#define STEM_CALLBACK_CONTEXT_ALIGN alignof(fd_dedup_ctx_t)

#define STEM_CALLBACK_METRICS_WRITE metrics_write
#define STEM_CALLBACK_DURING_FRAG   during_frag
#define STEM_CALLBACK_AFTER_FRAG    after_frag

#include "../../../../disco/stem/fd_stem.c"

fd_topo_run_tile_t fd_tile_dedup = {
  .name                     = "dedup",
  .populate_allowed_seccomp = populate_allowed_seccomp,
  .populate_allowed_fds     = populate_allowed_fds,
  .scratch_align            = scratch_align,
  .scratch_footprint        = scratch_footprint,
  .privileged_init          = privileged_init,
  .unprivileged_init        = unprivileged_init,
  .run                      = stem_run,
};
