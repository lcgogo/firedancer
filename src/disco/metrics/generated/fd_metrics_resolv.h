/* THIS FILE IS GENERATED BY gen_metrics.py. DO NOT HAND EDIT. */

#include "../fd_metrics_base.h"

#define FD_METRICS_COUNTER_RESOLV_LUT_RESOLVED_OFF  (16UL)
#define FD_METRICS_COUNTER_RESOLV_LUT_RESOLVED_CNT  (7UL)

#define FD_METRICS_COUNTER_RESOLV_LUT_RESOLVED_INVALID_LOOKUP_INDEX_OFF  (16UL)
#define FD_METRICS_COUNTER_RESOLV_LUT_RESOLVED_INVALID_LOOKUP_INDEX_NAME "resolv_lut_resolved_invalid_lookup_index"
#define FD_METRICS_COUNTER_RESOLV_LUT_RESOLVED_INVALID_LOOKUP_INDEX_TYPE (FD_METRICS_TYPE_COUNTER)
#define FD_METRICS_COUNTER_RESOLV_LUT_RESOLVED_INVALID_LOOKUP_INDEX_DESC "Count of address lookup tables resolved (The transaction referenced an index in a LUT that didn't exist)"
#define FD_METRICS_COUNTER_RESOLV_LUT_RESOLVED_INVALID_LOOKUP_INDEX_CVT  (FD_METRICS_CONVERTER_NONE)

#define FD_METRICS_COUNTER_RESOLV_LUT_RESOLVED_ACCOUNT_UNINITIALIZED_OFF  (17UL)
#define FD_METRICS_COUNTER_RESOLV_LUT_RESOLVED_ACCOUNT_UNINITIALIZED_NAME "resolv_lut_resolved_account_uninitialized"
#define FD_METRICS_COUNTER_RESOLV_LUT_RESOLVED_ACCOUNT_UNINITIALIZED_TYPE (FD_METRICS_TYPE_COUNTER)
#define FD_METRICS_COUNTER_RESOLV_LUT_RESOLVED_ACCOUNT_UNINITIALIZED_DESC "Count of address lookup tables resolved (The account referenced as a LUT hasn't been initialized)"
#define FD_METRICS_COUNTER_RESOLV_LUT_RESOLVED_ACCOUNT_UNINITIALIZED_CVT  (FD_METRICS_CONVERTER_NONE)

#define FD_METRICS_COUNTER_RESOLV_LUT_RESOLVED_INVALID_ACCOUNT_DATA_OFF  (18UL)
#define FD_METRICS_COUNTER_RESOLV_LUT_RESOLVED_INVALID_ACCOUNT_DATA_NAME "resolv_lut_resolved_invalid_account_data"
#define FD_METRICS_COUNTER_RESOLV_LUT_RESOLVED_INVALID_ACCOUNT_DATA_TYPE (FD_METRICS_TYPE_COUNTER)
#define FD_METRICS_COUNTER_RESOLV_LUT_RESOLVED_INVALID_ACCOUNT_DATA_DESC "Count of address lookup tables resolved (The account referenced as a LUT couldn't be parsed)"
#define FD_METRICS_COUNTER_RESOLV_LUT_RESOLVED_INVALID_ACCOUNT_DATA_CVT  (FD_METRICS_CONVERTER_NONE)

#define FD_METRICS_COUNTER_RESOLV_LUT_RESOLVED_INVALID_ACCOUNT_OWNER_OFF  (19UL)
#define FD_METRICS_COUNTER_RESOLV_LUT_RESOLVED_INVALID_ACCOUNT_OWNER_NAME "resolv_lut_resolved_invalid_account_owner"
#define FD_METRICS_COUNTER_RESOLV_LUT_RESOLVED_INVALID_ACCOUNT_OWNER_TYPE (FD_METRICS_TYPE_COUNTER)
#define FD_METRICS_COUNTER_RESOLV_LUT_RESOLVED_INVALID_ACCOUNT_OWNER_DESC "Count of address lookup tables resolved (The account referenced as a LUT wasn't owned by the ALUT program ID)"
#define FD_METRICS_COUNTER_RESOLV_LUT_RESOLVED_INVALID_ACCOUNT_OWNER_CVT  (FD_METRICS_CONVERTER_NONE)

#define FD_METRICS_COUNTER_RESOLV_LUT_RESOLVED_ACCOUNT_NOT_FOUND_OFF  (20UL)
#define FD_METRICS_COUNTER_RESOLV_LUT_RESOLVED_ACCOUNT_NOT_FOUND_NAME "resolv_lut_resolved_account_not_found"
#define FD_METRICS_COUNTER_RESOLV_LUT_RESOLVED_ACCOUNT_NOT_FOUND_TYPE (FD_METRICS_TYPE_COUNTER)
#define FD_METRICS_COUNTER_RESOLV_LUT_RESOLVED_ACCOUNT_NOT_FOUND_DESC "Count of address lookup tables resolved (The account referenced as a LUT couldn't be found)"
#define FD_METRICS_COUNTER_RESOLV_LUT_RESOLVED_ACCOUNT_NOT_FOUND_CVT  (FD_METRICS_CONVERTER_NONE)

#define FD_METRICS_COUNTER_RESOLV_LUT_RESOLVED_NO_BANK_OFF  (21UL)
#define FD_METRICS_COUNTER_RESOLV_LUT_RESOLVED_NO_BANK_NAME "resolv_lut_resolved_no_bank"
#define FD_METRICS_COUNTER_RESOLV_LUT_RESOLVED_NO_BANK_TYPE (FD_METRICS_TYPE_COUNTER)
#define FD_METRICS_COUNTER_RESOLV_LUT_RESOLVED_NO_BANK_DESC "Count of address lookup tables resolved (There was no bank available to resolve against)"
#define FD_METRICS_COUNTER_RESOLV_LUT_RESOLVED_NO_BANK_CVT  (FD_METRICS_CONVERTER_NONE)

#define FD_METRICS_COUNTER_RESOLV_LUT_RESOLVED_SUCCESS_OFF  (22UL)
#define FD_METRICS_COUNTER_RESOLV_LUT_RESOLVED_SUCCESS_NAME "resolv_lut_resolved_success"
#define FD_METRICS_COUNTER_RESOLV_LUT_RESOLVED_SUCCESS_TYPE (FD_METRICS_TYPE_COUNTER)
#define FD_METRICS_COUNTER_RESOLV_LUT_RESOLVED_SUCCESS_DESC "Count of address lookup tables resolved (Resolved successfully)"
#define FD_METRICS_COUNTER_RESOLV_LUT_RESOLVED_SUCCESS_CVT  (FD_METRICS_CONVERTER_NONE)


#define FD_METRICS_RESOLV_TOTAL (7UL)
extern const fd_metrics_meta_t FD_METRICS_RESOLV[FD_METRICS_RESOLV_TOTAL];
