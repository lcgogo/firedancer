#include "../fd_tests.h"
int test_919(fd_executor_test_suite_t *suite) {
  fd_executor_test_t test;
  fd_memset( &test, 0, FD_EXECUTOR_TEST_FOOTPRINT );
  test.disable_cnt = 47;
  test.bt = "   2: solana_config_program::config_processor::tests::test_process_store_fail_account0_not_signer             at ./src/config_processor.rs:295:9   3: solana_config_program::config_processor::tests::test_process_store_fail_account0_not_signer::{{closure}}             at ./src/config_processor.rs:286:5   4: core::ops::function::FnOnce::call_once             at /rustc/0677edc86e342f333d4828b0ee1ef395a4e70fe5/library/core/src/ops/function.rs:227:5   5: core::ops::function::FnOnce::call_once             at /rustc/0677edc86e342f333d4828b0ee1ef395a4e70fe5/library/core/src/ops/function.rs:227:5";
  test.test_name = "config_processor::tests::test_process_store_fail_account0_not_signer";
  test.test_number = 919;
  if (fd_executor_test_suite_check_filter(suite, &test)) return -9999;
  ulong test_accs_len = 1;
  fd_executor_test_acc_t* test_accs = fd_alloca( 1UL, FD_EXECUTOR_TEST_ACC_FOOTPRINT * test_accs_len );
  fd_memset( test_accs, 0, FD_EXECUTOR_TEST_ACC_FOOTPRINT * test_accs_len );


  uchar disabled_features[] = { 10,13,130,131,133,135,136,137,14,146,148,15,154,155,159,19,2,22,23,24,25,26,34,36,39,40,41,43,44,48,51,54,55,57,62,64,69,7,70,72,76,77,80,83,89,99 };
  test.disable_feature = disabled_features;
            
  fd_executor_test_acc_t* test_acc = test_accs;
  fd_base58_decode_32( "B2yGapHoRns2a7mjWgkefLptBsb3AcGq4vVace3rXuLk",  (uchar *) &test_acc->pubkey);
  fd_base58_decode_32( "Config1111111111111111111111111111111111111",  (uchar *) &test_acc->owner);
  fd_base58_decode_32( "Config1111111111111111111111111111111111111",  (uchar *) &test_acc->result_owner);
  test_acc->lamports        = 0UL;
  test_acc->result_lamports = 0UL;
  test_acc->executable      = 0;
  test_acc->result_executable= 0;
  test_acc->rent_epoch      = 0;
  test_acc->result_rent_epoch      = 0;
  static uchar const fd_flamenco_native_prog_test_919_acc_0_data[] = { 0x00,0x15,0xcd,0x5b,0x07,0x00,0x00,0x00,0x00 };
  test_acc->data            = fd_flamenco_native_prog_test_919_acc_0_data;
  test_acc->data_len        = 9UL;
  static uchar const fd_flamenco_native_prog_test_919_acc_0_post_data[] = { 0x00,0x15,0xcd,0x5b,0x07,0x00,0x00,0x00,0x00 };
  test_acc->result_data     = fd_flamenco_native_prog_test_919_acc_0_post_data;
  test_acc->result_data_len = 9UL;
  test_acc++;
  fd_base58_decode_32( "Config1111111111111111111111111111111111111",  (unsigned char *) &test.program_id);
  static uchar const fd_flamenco_native_prog_test_919_raw[] = { 0x00,0x00,0x00,0x01,0x02,0x95,0x16,0x12,0xc1,0x6a,0x48,0x3a,0x90,0x56,0x65,0xf8,0xea,0x13,0xc9,0xd3,0xcb,0x1e,0x10,0x7e,0xb1,0x0e,0x1e,0x99,0x7a,0x23,0xd7,0x3c,0xb4,0x29,0x07,0x87,0x89,0x03,0x06,0x4a,0xa3,0x00,0x2f,0x74,0xdc,0xc8,0x6e,0x43,0x31,0x0f,0x0c,0x05,0x2a,0xf8,0xc5,0xda,0x27,0xf6,0x10,0x40,0x19,0xa3,0x23,0xef,0xa0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x00,0x09,0x00,0x2a,0x00,0x00,0x00,0x00,0x00,0x00,0x00 };
  test.raw_tx = fd_flamenco_native_prog_test_919_raw;
  test.raw_tx_len = 115UL;
  test.expected_result = -8;
  test.custom_err = 0;

  test.accs_len = test_accs_len;
  test.accs = test_accs;

  return fd_executor_run_test( &test, suite );
}
