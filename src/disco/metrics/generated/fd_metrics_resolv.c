/* THIS FILE IS GENERATED BY gen_metrics.py. DO NOT HAND EDIT. */
#include "fd_metrics_resolv.h"

const fd_metrics_meta_t FD_METRICS_RESOLV[FD_METRICS_RESOLV_TOTAL] = {
    DECLARE_METRIC_COUNTER( RESOLV, NO_BANK_DROP ),
    DECLARE_METRIC_COUNTER( RESOLV, LUT_RESOLVED_INVALID_LOOKUP_INDEX ),
    DECLARE_METRIC_COUNTER( RESOLV, LUT_RESOLVED_ACCOUNT_UNINITIALIZED ),
    DECLARE_METRIC_COUNTER( RESOLV, LUT_RESOLVED_INVALID_ACCOUNT_DATA ),
    DECLARE_METRIC_COUNTER( RESOLV, LUT_RESOLVED_INVALID_ACCOUNT_OWNER ),
    DECLARE_METRIC_COUNTER( RESOLV, LUT_RESOLVED_ACCOUNT_NOT_FOUND ),
    DECLARE_METRIC_COUNTER( RESOLV, LUT_RESOLVED_SUCCESS ),
    DECLARE_METRIC_COUNTER( RESOLV, BLOCKHASH_EXPIRED ),
};
