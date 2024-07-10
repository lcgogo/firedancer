/* test_quic_tls_hs performs a handshake using the fd_quic_tls API.
   (fd_quic_tls is a light wrapper over fd_tls) */

#include <signal.h>

#include "../../tls/test_tls_helper.h"
#include "../tls/fd_quic_tls.h"
#include "../templ/fd_quic_transport_params.h"
#include "../../../util/net/fd_ip4.h"
#include "../../../ballet/x509/fd_x509_mock.h"

// test transport parameters
static uchar const test_tp[] =
  "\x01\x04\x80\x00\xea\x60\x04\x04\x80\x10\x00\x00"
  "\x05\x04\x80\x10\x00\x00\x06\x04\x80\x10\x00\x00\x07\x04\x80\x10"
  "\x00\x00\x08\x02\x40\x80\x09\x02\x40\x80\x0a\x01\x03\x0b\x01\x19"
  "\x0e\x01\x08\x0f\x08\xec\x73\x1b\x41\xa0\xd5\xc6\xfe";


typedef struct my_quic_tls my_quic_tls_t;
struct my_quic_tls {
  int is_server;
  int is_flush;
  int is_hs_complete;

  int state;
  int sec_level;
};

static void
my_hs_complete( fd_quic_tls_hs_t *           hs,
                void *                       context ) {
  (void)hs;
  (void)context;

  FD_LOG_DEBUG(( "callback handshake complete" ));

  my_quic_tls_t * ctx = (my_quic_tls_t*)context;
  ctx->is_hs_complete = 1;
}

static void
my_secrets( fd_quic_tls_hs_t *           hs,
            void *                       context,
            fd_quic_tls_secret_t const * secret ) {
  (void)context;
  FD_TEST( secret );
  FD_LOG_INFO(( "callback secrets (%s, level=%u)",
                hs->is_server ? "server" : "client", secret->enc_level ));
  FD_LOG_HEXDUMP_INFO(( "read secret",  secret->read_secret,  secret->secret_len ));
  FD_LOG_HEXDUMP_INFO(( "write secret", secret->write_secret, secret->secret_len ));
}

void
my_alert( fd_quic_tls_hs_t * hs,
          void *             context,
          int                alert ) {
  (void)hs;
  (void)context;

  FD_LOG_INFO(( "Alert: %d-%s",  alert, fd_tls_alert_cstr( (uint)alert ) ));
  FD_LOG_INFO(( "Reason: %d-%s", hs->hs.base.reason, fd_tls_reason_cstr( hs->hs.base.reason ) ));
}

void
my_transport_params( void *        context,
                     uchar const * quic_tp,
                     ulong         quic_tp_sz ) {
  (void)context;
  FD_TEST( quic_tp_sz == sizeof(test_tp)-1 );
  FD_TEST( 0==memcmp( quic_tp, test_tp, quic_tp_sz ) );
}

static uchar test_quic_tls_mem[ 288144UL ] __attribute__((aligned(128)));

