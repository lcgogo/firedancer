struct poh_link {
  fd_frag_meta_t * mcache;
  ulong            depth;
  ulong            tx_seq;

  void *           mem;
  void *           dcache;
  ulong            chunk0;
  ulong            wmark;
  ulong            chunk;

  ulong            cr_avail;
  ulong            rx_cnt;
  ulong *          rx_fseqs[ 32UL ];
};

typedef struct poh_link poh_link_t;

static void
poh_link_wait_credit( poh_link_t * link ) {
  if( FD_LIKELY( link->cr_avail ) ) return;

  while( 1 ) {
    ulong cr_query = ULONG_MAX;
    for( ulong i=0UL; i<link->rx_cnt; i++ ) {
      ulong const * _rx_seq = link->rx_fseqs[ i ];
      ulong rx_seq = FD_VOLATILE_CONST( *_rx_seq );
      ulong rx_cr_query = (ulong)fd_long_max( (long)link->depth - fd_long_max( fd_seq_diff( link->tx_seq, rx_seq ), 0L ), 0L );
      cr_query = fd_ulong_min( rx_cr_query, cr_query );
    }
    if( FD_LIKELY( cr_query>0UL ) ) {
      link->cr_avail = cr_query;
      break;
    }
    FD_SPIN_PAUSE();
  }
}

static void
poh_link_publish( poh_link_t *  link,
                  ulong         sig,
                  uchar const * data,
                  ulong         data_sz ) {
  while( FD_UNLIKELY( !FD_VOLATILE_CONST( link->mcache ) ) ) FD_SPIN_PAUSE();
  if( FD_UNLIKELY( !link->mem ) ) return; /* link not enabled, don't publish */
  poh_link_wait_credit( link );

  uchar * dst = (uchar *)fd_chunk_to_laddr( link->mem, link->chunk );
  fd_memcpy( dst, data, data_sz );
  ulong tspub = (ulong)fd_frag_meta_ts_comp( fd_tickcount() );
  fd_mcache_publish( link->mcache, link->depth, link->tx_seq, sig, link->chunk, data_sz, 0UL, 0UL, tspub );
  link->chunk = fd_dcache_compact_next( link->chunk, data_sz, link->chunk0, link->wmark );
  link->cr_avail--;
  link->tx_seq++;
}

static void
poh_link_init( poh_link_t *     link,
               fd_topo_t *      topo,
               fd_topo_tile_t * tile,
               ulong            out_idx ) {
  fd_topo_link_t * topo_link = &topo->links[ tile->out_link_id[ out_idx ] ];
  fd_topo_wksp_t * wksp = &topo->workspaces[ topo->objs[ topo_link->dcache_obj_id ].wksp_id ];

  link->mem      = wksp->wksp;
  link->depth    = fd_mcache_depth( topo_link->mcache );
  link->tx_seq   = 0UL;
  link->dcache   = topo_link->dcache;
  link->chunk0   = fd_dcache_compact_chunk0( wksp->wksp, topo_link->dcache );
  link->wmark    = fd_dcache_compact_wmark ( wksp->wksp, topo_link->dcache, topo_link->mtu );
  link->chunk    = link->chunk0;
  link->cr_avail = 0UL;
  link->rx_cnt   = 0UL;
  for( ulong i=0UL; i<topo->tile_cnt; i++ ) {
    fd_topo_tile_t * _tile = &topo->tiles[ i ];
    for( ulong j=0UL; j<_tile->in_cnt; j++ ) {
      if( _tile->in_link_id[ j ]==topo_link->id && _tile->in_link_reliable[ j ] ) {
        FD_TEST( link->rx_cnt<32UL );
        link->rx_fseqs[ link->rx_cnt++ ] = _tile->in_link_fseq[ j ];
        break;
      }
    }
  }
  FD_COMPILER_MFENCE();
  link->mcache = topo_link->mcache;
  FD_COMPILER_MFENCE();
  FD_TEST( link->mcache );
}

typedef struct {
  ulong       idx;
  fd_wksp_t * mem;
  ulong       chunk0;
  ulong       wmark;
  ulong       chunk;
} fd_poh_out_ctx_t;

static inline fd_poh_out_ctx_t
out1( fd_topo_t const *      topo,
      fd_topo_tile_t const * tile,
      char const *           name ) {
  ulong idx = ULONG_MAX;

  for( ulong i=0UL; i<tile->out_cnt; i++ ) {
    fd_topo_link_t const * link = &topo->links[ tile->out_link_id[ i ] ];
    if( !strcmp( link->name, name ) ) {
      if( FD_UNLIKELY( idx!=ULONG_MAX ) ) FD_LOG_ERR(( "tile %s:%lu had multiple output links named %s but expected one", tile->name, tile->kind_id, name ));
      idx = i;
    }
  }

  if( FD_UNLIKELY( idx==ULONG_MAX ) ) FD_LOG_ERR(( "tile %s:%lu had no output link named %s", tile->name, tile->kind_id, name ));

  void * mem = topo->workspaces[ topo->objs[ topo->links[ tile->out_link_id[ idx ] ].dcache_obj_id ].wksp_id ].wksp;
  ulong chunk0 = fd_dcache_compact_chunk0( mem, topo->links[ tile->out_link_id[ idx ] ].dcache );
  ulong wmark  = fd_dcache_compact_wmark ( mem, topo->links[ tile->out_link_id[ idx ] ].dcache, topo->links[ tile->out_link_id[ idx ] ].mtu );

  return (fd_poh_out_ctx_t){ .idx = idx, .mem = mem, .chunk0 = chunk0, .wmark = wmark, .chunk = chunk0 };
}

struct fd_replay_complete_msg {
  ulong slot;
  ulong total_txn_count;
  ulong nonvote_txn_count;
  ulong failed_txn_count;
  ulong nonvote_failed_txn_count;
  ulong compute_units;
  ulong transaction_fee;
  ulong priority_fee;
  ulong parent_slot;
};
typedef struct fd_replay_complete_msg fd_replay_complete_msg_t;
