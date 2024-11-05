#include "fd_vm_base.h"
#include "fd_vm_private.h"
#include "../runtime/context/fd_exec_epoch_ctx.h"
#include "../runtime/context/fd_exec_slot_ctx.h"
#include "../features/fd_features.h"

/* fd_vm_syscall_strerror() returns the error message corresponding to err,
   intended to be logged by log_collector, or an empty string if the error code
   should be omitted in logs for whatever reason.  Omitted examples are success,
   panic (logged in place)...
   See also fd_log_collector_program_failure(). */
char const *
fd_vm_syscall_strerror( int err ) {

  switch( err ) {

  case FD_VM_ERR_SYSCALL_INVALID_STRING:                         return "invalid utf-8 sequence"; // truncated
  case FD_VM_ERR_SYSCALL_ABORT:                                  return "SBF program panicked";
  case FD_VM_ERR_SYSCALL_PANIC:                                  return "SBF program Panicked in..."; // truncated
  case FD_VM_ERR_SYSCALL_INVOKE_CONTEXT_BORROW_FAILED:           return "Cannot borrow invoke context";
  case FD_VM_ERR_SYSCALL_MALFORMED_SIGNER_SEED:                  return "Malformed signer seed"; // truncated
  case FD_VM_ERR_SYSCALL_BAD_SEEDS:                              return "Could not create program address with signer seeds"; // truncated
  case FD_VM_ERR_SYSCALL_PROGRAM_NOT_SUPPORTED:                  return "Program not supported by inner instructions"; // truncated
  case FD_VM_ERR_SYSCALL_UNALIGNED_POINTER:                      return "Unaligned pointer";
  case FD_VM_ERR_SYSCALL_TOO_MANY_SIGNERS:                       return "Too many signers";
  case FD_VM_ERR_SYSCALL_INSTRUCTION_TOO_LARGE:                  return "Instruction passed to inner instruction is too large"; // truncated
  case FD_VM_ERR_SYSCALL_TOO_MANY_ACCOUNTS:                      return "Too many accounts passed to inner instruction";
  case FD_VM_ERR_SYSCALL_COPY_OVERLAPPING:                       return "Overlapping copy";
  case FD_VM_ERR_SYSCALL_RETURN_DATA_TOO_LARGE:                  return "Return data too large"; // truncated
  case FD_VM_ERR_SYSCALL_TOO_MANY_SLICES:                        return "Hashing too many sequences";
  case FD_VM_ERR_SYSCALL_INVALID_LENGTH:                         return "InvalidLength";
  case FD_VM_ERR_SYSCALL_MAX_INSTRUCTION_DATA_LEN_EXCEEDED:      return "Invoked an instruction with data that is too large"; // truncated
  case FD_VM_ERR_SYSCALL_MAX_INSTRUCTION_ACCOUNTS_EXCEEDED:      return "Invoked an instruction with too many accounts"; // truncated
  case FD_VM_ERR_SYSCALL_MAX_INSTRUCTION_ACCOUNT_INFOS_EXCEEDED: return "Invoked an instruction with too many account info's"; // truncated
  case FD_VM_ERR_SYSCALL_INVALID_ATTRIBUTE:                      return "InvalidAttribute";
  case FD_VM_ERR_SYSCALL_INVALID_POINTER:                        return "Invalid pointer";
  case FD_VM_ERR_SYSCALL_ARITHMETIC_OVERFLOW:                    return "Arithmetic overflow";

  case FD_VM_ERR_SYSCALL_POSEIDON_INVALID_PARAMS:                return "Syscall error: Invalid parameters.";
  case FD_VM_ERR_SYSCALL_POSEIDON_INVALID_ENDIANNESS:            return "Syscall error: Invalid endianness.";

  default: break;
  }

  return "";
}

/* fd_vm_ebpf_strerror() returns the error message corresponding to err,
   intended to be logged by log_collector, or an empty string if the error code
   should be omitted in logs for whatever reason.
   See also fd_log_collector_program_failure(). */
