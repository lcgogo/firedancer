#ifndef HEADER_fd_src_flamenco_runtime_fd_types_custom
#define HEADER_fd_src_flamenco_runtime_fd_types_custom

#include "fd_types_meta.h"
#include "fd_bincode.h"
#include "../../ballet/ed25519/fd_ed25519.h"
#include "../../ballet/txn/fd_txn.h"
#include "../../util/net/fd_ip4.h"

typedef void
(* fd_types_walk_fn_t)( void *       self,
                        void const * arg,
                        char const * name,
                        int          type,
                        char const * type_name,
                        uint         level );

#define FD_HASH_FOOTPRINT (32UL)
#define FD_HASH_ALIGN (8UL)
#define FD_PUBKEY_FOOTPRINT FD_HASH_FOOTPRINT
#define FD_PUBKEY_ALIGN FD_HASH_ALIGN

union __attribute__((aligned(FD_HASH_ALIGN))) fd_hash {
  uchar hash[ FD_HASH_FOOTPRINT ];
  uchar key [ FD_HASH_FOOTPRINT ]; // Making fd_hash and fd_pubkey interchangable

  // Generic type specific accessors
  ulong ul  [ FD_HASH_FOOTPRINT / sizeof(ulong) ];
  uchar uc  [ FD_HASH_FOOTPRINT ];
};

typedef union fd_hash fd_hash_t;
typedef union fd_hash fd_pubkey_t;

union fd_signature {
  uchar uc[ 64 ];
  ulong ul[  8 ];
};

typedef union fd_signature fd_signature_t;

FD_PROTOTYPES_BEGIN

static inline void
fd_hash_new( fd_hash_t * self FD_FN_UNUSED ) {}

static inline int
fd_hash_decode( fd_hash_t *               self,
                fd_bincode_decode_ctx_t * ctx ) {
  return fd_bincode_bytes_decode( &self->hash[0], sizeof(self->hash), ctx );
}

static inline void
fd_hash_destroy( fd_hash_t const *          self FD_FN_UNUSED,
                 fd_bincode_destroy_ctx_t * ctx  FD_FN_UNUSED ) {}

static inline ulong
fd_hash_size( fd_hash_t const * self FD_FN_UNUSED ) {
  return 32;
}

static inline int
fd_hash_encode( fd_hash_t const *         self,
                fd_bincode_encode_ctx_t * ctx ) {
  return fd_bincode_bytes_encode( &self->hash[0], sizeof(self->hash), ctx );
}

static inline void
fd_hash_walk( void *             w,
              fd_hash_t const *  self,
              fd_types_walk_fn_t fun,
              char const *       name,
              uint               level ) {
  fun( w, self->hash, name, FD_FLAMENCO_TYPE_HASH256, name, level );
}

#define fd_hash_check_zero(_x) (!((_x)->ul[0] | (_x)->ul[1] | (_x)->ul[2] | (_x)->ul[3]))
#define fd_hash_set_zero(_x)   {((_x)->ul[0] = 0); ((_x)->ul[1] = 0); ((_x)->ul[2] = 0); ((_x)->ul[3] = 0);}

#define fd_pubkey_new         fd_hash_new
#define fd_pubkey_decode      fd_hash_decode
#define fd_pubkey_encode      fd_hash_encode
#define fd_pubkey_destroy     fd_hash_destroy
#define fd_pubkey_size        fd_hash_size
#define fd_pubkey_check_zero  fd_hash_check_zero
#define fd_pubkey_set_zero    fd_hash_set_zero
#define fd_pubkey_walk        fd_hash_walk

/* https://github.com/solana-labs/solana/blob/8f2c8b8388a495d2728909e30460aa40dcc5d733/sdk/program/src/epoch_schedule.rs#L26 */
struct fd_epoch_schedule {
  ulong slots_per_epoch;
  ulong leader_schedule_slot_offset;
  uchar warmup;
  uchar _pad11[7];  /* Padding, must be zero */
  ulong first_normal_epoch;
  ulong first_normal_slot;
};
typedef struct fd_epoch_schedule fd_epoch_schedule_t;
#define FD_EPOCH_SCHEDULE_FOOTPRINT sizeof(fd_epoch_schedule_t)
#define FD_EPOCH_SCHEDULE_ALIGN (8UL)

void fd_epoch_schedule_new(fd_epoch_schedule_t* self);
int fd_epoch_schedule_decode(fd_epoch_schedule_t* self, fd_bincode_decode_ctx_t * ctx);
int fd_epoch_schedule_encode(fd_epoch_schedule_t const * self, fd_bincode_encode_ctx_t * ctx);
void fd_epoch_schedule_destroy(fd_epoch_schedule_t* self, fd_bincode_destroy_ctx_t * ctx);
void fd_epoch_schedule_walk(void * w, fd_epoch_schedule_t const * self, fd_types_walk_fn_t fun, const char *name, uint level);
ulong fd_epoch_schedule_size(fd_epoch_schedule_t const * self);

/* Index structure needed for transaction status (metadata) blocks */
struct fd_txnstatusidx {
    fd_ed25519_sig_t sig;
    ulong offset;
    ulong status_sz;
};
typedef struct fd_txnstatusidx fd_txnstatusidx_t;

/* Signature **********************************************************/

static inline void
fd_signature_new( fd_signature_t * self FD_FN_UNUSED ) {}

static inline int
fd_signature_decode( fd_signature_t *          self,
                     fd_bincode_decode_ctx_t * ctx ) {
  return fd_bincode_bytes_decode( &self->uc[0], 64UL, ctx );
}

static inline void
fd_signature_destroy( fd_signature_t const *     self FD_FN_UNUSED,
                      fd_bincode_destroy_ctx_t * ctx  FD_FN_UNUSED ) {}

