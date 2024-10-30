/* Automatically generated nanopb constant definitions */
/* Generated by nanopb-0.4.8-dev */

#include "context.pb.h"
#if PB_PROTO_HEADER_VERSION != 40
#error Regenerate this file with the current version of nanopb generator.
#endif

PB_BIND(FD_EXEC_TEST_FEATURE_SET, fd_exec_test_feature_set_t, AUTO)


PB_BIND(FD_EXEC_TEST_SEED_ADDRESS, fd_exec_test_seed_address_t, AUTO)


PB_BIND(FD_EXEC_TEST_ACCT_STATE, fd_exec_test_acct_state_t, AUTO)


PB_BIND(FD_EXEC_TEST_EPOCH_CONTEXT, fd_exec_test_epoch_context_t, AUTO)


PB_BIND(FD_EXEC_TEST_SLOT_CONTEXT, fd_exec_test_slot_context_t, AUTO)



#ifndef PB_CONVERT_DOUBLE_FLOAT
/* On some platforms (such as AVR), double is really float.
 * To be able to encode/decode double on these platforms, you need.
 * to define PB_CONVERT_DOUBLE_FLOAT in pb.h or compiler command line.
 */
PB_STATIC_ASSERT(sizeof(double) == 8, DOUBLE_MUST_BE_8_BYTES)
#endif