char const *
fd_vm_ebpf_strerror( int err ) {

  switch( err ) {

  case FD_VM_ERR_EBPF_ELF_ERROR:                   return "ELF error"; // truncated
  case FD_VM_ERR_EBPF_FUNCTION_ALREADY_REGISTERED: return "function was already registered"; // truncated
  case FD_VM_ERR_EBPF_CALL_DEPTH_EXCEEDED:         return "exceeded max BPF to BPF call depth";
  case FD_VM_ERR_EBPF_EXIT_ROOT_CALL_FRAME:        return "attempted to exit root call frame";
  case FD_VM_ERR_EBPF_DIVIDE_BY_ZERO:              return "divide by zero at BPF instruction";
  case FD_VM_ERR_EBPF_DIVIDE_OVERFLOW:             return "division overflow at BPF instruction";
  case FD_VM_ERR_EBPF_EXECUTION_OVERRUN:           return "attempted to execute past the end of the text segment at BPF instruction";
  case FD_VM_ERR_EBPF_CALL_OUTSIDE_TEXT_SEGMENT:   return "callx attempted to call outside of the text segment";
  case FD_VM_ERR_EBPF_EXCEEDED_MAX_INSTRUCTIONS:   return "exceeded CUs meter at BPF instruction";
  case FD_VM_ERR_EBPF_JIT_NOT_COMPILED:            return "program has not been JIT-compiled";
  case FD_VM_ERR_EBPF_INVALID_VIRTUAL_ADDRESS:     return "invalid virtual address"; // truncated
  case FD_VM_ERR_EBPF_INVALID_MEMORY_REGION:       return "Invalid memory region at index"; // truncated
  case FD_VM_ERR_EBPF_ACCESS_VIOLATION:            return "Access violation"; // truncated
  case FD_VM_ERR_EBPF_STACK_ACCESS_VIOLATION:      return "Access violation in stack frame"; // truncated
  case FD_VM_ERR_EBPF_INVALID_INSTRUCTION:         return "invalid BPF instruction";
  case FD_VM_ERR_EBPF_UNSUPPORTED_INSTRUCTION:     return "unsupported BPF instruction";
  case FD_VM_ERR_EBPF_EXHAUSTED_TEXT_SEGMENT:      return "Compilation exhausted text segment at BPF instruction"; // truncated
  case FD_VM_ERR_EBPF_LIBC_INVOCATION_FAILED:      return "Libc calling returned error code"; // truncated
  case FD_VM_ERR_EBPF_VERIFIER_ERROR:              return "Verifier error"; // truncated
  case FD_VM_ERR_EBPF_SYSCALL_ERROR:               return ""; // handled explicitly via fd_vm_syscall_strerror()

  default: break;
  }

  return "";
}

/* fd_vm_strerror() returns the error message corresponding to err, used internally
   for system logs, NOT for log_collector / transaction logs. */
char const *
fd_vm_strerror( int err ) {

  switch( err ) {

  /* "Standard" Firedancer error codes */

  case FD_VM_SUCCESS:   return "SUCCESS success";
  case FD_VM_ERR_INVAL: return "INVAL invalid request";
  case FD_VM_ERR_UNSUP: return "UNSUP unsupported request";
  case FD_VM_ERR_PERM:  return "PERM unauthorized request";
  case FD_VM_ERR_FULL:  return "FULL storage full";
  case FD_VM_ERR_EMPTY: return "EMPTY nothing to do";
  case FD_VM_ERR_IO:    return "IO input-output error";
  case FD_VM_ERR_AGAIN: return "AGAIN try again later";

  /* VM exec error codes */

  case FD_VM_ERR_SIGTEXT:   return "SIGTEXT illegal program counter";
  case FD_VM_ERR_SIGSPLIT:  return "SIGSPLIT split multiword instruction";
  case FD_VM_ERR_SIGCALL:   return "unsupported BPF instruction";
  case FD_VM_ERR_SIGSTACK:  return "SIGSTACK call depth limit exceeded";
  case FD_VM_ERR_SIGILL:    return "SIGILL illegal instruction";
  case FD_VM_ERR_SIGSEGV:   return "SIGSEGV illegal memory address";
  case FD_VM_ERR_SIGBUS:    return "SIGBUS misaligned memory address";
  case FD_VM_ERR_SIGRDONLY: return "SIGRDONLY illegal write";
  case FD_VM_ERR_SIGCOST:   return "SIGCOST compute unit limit exceeded";
  case FD_VM_ERR_SIGFPE:    return "SIGFPE division by zero";

  /* VM syscall error codes */
  /* https://github.com/anza-xyz/agave/blob/v2.0.6/programs/bpf_loader/src/syscalls/mod.rs#L81 */

  case FD_VM_ERR_ABORT:                        return "SBF program panicked";
  case FD_VM_ERR_PANIC:                        return "PANIC";                        /* FIXME: description */
  case FD_VM_ERR_MEM_OVERLAP:                  return "MEM_OVERLAP";                  /* FIXME: description */
  case FD_VM_ERR_INSTR_ERR:                    return "INSTR_ERR";                    /* FIXME: description */
  case FD_VM_ERR_RETURN_DATA_TOO_LARGE:        return "RETURN_DATA_TOO_LARGE";        /* FIXME: description */
  case FD_VM_ERR_INVOKE_CONTEXT_BORROW_FAILED: return "INVOKE_CONTEXT_BORROW_FAILED"; /* FIXME: description */

  /* VM validate error codes */

  case FD_VM_ERR_INVALID_OPCODE:    return "INVALID_OPCODE detected an invalid opcode";
  case FD_VM_ERR_INVALID_SRC_REG:   return "INVALID_SRC_REG detected an invalid source register";
  case FD_VM_ERR_INVALID_DST_REG:   return "INVALID_DST_REG detected an invalid destination register";
  case FD_VM_ERR_INF_LOOP:          return "INF_LOOP detected an infinite loop";
  case FD_VM_ERR_JMP_OUT_OF_BOUNDS: return "JMP_OUT_OF_BOUNDS detected an out of bounds jump";
  case FD_VM_ERR_JMP_TO_ADDL_IMM:   return "JMP_TO_ADDL_IMM detected a jump to an addl imm";
  case FD_VM_ERR_INVALID_END_IMM:   return "INVALID_END_IMM detected an invalid immediate for an endianness conversion instruction";
  case FD_VM_ERR_INCOMPLETE_LDQ:    return "INCOMPLETE_LDQ detected an incomplete ldq at program end";
  case FD_VM_ERR_LDQ_NO_ADDL_IMM:   return "LDQ_NO_ADDL_IMM detected a ldq without an addl imm following it";
  case FD_VM_ERR_NO_SUCH_EXT_CALL:  return "NO_SUCH_EXT_CALL detected a call imm with no function was registered for that immediate";
  case FD_VM_ERR_INVALID_REG:       return "INVALID_REG detected an invalid register number in a callx instruction";
  case FD_VM_ERR_BAD_TEXT:          return "BAD_TEXT detected a bad text section";
  case FD_VM_SH_OVERFLOW:           return "SH_OVERFLOW detected a shift overflow in an instruction";
  case FD_VM_TEXT_SZ_UNALIGNED:     return "TEXT_SZ_UNALIGNED detected an unaligned text section size (not a multiple of 8)";

  default: break;
  }

  return "UNKNOWN probably not a FD_VM_ERR code";
}

