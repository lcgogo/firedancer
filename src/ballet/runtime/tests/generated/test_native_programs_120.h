#include "../fd_tests.h"
int test_120(fd_executor_test_suite_t *suite) {
  fd_executor_test_t test;
  fd_memset( &test, 0, FD_EXECUTOR_TEST_FOOTPRINT );
  test.disable_cnt = 0;
  uchar disabled_features[] = { 55,83,78,29,80,111,76,30,112,27,114,89,2,121,106,77,33,128,117,127,56,108,87,105,126,110,118,98,116,109,75,123,92,120,103,15,62,90,24,125,82,122,79,113,61,26,124 };
  test.disable_feature = disabled_features;
  test.bt = "   2: solana_stake_program::stake_instruction::tests::process_instruction             at ./src/stake_instruction.rs:578:9   3: solana_stake_program::stake_instruction::tests::test_authorize_override             at ./src/stake_instruction.rs:1895:24   4: solana_stake_program::stake_instruction::tests::test_authorize_override::new_behavior             at ./src/stake_instruction.rs:1838:5";
  test.test_name = "stake_instruction::tests::test_authorize_override::new_behavior";
  test.test_nonce  = 31;
  test.test_number = 120;
  if (fd_executor_test_suite_check_filter(suite, &test)) return -9999;
  ulong test_accs_len = 3;
  fd_executor_test_acc_t* test_accs = fd_alloca( 1UL, FD_EXECUTOR_TEST_ACC_FOOTPRINT * test_accs_len );
  fd_memset( test_accs, 0, FD_EXECUTOR_TEST_ACC_FOOTPRINT * test_accs_len );

  fd_executor_test_acc_t* test_acc = test_accs;
  fd_base58_decode_32( "Cb9tBcvdTdABKx9b9wAZJfqF4pvhLQUBha19rb5dT3Tk",  (uchar *) &test_acc->pubkey);
  fd_base58_decode_32( "Stake11111111111111111111111111111111111111",  (uchar *) &test_acc->owner);
  test_acc->lamports        = 42UL;
  test_acc->result_lamports = 42UL;
  test_acc->executable      = 0;
  test_acc->rent_epoch      = 0;
  static uchar const fd_flamenco_native_prog_test_120_acc_0_data[] = { 0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x27,0xda,0x8a,0xc6,0x34,0x3f,0xc9,0x49,0xd7,0xfd,0x25,0x57,0x64,0x1b,0xdb,0x0c,0xf5,0x81,0xf1,0x5f,0xfa,0x65,0xa7,0xb8,0x81,0xc4,0xa9,0x03,0x9d,0xd9,0xfd,0xdc,0xac,0x30,0x58,0xdd,0xe7,0xf7,0x36,0xd5,0x2c,0x03,0x3a,0x8a,0x9c,0xfd,0x56,0x3c,0xee,0xbf,0x0c,0xba,0xdb,0x3e,0xf5,0x27,0x93,0xa7,0x7a,0x63,0x0a,0x93,0x98,0x27,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 };
  test_acc->data            = fd_flamenco_native_prog_test_120_acc_0_data;
  test_acc->data_len        = 200UL;
  static uchar const fd_flamenco_native_prog_test_120_acc_0_post_data[] = { 0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x71,0x3e,0x0b,0x68,0x50,0x26,0x4a,0x13,0x63,0x14,0xf6,0xf1,0x4f,0xad,0x75,0x29,0x1f,0x60,0x01,0x2b,0x87,0xeb,0x02,0x8e,0x78,0x76,0x51,0xbb,0xab,0x08,0x62,0x3e,0xac,0x30,0x58,0xdd,0xe7,0xf7,0x36,0xd5,0x2c,0x03,0x3a,0x8a,0x9c,0xfd,0x56,0x3c,0xee,0xbf,0x0c,0xba,0xdb,0x3e,0xf5,0x27,0x93,0xa7,0x7a,0x63,0x0a,0x93,0x98,0x27,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 };
  test_acc->result_data     = fd_flamenco_native_prog_test_120_acc_0_post_data;
  test_acc->result_data_len = 200UL;
  test_acc++;
  fd_base58_decode_32( "3gaBSKsurk9w4N84eTNu39aNJYNnEcKC7HepbuzATXns",  (uchar *) &test_acc->pubkey);
  fd_base58_decode_32( "11111111111111111111111111111111",  (uchar *) &test_acc->owner);
  test_acc->lamports        = 0UL;
  test_acc->result_lamports = 0UL;
  test_acc->executable      = 0;
  test_acc->rent_epoch      = 0;
  test_acc++;
  fd_base58_decode_32( "SysvarC1ock11111111111111111111111111111111",  (uchar *) &test_acc->pubkey);
  fd_base58_decode_32( "Sysvar1111111111111111111111111111111111111",  (uchar *) &test_acc->owner);
  test_acc->lamports        = 1UL;
  test_acc->result_lamports = 1UL;
  test_acc->executable      = 0;
  test_acc->rent_epoch      = 0;
  static uchar const fd_flamenco_native_prog_test_120_acc_2_data[] = { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 };
  test_acc->data            = fd_flamenco_native_prog_test_120_acc_2_data;
  test_acc->data_len        = 40UL;
  static uchar const fd_flamenco_native_prog_test_120_acc_2_post_data[] = { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 };
  test_acc->result_data     = fd_flamenco_native_prog_test_120_acc_2_post_data;
  test_acc->result_data_len = 40UL;
  test_acc++;
  fd_base58_decode_32( "Stake11111111111111111111111111111111111111",  (unsigned char *) &test.program_id);
  static uchar const fd_flamenco_native_prog_test_120_raw[] = { 0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x02,0x04,0x27,0xda,0x8a,0xc6,0x34,0x3f,0xc9,0x49,0xd7,0xfd,0x25,0x57,0x64,0x1b,0xdb,0x0c,0xf5,0x81,0xf1,0x5f,0xfa,0x65,0xa7,0xb8,0x81,0xc4,0xa9,0x03,0x9d,0xd9,0xfd,0xdc,0xac,0x30,0x58,0xdd,0xe7,0xf7,0x36,0xd5,0x2c,0x03,0x3a,0x8a,0x9c,0xfd,0x56,0x3c,0xee,0xbf,0x0c,0xba,0xdb,0x3e,0xf5,0x27,0x93,0xa7,0x7a,0x63,0x0a,0x93,0x98,0x27,0x06,0xa1,0xd8,0x17,0x91,0x37,0x54,0x2a,0x98,0x34,0x37,0xbd,0xfe,0x2a,0x7a,0xb2,0x55,0x7f,0x53,0x5c,0x8a,0x78,0x72,0x2b,0x68,0xa4,0x9d,0xc0,0x00,0x00,0x00,0x00,0x06,0xa7,0xd5,0x17,0x18,0xc7,0x74,0xc9,0x28,0x56,0x63,0x98,0x69,0x1d,0x5e,0xb6,0x8b,0x5e,0xb8,0xa3,0x9b,0x4b,0x6d,0x5c,0x73,0x55,0x5b,0x21,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x02,0x03,0x01,0x03,0x00,0x28,0x01,0x00,0x00,0x00,0x71,0x3e,0x0b,0x68,0x50,0x26,0x4a,0x13,0x63,0x14,0xf6,0xf1,0x4f,0xad,0x75,0x29,0x1f,0x60,0x01,0x2b,0x87,0xeb,0x02,0x8e,0x78,0x76,0x51,0xbb,0xab,0x08,0x62,0x3e,0x00,0x00,0x00,0x00 };
  test.raw_tx = fd_flamenco_native_prog_test_120_raw;
  test.raw_tx_len = 276UL;
  test.expected_result = 0;
  test.custom_err = 0;

  test.accs_len = test_accs_len;
  test.accs = test_accs;

  return fd_executor_run_test( &test, suite );
}
