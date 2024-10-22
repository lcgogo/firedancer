/* THIS FILE IS GENERATED BY gen_metrics.py. DO NOT HAND EDIT. */
#include "fd_metrics_all.h"

const fd_metrics_meta_t FD_METRICS_ALL[FD_METRICS_ALL_TOTAL] = {
    DECLARE_METRIC_GAUGE( TILE, PID ),
    DECLARE_METRIC_GAUGE( TILE, TID ),
    DECLARE_METRIC_COUNTER( TILE, CONTEXT_SWITCH_INVOLUNTARY_COUNT ),
    DECLARE_METRIC_COUNTER( TILE, CONTEXT_SWITCH_VOLUNTARY_COUNT ),
    DECLARE_METRIC_GAUGE( STEM, STATUS ),
    DECLARE_METRIC_GAUGE( STEM, HEARTBEAT ),
    DECLARE_METRIC_GAUGE( STEM, IN_BACKPRESSURE ),
    DECLARE_METRIC_COUNTER( STEM, BACKPRESSURE_COUNT ),
    DECLARE_METRIC_COUNTER( STEM, REGIME_DURATION_NANOS_CAUGHT_UP_HOUSEKEEPING ),
    DECLARE_METRIC_COUNTER( STEM, REGIME_DURATION_NANOS_PROCESSING_HOUSEKEEPING ),
    DECLARE_METRIC_COUNTER( STEM, REGIME_DURATION_NANOS_BACKPRESSURE_HOUSEKEEPING ),
    DECLARE_METRIC_COUNTER( STEM, REGIME_DURATION_NANOS_CAUGHT_UP_PREFRAG ),
    DECLARE_METRIC_COUNTER( STEM, REGIME_DURATION_NANOS_PROCESSING_PREFRAG ),
    DECLARE_METRIC_COUNTER( STEM, REGIME_DURATION_NANOS_BACKPRESSURE_PREFRAG ),
    DECLARE_METRIC_COUNTER( STEM, REGIME_DURATION_NANOS_CAUGHT_UP_POSTFRAG ),
    DECLARE_METRIC_COUNTER( STEM, REGIME_DURATION_NANOS_PROCESSING_POSTFRAG ),
};
const fd_metrics_meta_t FD_METRICS_ALL_LINK_IN[FD_METRICS_ALL_LINK_IN_TOTAL] = {
    DECLARE_METRIC_COUNTER( LINK, CONSUMED_COUNT ),
    DECLARE_METRIC_COUNTER( LINK, CONSUMED_SIZE_BYTES ),
    DECLARE_METRIC_COUNTER( LINK, FILTERED_COUNT ),
    DECLARE_METRIC_COUNTER( LINK, FILTERED_SIZE_BYTES ),
    DECLARE_METRIC_COUNTER( LINK, OVERRUN_POLLING_COUNT ),
    DECLARE_METRIC_COUNTER( LINK, OVERRUN_POLLING_FRAG_COUNT ),
    DECLARE_METRIC_COUNTER( LINK, OVERRUN_READING_COUNT ),
    DECLARE_METRIC_COUNTER( LINK, OVERRUN_READING_FRAG_COUNT ),
};
const fd_metrics_meta_t FD_METRICS_ALL_LINK_OUT[FD_METRICS_ALL_LINK_OUT_TOTAL] = {
    DECLARE_METRIC_COUNTER( LINK, SLOW_COUNT ),
};
const char * FD_METRICS_TILE_KIND_NAMES[FD_METRICS_TILE_KIND_CNT] = {
    "bank",
    "dedup",
    "netrx",
    "nettx",
    "pack",
    "poh",
    "quic",
    "shred",
    "store",
    "verify",
#ifdef FD_HAS_NO_AGAVE
    "replay",
    "storei",
#endif
};
const ulong FD_METRICS_TILE_KIND_SIZES[FD_METRICS_TILE_KIND_CNT] = {
    FD_METRICS_BANK_TOTAL,
    FD_METRICS_DEDUP_TOTAL,
    FD_METRICS_NETRX_TOTAL,
    FD_METRICS_NETTX_TOTAL,
    FD_METRICS_PACK_TOTAL,
    FD_METRICS_POH_TOTAL,
    FD_METRICS_QUIC_TOTAL,
    FD_METRICS_SHRED_TOTAL,
    FD_METRICS_STORE_TOTAL,
    FD_METRICS_VERIFY_TOTAL,
#ifdef FD_HAS_NO_AGAVE
    FD_METRICS_REPLAY_TOTAL,
    FD_METRICS_STOREI_TOTAL,
#endif
};
const fd_metrics_meta_t * FD_METRICS_TILE_KIND_METRICS[FD_METRICS_TILE_KIND_CNT] = {
    FD_METRICS_BANK,
    FD_METRICS_DEDUP,
    FD_METRICS_NETRX,
    FD_METRICS_NETTX,
    FD_METRICS_PACK,
    FD_METRICS_POH,
    FD_METRICS_QUIC,
    FD_METRICS_SHRED,
    FD_METRICS_STORE,
    FD_METRICS_VERIFY,
#ifdef FD_HAS_NO_AGAVE
    FD_METRICS_REPLAY,
    FD_METRICS_STOREI,
#endif
};