/* FIXME: add a pedantic version of this validation that does things
   like:
  - only 0 imms when the instruction does not use an imm
  - same as above but for src/dst reg, offset */
/* FIXME: HANDLING OF LDQ ON NEWER SBPF VERSUS OLDER SBPF (AND WITH CALL
   REG) */
/* FIXME: LINK TO SOLANA CODE VALIDATE AND DOUBLE CHECK THIS BEHAVES
   IDENTICALLY! */

int
fd_vm_validate( fd_vm_t const * vm ) {

  /* A mapping of all the possible 1-byte sBPF opcodes to their
     validation criteria. */

# define FD_VALID       ((uchar)0) /* Valid opcode */
# define FD_CHECK_JMP   ((uchar)1) /* Validation should check that the instruction is a valid jump */
# define FD_CHECK_END   ((uchar)2) /* Validation should check that the instruction is a valid endianness conversion */
# define FD_CHECK_ST    ((uchar)3) /* Validation should check that the instruction is a valid store */
# define FD_CHECK_LDQ   ((uchar)4) /* Validation should check that the instruction is a valid load-quad */
# define FD_CHECK_DIV   ((uchar)5) /* Validation should check that the instruction is a valid division by immediate */
# define FD_CHECK_SH32  ((uchar)6) /* Validation should check that the immediate is a valid 32-bit shift exponent */
# define FD_CHECK_SH64  ((uchar)7) /* Validation should check that the immediate is a valid 64-bit shift exponent */
# define FD_INVALID     ((uchar)8) /* The opcode is invalid */
# define FD_CHECK_CALLX ((uchar)9) /* Validation should check that callx has valid register number */
# define FD_CHECK_CALL_IMM ((uchar)10) /* Validation should check that callimm jumps to a valid program counter */

  static uchar const validation_map[ 256 ] = {
    /* 0x00 */ FD_INVALID,    /* 0x01 */ FD_INVALID,    /* 0x02 */ FD_INVALID,    /* 0x03 */ FD_INVALID,
    /* 0x04 */ FD_VALID,      /* 0x05 */ FD_CHECK_JMP,  /* 0x06 */ FD_INVALID,    /* 0x07 */ FD_VALID,
    /* 0x08 */ FD_INVALID,    /* 0x09 */ FD_INVALID,    /* 0x0a */ FD_INVALID,    /* 0x0b */ FD_INVALID,
    /* 0x0c */ FD_VALID,      /* 0x0d */ FD_INVALID,    /* 0x0e */ FD_INVALID,    /* 0x0f */ FD_VALID,
    /* 0x10 */ FD_INVALID,    /* 0x11 */ FD_INVALID,    /* 0x12 */ FD_INVALID,    /* 0x13 */ FD_INVALID,
    /* 0x14 */ FD_VALID,      /* 0x15 */ FD_CHECK_JMP,  /* 0x16 */ FD_INVALID,    /* 0x17 */ FD_VALID,
    /* 0x18 */ FD_CHECK_LDQ,  /* 0x19 */ FD_INVALID,    /* 0x1a */ FD_INVALID,    /* 0x1b */ FD_INVALID,
    /* 0x1c */ FD_VALID,      /* 0x1d */ FD_CHECK_JMP,  /* 0x1e */ FD_INVALID,    /* 0x1f */ FD_VALID,
    /* 0x20 */ FD_INVALID,    /* 0x21 */ FD_INVALID,    /* 0x22 */ FD_INVALID,    /* 0x23 */ FD_INVALID,
    /* 0x24 */ FD_VALID,      /* 0x25 */ FD_CHECK_JMP,  /* 0x26 */ FD_INVALID,    /* 0x27 */ FD_VALID,
    /* 0x28 */ FD_INVALID,    /* 0x29 */ FD_INVALID,    /* 0x2a */ FD_INVALID,    /* 0x2b */ FD_INVALID,
    /* 0x2c */ FD_VALID,      /* 0x2d */ FD_CHECK_JMP,  /* 0x2e */ FD_INVALID,    /* 0x2f */ FD_VALID,
    /* 0x30 */ FD_INVALID,    /* 0x31 */ FD_INVALID,    /* 0x32 */ FD_INVALID,    /* 0x33 */ FD_INVALID,
    /* 0x34 */ FD_CHECK_DIV,  /* 0x35 */ FD_CHECK_JMP,  /* 0x36 */ FD_INVALID,    /* 0x37 */ FD_CHECK_DIV,
    /* 0x38 */ FD_INVALID,    /* 0x39 */ FD_INVALID,    /* 0x3a */ FD_INVALID,    /* 0x3b */ FD_INVALID,
    /* 0x3c */ FD_VALID,      /* 0x3d */ FD_CHECK_JMP,  /* 0x3e */ FD_INVALID,    /* 0x3f */ FD_VALID,
    /* 0x40 */ FD_INVALID,    /* 0x41 */ FD_INVALID,    /* 0x42 */ FD_INVALID,    /* 0x43 */ FD_INVALID,
    /* 0x44 */ FD_VALID,      /* 0x45 */ FD_CHECK_JMP,  /* 0x46 */ FD_INVALID,    /* 0x47 */ FD_VALID,
    /* 0x48 */ FD_INVALID,    /* 0x49 */ FD_INVALID,    /* 0x4a */ FD_INVALID,    /* 0x4b */ FD_INVALID,
    /* 0x4c */ FD_VALID,      /* 0x4d */ FD_CHECK_JMP,  /* 0x4e */ FD_INVALID,    /* 0x4f */ FD_VALID,
    /* 0x50 */ FD_INVALID,    /* 0x51 */ FD_INVALID,    /* 0x52 */ FD_INVALID,    /* 0x53 */ FD_INVALID,
    /* 0x54 */ FD_VALID,      /* 0x55 */ FD_CHECK_JMP,  /* 0x56 */ FD_INVALID,    /* 0x57 */ FD_VALID,
    /* 0x58 */ FD_INVALID,    /* 0x59 */ FD_INVALID,    /* 0x5a */ FD_INVALID,    /* 0x5b */ FD_INVALID,
    /* 0x5c */ FD_VALID,      /* 0x5d */ FD_CHECK_JMP,  /* 0x5e */ FD_INVALID,    /* 0x5f */ FD_VALID,
    /* 0x60 */ FD_INVALID,    /* 0x61 */ FD_VALID,      /* 0x62 */ FD_CHECK_ST,   /* 0x63 */ FD_CHECK_ST,
    /* 0x64 */ FD_CHECK_SH32, /* 0x65 */ FD_CHECK_JMP,  /* 0x66 */ FD_INVALID,    /* 0x67 */ FD_CHECK_SH64,
    /* 0x68 */ FD_INVALID,    /* 0x69 */ FD_VALID,      /* 0x6a */ FD_CHECK_ST,   /* 0x6b */ FD_CHECK_ST,
    /* 0x6c */ FD_VALID,      /* 0x6d */ FD_CHECK_JMP,  /* 0x6e */ FD_INVALID,    /* 0x6f */ FD_VALID,
    /* 0x70 */ FD_INVALID,    /* 0x71 */ FD_VALID,      /* 0x72 */ FD_CHECK_ST,   /* 0x73 */ FD_CHECK_ST,
    /* 0x74 */ FD_CHECK_SH32, /* 0x75 */ FD_CHECK_JMP,  /* 0x76 */ FD_INVALID,    /* 0x77 */ FD_CHECK_SH64,
    /* 0x78 */ FD_INVALID,    /* 0x79 */ FD_VALID,      /* 0x7a */ FD_CHECK_ST,   /* 0x7b */ FD_CHECK_ST,
    /* 0x7c */ FD_VALID,      /* 0x7d */ FD_CHECK_JMP,  /* 0x7e */ FD_INVALID,    /* 0x7f */ FD_VALID,
    /* 0x80 */ FD_INVALID,    /* 0x81 */ FD_INVALID,    /* 0x82 */ FD_INVALID,    /* 0x83 */ FD_INVALID,
    /* 0x84 */ FD_VALID,      /* 0x85 */ FD_CHECK_CALL_IMM, /* 0x86 */ FD_INVALID,    /* 0x87 */ FD_VALID,
    /* 0x88 */ FD_INVALID,    /* 0x89 */ FD_INVALID,    /* 0x8a */ FD_INVALID,    /* 0x8b */ FD_INVALID,
    /* 0x8c */ FD_INVALID,    /* 0x8d */ FD_CHECK_CALLX,/* 0x8e */ FD_INVALID,    /* 0x8f */ FD_INVALID,
    /* 0x90 */ FD_INVALID,    /* 0x91 */ FD_INVALID,    /* 0x92 */ FD_INVALID,    /* 0x93 */ FD_INVALID,
    /* 0x94 */ FD_CHECK_DIV,  /* 0x95 */ FD_VALID,      /* 0x96 */ FD_INVALID,    /* 0x97 */ FD_CHECK_DIV,
    /* 0x98 */ FD_INVALID,    /* 0x99 */ FD_INVALID,    /* 0x9a */ FD_INVALID,    /* 0x9b */ FD_INVALID,
    /* 0x9c */ FD_VALID,      /* 0x9d */ FD_INVALID,    /* 0x9e */ FD_INVALID,    /* 0x9f */ FD_VALID,
    /* 0xa0 */ FD_INVALID,    /* 0xa1 */ FD_INVALID,    /* 0xa2 */ FD_INVALID,    /* 0xa3 */ FD_INVALID,
    /* 0xa4 */ FD_VALID,      /* 0xa5 */ FD_CHECK_JMP,  /* 0xa6 */ FD_INVALID,    /* 0xa7 */ FD_VALID,
    /* 0xa8 */ FD_INVALID,    /* 0xa9 */ FD_INVALID,    /* 0xaa */ FD_INVALID,    /* 0xab */ FD_INVALID,
    /* 0xac */ FD_VALID,      /* 0xad */ FD_CHECK_JMP,  /* 0xae */ FD_INVALID,    /* 0xaf */ FD_VALID,
    /* 0xb0 */ FD_INVALID,    /* 0xb1 */ FD_INVALID,    /* 0xb2 */ FD_INVALID,    /* 0xb3 */ FD_INVALID,
    /* 0xb4 */ FD_VALID,      /* 0xb5 */ FD_CHECK_JMP,  /* 0xb6 */ FD_INVALID,    /* 0xb7 */ FD_VALID,
    /* 0xb8 */ FD_INVALID,    /* 0xb9 */ FD_INVALID,    /* 0xba */ FD_INVALID,    /* 0xbb */ FD_INVALID,
    /* 0xbc */ FD_VALID,      /* 0xbd */ FD_CHECK_JMP,  /* 0xbe */ FD_INVALID,    /* 0xbf */ FD_VALID,
    /* 0xc0 */ FD_INVALID,    /* 0xc1 */ FD_INVALID,    /* 0xc2 */ FD_INVALID,    /* 0xc3 */ FD_INVALID,
    /* 0xc4 */ FD_CHECK_SH32, /* 0xc5 */ FD_CHECK_JMP,  /* 0xc6 */ FD_INVALID,    /* 0xc7 */ FD_CHECK_SH64,
    /* 0xc8 */ FD_INVALID,    /* 0xc9 */ FD_INVALID,    /* 0xca */ FD_INVALID,    /* 0xcb */ FD_INVALID,
    /* 0xcc */ FD_VALID,      /* 0xcd */ FD_CHECK_JMP,  /* 0xce */ FD_INVALID,    /* 0xcf */ FD_VALID,
    /* 0xd0 */ FD_INVALID,    /* 0xd1 */ FD_INVALID,    /* 0xd2 */ FD_INVALID,    /* 0xd3 */ FD_INVALID,
    /* 0xd4 */ FD_CHECK_END,  /* 0xd5 */ FD_CHECK_JMP,  /* 0xd6 */ FD_INVALID,    /* 0xd7 */ FD_INVALID,
    /* 0xd8 */ FD_INVALID,    /* 0xd9 */ FD_INVALID,    /* 0xda */ FD_INVALID,    /* 0xdb */ FD_INVALID,
    /* 0xdc */ FD_CHECK_END,  /* 0xdd */ FD_CHECK_JMP,  /* 0xde */ FD_INVALID,    /* 0xdf */ FD_INVALID,
    /* 0xe0 */ FD_INVALID,    /* 0xe1 */ FD_INVALID,    /* 0xe2 */ FD_INVALID,    /* 0xe3 */ FD_INVALID,
    /* 0xe4 */ FD_INVALID,    /* 0xe5 */ FD_INVALID,    /* 0xe6 */ FD_INVALID,    /* 0xe7 */ FD_INVALID,
    /* 0xe8 */ FD_INVALID,    /* 0xe9 */ FD_INVALID,    /* 0xea */ FD_INVALID,    /* 0xeb */ FD_INVALID,
    /* 0xec */ FD_INVALID,    /* 0xed */ FD_INVALID,    /* 0xee */ FD_INVALID,    /* 0xef */ FD_INVALID,
    /* 0xf0 */ FD_INVALID,    /* 0xf1 */ FD_INVALID,    /* 0xf2 */ FD_INVALID,    /* 0xf3 */ FD_INVALID,
    /* 0xf4 */ FD_INVALID,    /* 0xf5 */ FD_INVALID,    /* 0xf6 */ FD_INVALID,    /* 0xf7 */ FD_INVALID,
    /* 0xf8 */ FD_INVALID,    /* 0xf9 */ FD_INVALID,    /* 0xfa */ FD_INVALID,    /* 0xfb */ FD_INVALID,
    /* 0xfc */ FD_INVALID,    /* 0xfd */ FD_INVALID,    /* 0xfe */ FD_INVALID,    /* 0xff */ FD_INVALID,
  };

  /* FIXME: These checks are not necessary assuming fd_vm_t is populated by metadata
     generated in fd_sbpf_elf_peek (which performs these checks). But there is no guarantee, and
     this non-guarantee is (rightfully) exploited by the fuzz harnesses.
     Agave doesn't perform these checks explicitly due to Rust's guarantees  */
  if( FD_UNLIKELY( vm->text_sz / 8UL != vm->text_cnt ||
                   (const uchar *)vm->text < vm->rodata ||
                   (ulong)vm->text > (ulong)vm->text + vm->text_sz || /* Overflow chk */
                   (const uchar *)vm->text + vm->text_sz > vm->rodata + vm->rodata_sz ) )
    return FD_VM_ERR_BAD_TEXT;

  if( FD_UNLIKELY( !fd_ulong_is_aligned( vm->text_sz, 8UL ) ) ) /* https://github.com/solana-labs/rbpf/blob/v0.8.0/src/verifier.rs#L109 */
    return FD_VM_TEXT_SZ_UNALIGNED;

  if ( FD_UNLIKELY( vm->text_cnt == 0UL ) ) /* https://github.com/solana-labs/rbpf/blob/v0.8.0/src/verifier.rs#L112 */
    return FD_VM_ERR_EMPTY;

  /* Keep track of the first and last instruction of the current function */
  ulong calldests_iter = fd_sbpf_calldests_const_iter_init( vm->calldests );
  ulong function_start_instruction = calldests_iter;
  ulong function_end_instruction = fd_ulong_sat_sub( vm->text_cnt, 1UL );

  /* FIXME: CLEAN UP LONG / ULONG TYPE CONVERSION */
  ulong const * text     = vm->text;
  ulong         text_cnt = vm->text_cnt;
  for( ulong i=0UL; i<text_cnt; i++ ) {

    /* If we are at the beginning of a function, update the last instruction tracker */
    if ( FD_UNLIKELY( function_start_instruction == i ) ) {
      calldests_iter = fd_sbpf_calldests_const_iter_next( vm->calldests, calldests_iter );
      ulong next_function_start = text_cnt;
      if ( !fd_sbpf_calldests_const_iter_done( calldests_iter ) ) {
        next_function_start = calldests_iter;
      }
      function_end_instruction = fd_ulong_sat_sub( next_function_start, 1UL );
    }

    /* https://github.com/solana-labs/rbpf/blob/69a52ec6a341bb7374d387173b5e6dc56218fe0c/src/verifier.rs#L238-L248 */
    if ( ( vm->sbpf_version == SBPF_VERSION_STATIC_SYSCALLS ) ) {
      /* If we are at the start of a function, check that the function ends in a JA or RETURN */
      if ( function_start_instruction == i ) {
        /* Ensure that the last instruction is either a JA or RETURN */
        uchar last_instr_opcode = fd_sbpf_instr( text[ function_end_instruction ] ).opcode.raw;
        if ( !( last_instr_opcode == 0x05 || last_instr_opcode == 0x9D ) ) {
          return FD_VM_ERR_INVALID_FUNCTION;
        }
      }
    }

    fd_sbpf_instr_t instr = fd_sbpf_instr( text[i] );

    uchar validation_code = validation_map[ instr.opcode.raw ];
    switch( validation_code ) {

    case FD_VALID: break;

    case FD_CHECK_JMP: {
      long jmp_dst = (long)i + (long)instr.offset + 1L;
      if( FD_UNLIKELY( (jmp_dst<0) | (jmp_dst>=(long)text_cnt)                          ) ) return FD_VM_ERR_JMP_OUT_OF_BOUNDS;
      if( FD_UNLIKELY( fd_sbpf_instr( text[ jmp_dst ] ).opcode.raw==FD_SBPF_OP_ADDL_IMM ) ) return FD_VM_ERR_JMP_TO_ADDL_IMM;

      if ( vm->sbpf_version == SBPF_VERSION_STRICT_VERIFICATION ) {
        /* Check the jump offset is to a code location inside the same function */
        if ( i < function_start_instruction || i > function_end_instruction ) {
          return FD_VM_ERR_JUMP_OUT_OF_CODE;
        }
      }

      break;
    }

    case FD_CHECK_END: {
      if( FD_UNLIKELY( !((instr.imm==16) | (instr.imm==32) | (instr.imm==64)) ) ) return FD_VM_ERR_INVALID_END_IMM;
      break;
    }

    case FD_CHECK_ST: break; /* FIXME: HMMM ... */

    /* https://github.com/solana-labs/rbpf/blob/b503a1867a9cfa13f93b4d99679a17fe219831de/src/verifier.rs#L244 */
    case FD_CHECK_LDQ: {
      /* https://github.com/solana-labs/rbpf/blob/b503a1867a9cfa13f93b4d99679a17fe219831de/src/verifier.rs#L131 */
      if( FD_UNLIKELY( (i+1UL)>=text_cnt ) ) return FD_VM_ERR_INCOMPLETE_LDQ;

      /* https://github.com/solana-labs/rbpf/blob/b503a1867a9cfa13f93b4d99679a17fe219831de/src/verifier.rs#L137-L139 */
      fd_sbpf_instr_t addl_imm = fd_sbpf_instr( text[ i+1UL ] );
      if( FD_UNLIKELY( addl_imm.opcode.raw!=FD_SBPF_OP_ADDL_IMM ) ) return FD_VM_ERR_LDQ_NO_ADDL_IMM;

      /* FIXME: SET A BIT MAP HERE OF ADDL_IMM TO DENOTE * AS FORBIDDEN
         BRANCH TARGETS OF CALL_REG?? */

      i++; /* Skip the addl imm */
      break;
    }

    case FD_CHECK_DIV: {
      if( FD_UNLIKELY( instr.imm==0 ) ) return FD_VM_ERR_SIGFPE;  /* FIXME: SIGILL? */
      break;
    }

    case FD_CHECK_SH32: {
      if( FD_UNLIKELY( instr.imm>=32 ) ) return FD_VM_SH_OVERFLOW;
      break;
    }

    case FD_CHECK_SH64: {
      if( FD_UNLIKELY( instr.imm>=64 ) ) return FD_VM_SH_OVERFLOW;
      break;
    }

    case FD_CHECK_CALLX: {
      /* The register number to read is stored in the immediate.
         https://github.com/solana-labs/rbpf/blob/v0.8.1/src/verifier.rs#L218 */
      if( FD_UNLIKELY( instr.imm > 9 ) ) {
        return FD_VM_ERR_INVALID_REG;
      }
      break;
    }

    case FD_CHECK_CALL_IMM: {
      /* Check that the destination is a valid calldest */
      if ( vm->sbpf_version == SBPF_VERSION_STATIC_SYSCALLS ) {
        if ( FD_UNLIKELY( fd_sbpf_calldests_test( vm->calldests, instr.imm ) ) ) {
          return FD_VM_ERR_INVALID_FUNCTION;
        }
      }
      break;
    }

    case FD_INVALID: default: return FD_VM_ERR_INVALID_OPCODE;
    }

    if( FD_UNLIKELY( instr.src_reg>10 ) ) return FD_VM_ERR_INVALID_SRC_REG; /* FIXME: MAGIC NUMBER */

    int is_invalid_dst_reg = instr.dst_reg > ((validation_code == FD_CHECK_ST) ? 10 : 9); /* FIXME: MAGIC NUMBER */
    if( FD_UNLIKELY( is_invalid_dst_reg ) ) return FD_VM_ERR_INVALID_DST_REG;

    /* If we have just finished an instruction, advance the start instruction tracker */
    if ( FD_UNLIKELY( function_end_instruction == i ) ) {
      function_start_instruction = fd_ulong_sat_add( function_end_instruction, 1UL );
    }
  }

  return FD_VM_SUCCESS;
}

