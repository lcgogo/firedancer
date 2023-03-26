#ifndef HEADER_fd_src_tango_quic_fd_quic_private_h
#define HEADER_fd_src_tango_quic_fd_quic_private_h

#include "fd_quic.h"
#include "fd_quic_conn_map.h"
#include "fd_quic_stream.h"
#include "fd_quic_pkt_meta.h"
#include "crypto/fd_quic_crypto_suites.h"
#include "tls/fd_quic_tls.h"

/* FD_QUIC_MAGIC is used to signal the layout of shared memory region
   of an fd_quic_t. */

#define FD_QUIC_MAGIC (0xdadf8cfa01cc5460UL)


enum {
  FD_QUIC_TYPE_INGRESS = 1 << 0,
  FD_QUIC_TYPE_EGRESS  = 1 << 1,

  FD_QUIC_TRANSPORT_PARAMS_RAW_SZ = 1200,
};

#define FD_QUIC_PKT_NUM_UNUSED (~0ul)

/* events for time based processing */
struct fd_quic_event {
  ulong            timeout;

  fd_quic_conn_t * conn; /* connection or NULL */
  /* possibly "type", etc., for other kinds of service */
};
typedef struct fd_quic_event fd_quic_event_t;

/* fd_quic_join_t contains externally provided objects that are
   required to join an fd_quic_t. */

struct fd_quic_join {
  /* User-provided callbacks */

  fd_quic_callbacks_t cb;

  /* fd_aio I/O abstraction */

  fd_aio_t aio_rx; /* owned by fd_quic_t, used by net driver to send rx data to fd_quic_t  */
  fd_aio_t aio_tx; /* owned externally,   used by fd_quic_t  to send tx data to net driver */
};
typedef struct fd_quic_join fd_quic_join_t;

/* fd_quic_state_t is the internal state of an fd_quic_t.  Valid for
 lifetime of join. */

struct fd_quic_state {
  /* Pointer to OpenSSL TLS state (part of quic memory region) */

  fd_quic_tls_t * tls;
  int             keylog_fd;

  /* Various internal state */

  fd_quic_conn_t *       conns;          /* free list of unused connections */
  ulong                  conn_cnt;       /* current number of connections */

  fd_quic_conn_map_t *   conn_map;       /* map connection ids -> connection */

  fd_quic_event_t *      service_queue;  /* priority queue of connections by service time */

  /* crypto members */
  fd_quic_crypto_ctx_t   crypto_ctx[1];  /* crypto context */

  fd_quic_pkt_meta_t *   pkt_meta;       /* records the metadata for the contents
                                            of each sent packet */

  /* flow control - configured initial limits */
  ulong initial_max_data;           /* directly from transport params */
  ulong initial_max_stream_data[4]; /* from 4 transport params indexed by stream type */

  /* next_ephem_udp_port: Next ephemeral UDP port to allocate */
  ushort next_ephem_udp_port;
};
typedef struct fd_quic_state fd_quic_state_t;

/* fd_quic_private is the memory layout of an fd_quic_t */

struct __attribute__((aligned(FD_QUIC_ALIGN))) fd_quic_private {
  ulong            magic;   /* ==FD_QUIC_MAGIC */
  fd_quic_limits_t limits;
  fd_quic_config_t config;
  fd_quic_join_t   join;
  fd_quic_state_t  state;

  /* ... variable length structures follow ... */
};

FD_PROTOTYPES_BEGIN

/* fd_quic_conn_service is called periodically to perform pending
   operations and time based operations.

   args
     quic        managing quic
     conn        connection to service
     now         the current time in ns */
void
fd_quic_conn_service( fd_quic_t *      quic,
                      fd_quic_conn_t * conn,
                      ulong            now );

/* reschedule a connection */
void
fd_quic_reschedule_conn( fd_quic_conn_t * conn,
                         ulong            timeout );

/* Memory management **************************************************/

fd_quic_conn_t *
fd_quic_conn_create( fd_quic_t *               quic,
                     fd_quic_conn_id_t const * our_conn_id,
                     fd_quic_conn_id_t const * peer_conn_id,
                     uint                      dst_ip_addr,
                     ushort                    dst_udp_port,
                     int                       server,
                     uint                      version );