int
main( int     argc,
      char ** argv ) {
  fd_boot( &argc, &argv );

  fd_rng_t  _rng[1];   fd_rng_t * rng    = fd_rng_join   ( fd_rng_new   ( _rng, 0U, 0UL ) );

  /* Generate certificate key */
  fd_tls_test_sign_ctx_t sign_ctx = fd_tls_test_sign_ctx( rng );

  // config parameters
  fd_quic_tls_cfg_t cfg = {
    .alert_cb              = my_alert,
    .secret_cb             = my_secrets,
    .handshake_complete_cb = my_hs_complete,
    .peer_params_cb        = my_transport_params,

    .max_concur_handshakes = 16,
    .cert_public_key       = sign_ctx.public_key,
    .signer                = fd_tls_test_sign( &sign_ctx ),
  };

  /* dump transport params */
  fd_quic_transport_params_t tmp_tp[1] = {0};
  FD_TEST( fd_quic_decode_transport_params( tmp_tp, test_tp, sizeof(test_tp)-1 )>=0 );

  fd_quic_dump_transport_params( tmp_tp, stdout );
  fflush( stdout );

  ulong tls_align     = fd_quic_tls_align();
  ulong tls_footprint = fd_quic_tls_footprint( cfg.max_concur_handshakes );

  FD_LOG_INFO(( "fd_quic_tls_t align:     %lu bytes", tls_align     ));
  FD_LOG_INFO(( "fd_quic_tls_t footprint: %lu bytes", tls_footprint ));
  FD_TEST( tls_footprint<=sizeof(test_quic_tls_mem) );

  fd_quic_tls_t * quic_tls = fd_quic_tls_new( test_quic_tls_mem, &cfg );
  FD_TEST( quic_tls );

  my_quic_tls_t tls_client[1] = {0};
  my_quic_tls_t tls_server[1] = {0};

  //uchar   cli_dst_conn_id[1] = {0};
  //ulong  cli_dst_conn_id_sz = 0;
  fd_quic_tls_hs_t * hs_client = fd_quic_tls_hs_new( quic_tls,
                                                     tls_client,
                                                     0 /* is_server */,
                                                     "localhost",
                                                     tmp_tp );
  FD_TEST( hs_client );

  //uchar   svr_dst_conn_id[1] = {0};
  //ulong  svr_dst_conn_id_sz = 0;
  fd_quic_tls_hs_t * hs_server = fd_quic_tls_hs_new( quic_tls,
                                                     tls_server,
                                                     1 /* is_server */,
                                                     "localhost",
                                                     tmp_tp );
  FD_TEST( hs_server );

  // generate initial secrets for client

  // server needs first packet with dst conn id in order to generate keys
  //   What happens when dst conn id changes?

  /* Ignore broken pipe signals */
  signal( SIGPIPE, SIG_IGN );

  // start client handshake
  // client fd_quic_tls_hs_t is primed upon creation

  FD_LOG_INFO(( "entering main handshake loop" ));

  for( int l=0; l<16; ++l ) {
    FD_LOG_INFO(( "handshake loop step" ));

    int have_client_data = 0;
    int have_server_data = 0;

    // do we have data to transfer from client to server
    //while( tls_client->hs_data ) {
    while( 1 ) {
      //fd_hs_data_t * hs_data = tls_client->hs_data;
      fd_quic_tls_hs_data_t * hs_data   = NULL;
      for( uint j = 0; j < 4; ++j ) {
        hs_data = fd_quic_tls_get_hs_data( hs_client, j );
        if( hs_data ) break;
      }
      if( !hs_data ) break;

      FD_LOG_DEBUG(( "client hs_data: %p", (void*)hs_data ));
      FD_LOG_DEBUG(( "provide quic data client->server" ));

      // here we need encrypt/decrypt
      // collect fragments at the same enc/sec level, then encrypt
      // ... then decrypt and forward

      int provide_rc = fd_quic_tls_provide_data( hs_server, hs_data->enc_level, hs_data->data, hs_data->data_sz );
      FD_TEST( provide_rc!=FD_QUIC_TLS_FAILED );

      // remove hs_data from head of list
      //tls_client->hs_data = hs_data->next;
      fd_quic_tls_pop_hs_data( hs_client, hs_data->enc_level );

      // delete it
      //fd_hs_data_delete( hs_data );
    }

    // do we have data to transfer from server to client
    while( 1 ) {
      fd_quic_tls_hs_data_t * hs_data = NULL;
      for( uint j=0; j<4; ++j ) {
        hs_data = fd_quic_tls_get_hs_data( hs_server, j );
        if( hs_data ) break;
      }
      if( !hs_data ) break;

      FD_LOG_DEBUG(( "server hs_data: %p", (void *)hs_data ));
      FD_LOG_DEBUG(( "provide quic data server->client" ));

      // here we need encrypt/decrypt
      FD_TEST( fd_quic_tls_provide_data( hs_client, hs_data->enc_level, hs_data->data, hs_data->data_sz )!=FD_QUIC_TLS_FAILED );

      // remove hs_data from head of list
      fd_quic_tls_pop_hs_data( hs_server, hs_data->enc_level );
    }

    int process_rc = fd_quic_tls_process( hs_client );
    FD_LOG_DEBUG(( "fd_quic_tls_process(client) returned %d", process_rc ));
    if( process_rc != FD_QUIC_TLS_SUCCESS ) {
      FD_LOG_ERR(( "process failed. alert: %u-%s reason: %d-%s",
                   hs_client->alert, fd_tls_alert_cstr( hs_client->alert ),
                   hs_client->hs.base.reason, fd_tls_reason_cstr( hs_client->hs.base.reason ) ));
    }

    process_rc = fd_quic_tls_process( hs_server );
    FD_LOG_DEBUG(( "fd_quic_tls_process(server) returned %d", (int)process_rc ));
    if( process_rc != FD_QUIC_TLS_SUCCESS ) {
      FD_LOG_ERR(( "process failed. alert: %u", hs_client->alert ));
    }

    /* check for hs data here */
    have_client_data = 1;
    for( uint i=0; i<4; ++i ) {
      have_client_data &= (_Bool)fd_quic_tls_get_hs_data( hs_client, i );
    }

    have_server_data = 1;
    for( uint i=0; i<4; ++i ) {
      have_server_data &= (_Bool)fd_quic_tls_get_hs_data( hs_server, i );
    }

    if( tls_server->is_hs_complete && tls_client->is_hs_complete ) {
      FD_LOG_INFO(( "both handshakes complete" ));
      if( have_server_data ) {
        FD_LOG_INFO(( "tls_server still has data" ));
      }

      if( have_client_data ) {
        FD_LOG_INFO(( "tls_client still has data" ));
      }
      if( !( have_server_data || have_client_data ) ) break;
    }

    if( !( have_server_data || have_client_data ) ) {
      FD_LOG_INFO(( "incomplete, but no more data to exchange" ));
    }

  }

  FD_TEST( hs_server->hs.base.state == FD_TLS_HS_CONNECTED );
  FD_TEST( hs_client->hs.base.state == FD_TLS_HS_CONNECTED );
  FD_LOG_INFO(( "connected" ));

           fd_quic_tls_hs_delete( hs_client );
           fd_quic_tls_hs_delete( hs_server );
  FD_TEST( fd_quic_tls_delete   ( quic_tls  ) );

  fd_rng_delete( fd_rng_leave( rng ) );

  FD_LOG_NOTICE(( "pass" ));
  fd_halt();
  return 0;
}