FD_FN_CONST ulong
fd_vm_align( void ) {
  return FD_VM_ALIGN;
}

FD_FN_CONST ulong
fd_vm_footprint( void ) {
  return FD_VM_FOOTPRINT;
}

void *
fd_vm_new( void * shmem ) {

  if( FD_UNLIKELY( !shmem ) ) {
    FD_LOG_WARNING(( "NULL shmem" ));
    return NULL;
  }

  if( FD_UNLIKELY( !fd_ulong_is_aligned( (ulong)shmem, fd_vm_align() ) ) ) {
    FD_LOG_WARNING(( "misaligned shmem" ));
    return NULL;
  }

  fd_vm_t * vm = (fd_vm_t *)shmem;
  fd_memset( vm, 0, fd_vm_footprint() );

  FD_COMPILER_MFENCE();
  FD_VOLATILE( vm->magic ) = FD_VM_MAGIC;
  FD_COMPILER_MFENCE();

  return shmem;
}

fd_vm_t *
fd_vm_join( void * shmem ) {

  if( FD_UNLIKELY( !shmem ) ) {
    FD_LOG_WARNING(( "NULL shmem" ));
    return NULL;
  }

  if( FD_UNLIKELY( !fd_ulong_is_aligned( (ulong)shmem, fd_vm_align() ) ) ) {
    FD_LOG_WARNING(( "misaligned shmem" ));
    return NULL;
  }

  fd_vm_t * vm = (fd_vm_t *)shmem;

  if( FD_UNLIKELY( vm->magic!=FD_VM_MAGIC ) ) {
    FD_LOG_WARNING(( "bad magic" ));
    return NULL;
  }

  return vm;
}

