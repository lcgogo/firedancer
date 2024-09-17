/* Automatically generated nanopb header */
/* Generated by nanopb-0.4.9-dev */

#ifndef PB_FD_V2_SLOT_V2_PB_H_INCLUDED
#define PB_FD_V2_SLOT_V2_PB_H_INCLUDED

#include "txn_v2.pb.h"

#if PB_PROTO_HEADER_VERSION != 40
#error Regenerate this file with the current version of nanopb generator.
#endif

/* Struct definitions */
typedef struct fd_v2_slot_env {
    /* Transactions within the given slot */
    pb_size_t txns_count;
    struct fd_v2_txn_env *txns;
    /* Slot number for the given slot. */
    uint64_t slot_number;
    /* Previously executed slot. */
    uint64_t prev_slot;
    /* Block height for the given slot. */
    uint64_t block_height;
    /* Previous lamports per signature. */
    uint64_t prev_lamports_per_sig;
    /* Number of signatures in the previous slot. */
    uint64_t parent_signature_cnt;
    /* Fee rate for the slot that is determined at the epoch boundary. */
    uint64_t fee_rate;
    /* Last restart slot. */
    uint64_t last_restart_slot;
    /* Vector of account pubkeys that is set at the epoch boundary. */
    pb_bytes_array_t *vote_accounts;
    pb_bytes_array_t *stake_accounts;
} fd_v2_slot_env_t;

typedef struct fd_v2_slot_effects {
    /* The resulting state after each transaction in the slot */
    pb_size_t txn_effects_count;
    struct fd_v2_txn_effects *txn_effects;
    /* Capitalization for the end of the slot */
    uint64_t capitalization;
} fd_v2_slot_effects_t;


#ifdef __cplusplus
extern "C" {
#endif

/* Initializer values for message structs */
#define FD_V2_SLOT_ENV_INIT_DEFAULT              {0, NULL, 0, 0, 0, 0, 0, 0, 0, NULL, NULL}
#define FD_V2_SLOT_EFFECTS_INIT_DEFAULT          {0, NULL, 0}
#define FD_V2_SLOT_ENV_INIT_ZERO                 {0, NULL, 0, 0, 0, 0, 0, 0, 0, NULL, NULL}
#define FD_V2_SLOT_EFFECTS_INIT_ZERO             {0, NULL, 0}

/* Field tags (for use in manual encoding/decoding) */
#define FD_V2_SLOT_ENV_TXNS_TAG                  1
#define FD_V2_SLOT_ENV_SLOT_NUMBER_TAG           2
#define FD_V2_SLOT_ENV_PREV_SLOT_TAG             3
#define FD_V2_SLOT_ENV_BLOCK_HEIGHT_TAG          4
#define FD_V2_SLOT_ENV_PREV_LAMPORTS_PER_SIG_TAG 5
#define FD_V2_SLOT_ENV_PARENT_SIGNATURE_CNT_TAG  6
#define FD_V2_SLOT_ENV_FEE_RATE_TAG              7
#define FD_V2_SLOT_ENV_LAST_RESTART_SLOT_TAG     8
#define FD_V2_SLOT_ENV_VOTE_ACCOUNTS_TAG         9
#define FD_V2_SLOT_ENV_STAKE_ACCOUNTS_TAG        10
#define FD_V2_SLOT_EFFECTS_TXN_EFFECTS_TAG       1
#define FD_V2_SLOT_EFFECTS_CAPITALIZATION_TAG    2

/* Struct field encoding specification for nanopb */
#define FD_V2_SLOT_ENV_FIELDLIST(X, a) \
X(a, POINTER,  REPEATED, MESSAGE,  txns,              1) \
X(a, STATIC,   SINGULAR, UINT64,   slot_number,       2) \
X(a, STATIC,   SINGULAR, UINT64,   prev_slot,         3) \
X(a, STATIC,   SINGULAR, UINT64,   block_height,      4) \
X(a, STATIC,   SINGULAR, UINT64,   prev_lamports_per_sig,   5) \
X(a, STATIC,   SINGULAR, UINT64,   parent_signature_cnt,   6) \
X(a, STATIC,   SINGULAR, UINT64,   fee_rate,          7) \
X(a, STATIC,   SINGULAR, UINT64,   last_restart_slot,   8) \
X(a, POINTER,  SINGULAR, BYTES,    vote_accounts,     9) \
X(a, POINTER,  SINGULAR, BYTES,    stake_accounts,   10)
#define FD_V2_SLOT_ENV_CALLBACK NULL
#define FD_V2_SLOT_ENV_DEFAULT NULL
#define fd_v2_slot_env_t_txns_MSGTYPE fd_v2_txn_env_t

#define FD_V2_SLOT_EFFECTS_FIELDLIST(X, a) \
X(a, POINTER,  REPEATED, MESSAGE,  txn_effects,       1) \
X(a, STATIC,   SINGULAR, UINT64,   capitalization,    2)
#define FD_V2_SLOT_EFFECTS_CALLBACK NULL
#define FD_V2_SLOT_EFFECTS_DEFAULT NULL
#define fd_v2_slot_effects_t_txn_effects_MSGTYPE fd_v2_txn_effects_t

extern const pb_msgdesc_t fd_v2_slot_env_t_msg;
extern const pb_msgdesc_t fd_v2_slot_effects_t_msg;

/* Defines for backwards compatibility with code written before nanopb-0.4.0 */
#define FD_V2_SLOT_ENV_FIELDS &fd_v2_slot_env_t_msg
#define FD_V2_SLOT_EFFECTS_FIELDS &fd_v2_slot_effects_t_msg

/* Maximum encoded size of messages (where known) */
/* fd_v2_SlotEnv_size depends on runtime parameters */
/* fd_v2_SlotEffects_size depends on runtime parameters */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif