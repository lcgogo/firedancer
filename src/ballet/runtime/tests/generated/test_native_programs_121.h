#include "../fd_tests.h"
int test_121(fd_executor_test_suite_t *suite) {
  fd_executor_test_t test;
  fd_memset( &test, 0, FD_EXECUTOR_TEST_FOOTPRINT );
  test.disable_cnt = 47;
  uchar disabled_features[] = { 24,15,90,80,89,82,27,110,33,26,79,56,87,76,61,105,98,75,122,120,117,30,108,116,113,121,103,118,78,62,125,111,55,29,83,106,126,92,123,2,128,114,109,124,127,112,77 };
  test.disable_feature = disabled_features;
  test.bt = "   2: solana_stake_program::stake_instruction::tests::process_instruction             at ./src/stake_instruction.rs:578:9   3: solana_stake_program::stake_instruction::tests::test_authorize_override             at ./src/stake_instruction.rs:1895:24   4: solana_stake_program::stake_instruction::tests::test_authorize_override::new_behavior             at ./src/stake_instruction.rs:1838:5";
  test.test_name = "stake_instruction::tests::test_authorize_override::new_behavior";
  test.test_nonce  = 62;
  test.test_number = 121;
  if (fd_executor_test_suite_check_filter(suite, &test)) return -9999;
  ulong test_accs_len = 3;
  fd_executor_test_acc_t* test_accs = fd_alloca( 1UL, FD_EXECUTOR_TEST_ACC_FOOTPRINT * test_accs_len );
  fd_memset( test_accs, 0, FD_EXECUTOR_TEST_ACC_FOOTPRINT * test_accs_len );

  fd_executor_test_acc_t* test_acc = test_accs;
  fd_base58_decode_32( "gBNKoN5Nn1jMiCFFK1BYpMsVNnY6mbkkJiqJK8hnwZc",  (uchar *) &test_acc->pubkey);
  fd_base58_decode_32( "Stake11111111111111111111111111111111111111",  (uchar *) &test_acc->owner);
  test_acc->lamports        = 42UL;
  test_acc->result_lamports = 42UL;
  test_acc->executable      = 0;
  test_acc->rent_epoch      = 0;
  static uchar const fd_flamenco_native_prog_test_121_acc_0_data[] = { 0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xb7,0xe2,0xb1,0x69,0xcd,0x5b,0xfc,0x95,0xf6,0x03,0x08,0xee,0xc0,0x48,0x1d,0xf9,0x40,0x8c,0xcf,0x6a,0xe0,0x5e,0xc9,0x36,0x5f,0x44,0xc8,0xf8,0x91,0x5f,0xde,0xc9,0x0a,0x09,0x5f,0xd2,0x38,0xc5,0x0a,0xb4,0x93,0x5f,0x80,0xb9,0xed,0x0f,0x01,0x7a,0xd9,0xe8,0x4c,0xc7,0x75,0x64,0x66,0xad,0x7f,0xc6,0xd4,0x6c,0x18,0x0f,0x10,0xe3,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 };
  test_acc->data            = fd_flamenco_native_prog_test_121_acc_0_data;
  test_acc->data_len        = 200UL;
  static uchar const fd_flamenco_native_prog_test_121_acc_0_post_data[] = { 0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xb3,0x6a,0x1b,0x18,0x5b,0x4d,0x50,0x00,0x9e,0x6f,0x23,0xeb,0xe6,0x00,0xaf,0xf3,0x1a,0xc6,0xbe,0xf6,0x94,0xd2,0xbd,0x9d,0x50,0x12,0x1f,0x69,0x45,0x38,0x42,0xb0,0x0a,0x09,0x5f,0xd2,0x38,0xc5,0x0a,0xb4,0x93,0x5f,0x80,0xb9,0xed,0x0f,0x01,0x7a,0xd9,0xe8,0x4c,0xc7,0x75,0x64,0x66,0xad,0x7f,0xc6,0xd4,0x6c,0x18,0x0f,0x10,0xe3,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 };
  test_acc->result_data     = fd_flamenco_native_prog_test_121_acc_0_post_data;
  test_acc->result_data_len = 200UL;
  test_acc++;
  fd_base58_decode_32( "DNp6doYNCYnEZkCPozhmhQpjTudWuiJKLiu44U4dXDmJ",  (uchar *) &test_acc->pubkey);
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
  static uchar const fd_flamenco_native_prog_test_121_acc_2_data[] = { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 };
  test_acc->data            = fd_flamenco_native_prog_test_121_acc_2_data;
  test_acc->data_len        = 40UL;
  static uchar const fd_flamenco_native_prog_test_121_acc_2_post_data[] = { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 };
  test_acc->result_data     = fd_flamenco_native_prog_test_121_acc_2_post_data;
  test_acc->result_data_len = 40UL;
  test_acc++;
  fd_base58_decode_32( "Stake11111111111111111111111111111111111111",  (unsigned char *) &test.program_id);
  static uchar const fd_flamenco_native_prog_test_121_raw[] = { 0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x02,0x04,0xb7,0xe2,0xb1,0x69,0xcd,0x5b,0xfc,0x95,0xf6,0x03,0x08,0xee,0xc0,0x48,0x1d,0xf9,0x40,0x8c,0xcf,0x6a,0xe0,0x5e,0xc9,0x36,0x5f,0x44,0xc8,0xf8,0x91,0x5f,0xde,0xc9,0x0a,0x09,0x5f,0xd2,0x38,0xc5,0x0a,0xb4,0x93,0x5f,0x80,0xb9,0xed,0x0f,0x01,0x7a,0xd9,0xe8,0x4c,0xc7,0x75,0x64,0x66,0xad,0x7f,0xc6,0xd4,0x6c,0x18,0x0f,0x10,0xe3,0x06,0xa1,0xd8,0x17,0x91,0x37,0x54,0x2a,0x98,0x34,0x37,0xbd,0xfe,0x2a,0x7a,0xb2,0x55,0x7f,0x53,0x5c,0x8a,0x78,0x72,0x2b,0x68,0xa4,0x9d,0xc0,0x00,0x00,0x00,0x00,0x06,0xa7,0xd5,0x17,0x18,0xc7,0x74,0xc9,0x28,0x56,0x63,0x98,0x69,0x1d,0x5e,0xb6,0x8b,0x5e,0xb8,0xa3,0x9b,0x4b,0x6d,0x5c,0x73,0x55,0x5b,0x21,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x02,0x03,0x01,0x03,0x00,0x28,0x01,0x00,0x00,0x00,0xb3,0x6a,0x1b,0x18,0x5b,0x4d,0x50,0x00,0x9e,0x6f,0x23,0xeb,0xe6,0x00,0xaf,0xf3,0x1a,0xc6,0xbe,0xf6,0x94,0xd2,0xbd,0x9d,0x50,0x12,0x1f,0x69,0x45,0x38,0x42,0xb0,0x00,0x00,0x00,0x00 };
  test.raw_tx = fd_flamenco_native_prog_test_121_raw;
  test.raw_tx_len = 276UL;
  test.expected_result = 0;
  test.custom_err = 0;

  test.accs_len = test_accs_len;
  test.accs = test_accs;

  return fd_executor_run_test( &test, suite );
}