void *
fd_vm_leave( fd_vm_t * vm ) {

  if( FD_UNLIKELY( !vm ) ) {
    FD_LOG_WARNING(( "NULL vm" ));
    return NULL;
  }

  return (void *)vm;
}

void *
fd_vm_delete( void * shmem ) {

  if( FD_UNLIKELY( !shmem ) ) {
    FD_LOG_WARNING(( "NULL shmem" ));
    return NULL;
  }

  if( FD_UNLIKELY( !fd_ulong_is_aligned( (ulong)shmem, fd_vm_align() ) ) ) {
    FD_LOG_WARNING(( "misaligned shmem" ));
    return NULL;
  }

  fd_vm_t * vm = (fd_vm_t *)shmem;

  if( FD_UNLIKELY( vm->magic!=FD_VM_MAGIC ) ) {
    FD_LOG_WARNING(( "bad magic" ));
    return NULL;
  }

  FD_COMPILER_MFENCE();
  FD_VOLATILE( vm->magic ) = 0UL;
  FD_COMPILER_MFENCE();

  return (void *)vm;
}

fd_vm_t *
fd_vm_init(
   fd_vm_t * vm,
   fd_exec_instr_ctx_t *instr_ctx,
   ulong heap_max,
   ulong entry_cu,
   uchar const * rodata,
   ulong rodata_sz,
   ulong const * text,
   ulong text_cnt,
   ulong text_off,
   ulong text_sz,
   ulong entry_pc,
   ulong * calldests,
   fd_sbpf_syscalls_t * syscalls,
   fd_vm_trace_t * trace,
   fd_sha256_t * sha,
   fd_vm_input_region_t * mem_regions,
   uint mem_regions_cnt,
   fd_vm_acc_region_meta_t * acc_region_metas,
   uchar is_deprecated,
   int direct_mapping ) {

  if ( FD_UNLIKELY( vm == NULL ) ) {
    FD_LOG_WARNING(( "NULL vm" ));
    return NULL;
  }

  if ( FD_UNLIKELY( vm->magic != FD_VM_MAGIC ) ) {
    FD_LOG_WARNING(( "bad magic" ));
    return NULL;
  }

  if ( FD_UNLIKELY( instr_ctx == NULL ) ) {
    FD_LOG_WARNING(( "NULL instr_ctx" ));
    return NULL;
  }

  if ( FD_UNLIKELY( heap_max > FD_VM_HEAP_MAX ) ) {
    FD_LOG_WARNING(( "heap_max > FD_VM_HEAP_MAX" ));
    return NULL;
  }

  // Set the vm fields
  vm->instr_ctx = instr_ctx;
  vm->heap_max = heap_max;
  vm->entry_cu = entry_cu;
  vm->rodata = rodata;
  vm->rodata_sz = rodata_sz;
  vm->text = text;
  vm->text_cnt = text_cnt;
  vm->text_off = text_off;
  vm->text_sz = text_sz;
  vm->entry_pc = entry_pc;
  vm->calldests = calldests;
  vm->syscalls = syscalls;
  vm->trace = trace;
  vm->sha = sha;
  vm->input_mem_regions = mem_regions;
  vm->input_mem_regions_cnt = mem_regions_cnt;
  vm->acc_region_metas = acc_region_metas;
  vm->is_deprecated = is_deprecated;
  vm->direct_mapping = direct_mapping;
  vm->stack_frame_size = FD_VM_STACK_FRAME_SZ + ( direct_mapping ? 0UL : FD_VM_STACK_GUARD_SZ );
  vm->segv_store_vaddr = ULONG_MAX;

  /* Unpack the configuration */
  int err = fd_vm_setup_state_for_execution( vm );
  if( FD_UNLIKELY( err != FD_VM_SUCCESS ) ) {
    return NULL;
  }

  return vm;
}

int
fd_vm_setup_state_for_execution( fd_vm_t * vm ) {

  if ( FD_UNLIKELY( !vm ) ) {
    FD_LOG_WARNING(( "NULL vm" ));
    return FD_VM_ERR_INVAL;
  }

  /* Unpack input and rodata */
  fd_vm_mem_cfg( vm );

  /* Initialize registers */
  /* FIXME: Zero out shadow, stack and heap here? */
  fd_memset( vm->reg, 0, FD_VM_REG_MAX * sizeof(ulong) );
  vm->reg[ 1] = FD_VM_MEM_MAP_INPUT_REGION_START;
  vm->reg[10] = FD_VM_MEM_MAP_STACK_REGION_START + 0x1000;

  /* Set execution state */
  vm->pc        = vm->entry_pc;
  vm->ic        = 0UL;
  vm->cu        = vm->entry_cu;
  vm->frame_cnt = 0UL;

  vm->heap_sz = 0UL;

  /* Do NOT reset logs */

  return FD_VM_SUCCESS;
}