/* fd_quic_conn_free frees up resources related to the connection and
   returns it to the connection free list. */
void
fd_quic_conn_free( fd_quic_t *      quic,
                   fd_quic_conn_t * conn );

void
fd_quic_stream_free( fd_quic_t *        quic,
                     fd_quic_conn_t *   conn,
                     fd_quic_stream_t * stream );

/* Callbacks provided by fd_quic **************************************/

/* used by quic to receive data from network */
int
fd_quic_aio_cb_receive( void *                    context,
                        fd_aio_pkt_info_t const * batch,
                        ulong                     batch_sz,
                        ulong *                   opt_batch_idx );

/* declare callbacks from quic-tls into quic */
int
fd_quic_tls_cb_client_hello( fd_quic_tls_hs_t * hs,
                             void *             context );

int
fd_quic_tls_cb_handshake_data( fd_quic_tls_hs_t *    hs,
                               void *                context,
                               OSSL_ENCRYPTION_LEVEL enc_level,
                               uchar const *         data,
                               ulong                 data_sz );

void
fd_quic_tls_cb_alert( fd_quic_tls_hs_t * hs,
                      void *             context,
                      int                alert );

void
fd_quic_tls_cb_secret( fd_quic_tls_hs_t *           hs,
                       void *                       context,
                       fd_quic_tls_secret_t const * secret );

void
fd_quic_tls_cb_handshake_complete( fd_quic_tls_hs_t * hs,
                                   void *             context  );

int
fd_quic_tls_cb_alpn_select( SSL * ssl,
                            uchar const ** out,
                            uchar       *  outlen,
                            uchar const *  in,
                            uint           inlen,
                            void *         arg );

/* Helpers for calling callbacks **************************************/

static inline ulong
fd_quic_now( fd_quic_t * quic ) {
  return quic->join.cb.now( quic->join.cb.now_ctx );
}

static inline void
fd_quic_cb_conn_new( fd_quic_t *      quic,
                     fd_quic_conn_t * conn ) {
  if( FD_UNLIKELY( !quic->join.cb.conn_new ) ) return;
  quic->join.cb.conn_new( conn, quic->join.cb.quic_ctx );
}

static inline void
fd_quic_cb_conn_hs_complete( fd_quic_t *      quic,
                             fd_quic_conn_t * conn ) {
  if( FD_UNLIKELY( !quic->join.cb.conn_hs_complete ) ) return;
  quic->join.cb.conn_hs_complete( conn, quic->join.cb.quic_ctx );
}

static inline void
fd_quic_cb_conn_final( fd_quic_t *      quic,
                       fd_quic_conn_t * conn ) {
  if( FD_UNLIKELY( !quic->join.cb.conn_final ) ) return;
  quic->join.cb.conn_final( conn, quic->join.cb.quic_ctx );
}

static inline void
fd_quic_cb_stream_new( fd_quic_t *        quic,
                       fd_quic_stream_t * stream,
                       int                stream_type ) {
  if( FD_UNLIKELY( !quic->join.cb.stream_new ) ) return;
  quic->join.cb.stream_new( stream, quic->join.cb.quic_ctx, stream_type );
}

static inline void
fd_quic_cb_stream_receive( fd_quic_t *        quic,
                           fd_quic_stream_t * stream,
                           void *             stream_ctx,
                           uchar const *      data,
                           ulong              data_sz,
                           ulong              offset,
                           int                fin ) {
  if( FD_UNLIKELY( !quic->join.cb.stream_receive ) ) return;
  quic->join.cb.stream_receive( stream, stream_ctx, data, data_sz, offset, fin );
}

static inline void
fd_quic_cb_stream_notify( fd_quic_t *        quic,
                          fd_quic_stream_t * stream,
                          void *             stream_ctx,
                          int                event ) {
  if( FD_UNLIKELY( !quic->join.cb.stream_notify ) ) return;
  quic->join.cb.stream_notify( stream, stream_ctx, event );
}

FD_PROTOTYPES_END

#endif /* HEADER_fd_src_tango_quic_fd_quic_private_h */