FD_FN_CONST static inline ulong
fd_signature_size( fd_signature_t const * self FD_FN_UNUSED ) {
  return 64;
}

static inline int
fd_signature_encode( fd_signature_t const *    self,
                     fd_bincode_encode_ctx_t * ctx ) {
  return fd_bincode_bytes_encode( &self->uc[0], 64UL, ctx );
}

static inline void
fd_signature_walk( void *                 w,
                   fd_signature_t const * self,
                   fd_types_walk_fn_t     fun,
                   char const *           name,
                   uint                   level ) {
  fun( w, self->uc, name, FD_FLAMENCO_TYPE_SIG512, name, level );
}

/* IPv4 ***************************************************************/

typedef uint fd_gossip_ip4_addr_t;

static inline void
fd_gossip_ip4_addr_new( fd_gossip_ip4_addr_t * self FD_FN_UNUSED ) {}

static inline int
fd_gossip_ip4_addr_decode( fd_gossip_ip4_addr_t *    self,
                           fd_bincode_decode_ctx_t * ctx ) {
  return fd_bincode_uint32_decode( self, ctx );
}

static inline void
fd_gossip_ip4_addr_destroy( fd_gossip_ip4_addr_t const * self FD_FN_UNUSED,
                            fd_bincode_destroy_ctx_t *   ctx  FD_FN_UNUSED ) {}

FD_FN_CONST static inline ulong
fd_gossip_ip4_addr_size( fd_gossip_ip4_addr_t const * self FD_FN_UNUSED ) {
  return 4UL;
}

static inline int
fd_gossip_ip4_addr_encode( fd_gossip_ip4_addr_t const * self,
                           fd_bincode_encode_ctx_t *    ctx ) {
  return fd_bincode_uint32_encode( self, ctx );
}

void
fd_gossip_ip4_addr_walk( void *                       w,
                         fd_gossip_ip4_addr_t const * self,
                         fd_types_walk_fn_t           fun,
                         char const *                 name,
                         uint                         level );

/* IPv6 ***************************************************************/

union fd_gossip_ip6_addr {
  uchar  uc[ 16 ];
  ushort us[  8 ];
  uint   ul[  4 ];
};

typedef union fd_gossip_ip6_addr fd_gossip_ip6_addr_t;

static inline void
fd_gossip_ip6_addr_new( fd_gossip_ip6_addr_t * self FD_FN_UNUSED ) {}

static inline int
fd_gossip_ip6_addr_decode( fd_gossip_ip6_addr_t *    self,
                           fd_bincode_decode_ctx_t * ctx ) {
  return fd_bincode_bytes_decode( &self->uc[0], 16UL, ctx );
}

static inline void
fd_gossip_ip6_addr_destroy( fd_gossip_ip6_addr_t const * self FD_FN_UNUSED,
                            fd_bincode_destroy_ctx_t *   ctx  FD_FN_UNUSED ) {}

FD_FN_CONST static inline ulong
fd_gossip_ip6_addr_size( fd_gossip_ip6_addr_t const * self FD_FN_UNUSED ) {
  return 16UL;
}

static inline int
fd_gossip_ip6_addr_encode( fd_gossip_ip6_addr_t const * self,
                           fd_bincode_encode_ctx_t *    ctx ) {
  return fd_bincode_bytes_encode( &self->uc[0], 16UL, ctx );
}

void
fd_gossip_ip6_addr_walk( void *                       w,
                         fd_gossip_ip6_addr_t const * self,
                         fd_types_walk_fn_t           fun,
                         char const *                 name,
                         uint                         level );

/* Transaction wrapper ************************************************/

/* fd_flamenco_txn_t is yet another fd_txn_t wrapper.
   This should die as soon as we have a better stubs generator. */

struct fd_flamenco_txn {
  union {
    uchar                  txn_buf[ FD_TXN_MAX_SZ ];
    __extension__ fd_txn_t txn[0];
  };
  uchar raw[ FD_TXN_MTU ];
  ulong raw_sz;
};

typedef struct fd_flamenco_txn fd_flamenco_txn_t;

static inline void
fd_flamenco_txn_new( fd_flamenco_txn_t * self FD_FN_UNUSED ) {}

int
fd_flamenco_txn_decode( fd_flamenco_txn_t *       self,
                        fd_bincode_decode_ctx_t * ctx );

static inline void
fd_flamenco_txn_destroy( fd_flamenco_txn_t const *  self FD_FN_UNUSED,
                         fd_bincode_destroy_ctx_t * ctx  FD_FN_UNUSED ) {}

FD_FN_CONST static inline ulong
fd_flamenco_txn_size( fd_flamenco_txn_t const * self FD_FN_UNUSED ) {
  return self->raw_sz;
}

static inline int
fd_flamenco_txn_encode( fd_flamenco_txn_t const * self,
                        fd_bincode_encode_ctx_t * ctx ) {
  return fd_bincode_bytes_encode( self->raw, self->raw_sz, ctx );
}

static inline void
fd_flamenco_txn_walk( void *                    w,
                      fd_flamenco_txn_t const * self,
                      fd_types_walk_fn_t        fun,
                      char const *              name,
                      uint                      level ) {

  static uchar const zero[ 64 ]={0};
  fd_txn_t const *   txn  = self->txn;
  uchar const *      sig0 = zero;

  if( FD_LIKELY( txn->signature_cnt > 0 ) )
    sig0 = fd_txn_get_signatures( txn, self->raw )[0];

  /* For now, just print the transaction's signature */
  fun( w, sig0, name, FD_FLAMENCO_TYPE_SIG512, "txn", level );
}

FD_PROTOTYPES_END

#endif /* HEADER_fd_src_flamenco_runtime_fd_types_custom */
