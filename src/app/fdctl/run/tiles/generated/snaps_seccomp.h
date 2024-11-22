/* THIS FILE WAS GENERATED BY generate_filters.py. DO NOT EDIT BY HAND! */
#ifndef HEADER_fd_src_app_fdctl_run_tiles_generated_snaps_seccomp_h
#define HEADER_fd_src_app_fdctl_run_tiles_generated_snaps_seccomp_h

#include "../../../../../../src/util/fd_util_base.h"
#include <linux/audit.h>
#include <linux/capability.h>
#include <linux/filter.h>
#include <linux/seccomp.h>
#include <linux/bpf.h>
#include <sys/syscall.h>
#include <signal.h>
#include <stddef.h>

#if defined(__i386__)
# define ARCH_NR  AUDIT_ARCH_I386
#elif defined(__x86_64__)
# define ARCH_NR  AUDIT_ARCH_X86_64
#elif defined(__aarch64__)
# define ARCH_NR AUDIT_ARCH_AARCH64
#else
# error "Target architecture is unsupported by seccomp."
#endif
static const unsigned int sock_filter_policy_snaps_instr_cnt = 42;

static void populate_sock_filter_policy_snaps( ulong out_cnt, struct sock_filter * out, unsigned int logfile_fd, unsigned int tmp_fd, unsigned int full_snapshot_fd, unsigned int incremental_snapshot_fd) {
  FD_TEST( out_cnt >= 42 );
  struct sock_filter filter[42] = {
    /* Check: Jump to RET_KILL_PROCESS if the script's arch != the runtime arch */
    BPF_STMT( BPF_LD | BPF_W | BPF_ABS, ( offsetof( struct seccomp_data, arch ) ) ),
    BPF_JUMP( BPF_JMP | BPF_JEQ | BPF_K, ARCH_NR, 0, /* RET_KILL_PROCESS */ 38 ),
    /* loading syscall number in accumulator */
    BPF_STMT( BPF_LD | BPF_W | BPF_ABS, ( offsetof( struct seccomp_data, nr ) ) ),
    /* allow write based on expression */
    BPF_JUMP( BPF_JMP | BPF_JEQ | BPF_K, SYS_write, /* check_write */ 6, 0 ),
    /* allow fsync based on expression */
    BPF_JUMP( BPF_JMP | BPF_JEQ | BPF_K, SYS_fsync, /* check_fsync */ 15, 0 ),
    /* allow fchmod based on expression */
    BPF_JUMP( BPF_JMP | BPF_JEQ | BPF_K, SYS_fchmod, /* check_fchmod */ 16, 0 ),
    /* allow ftruncate based on expression */
    BPF_JUMP( BPF_JMP | BPF_JEQ | BPF_K, SYS_ftruncate, /* check_ftruncate */ 17, 0 ),
    /* allow lseek based on expression */
    BPF_JUMP( BPF_JMP | BPF_JEQ | BPF_K, SYS_lseek, /* check_lseek */ 24, 0 ),
    /* allow read based on expression */
    BPF_JUMP( BPF_JMP | BPF_JEQ | BPF_K, SYS_read, /* check_read */ 29, 0 ),
    /* none of the syscalls matched */
    { BPF_JMP | BPF_JA, 0, 0, /* RET_KILL_PROCESS */ 30 },
//  check_write:
    /* load syscall argument 0 in accumulator */
    BPF_STMT( BPF_LD | BPF_W | BPF_ABS, offsetof(struct seccomp_data, args[0])),
    BPF_JUMP( BPF_JMP | BPF_JEQ | BPF_K, 2, /* RET_ALLOW */ 29, /* lbl_1 */ 0 ),
//  lbl_1:
    /* load syscall argument 0 in accumulator */
    BPF_STMT( BPF_LD | BPF_W | BPF_ABS, offsetof(struct seccomp_data, args[0])),
    BPF_JUMP( BPF_JMP | BPF_JEQ | BPF_K, logfile_fd, /* RET_ALLOW */ 27, /* lbl_2 */ 0 ),
//  lbl_2:
    /* load syscall argument 0 in accumulator */
    BPF_STMT( BPF_LD | BPF_W | BPF_ABS, offsetof(struct seccomp_data, args[0])),
    BPF_JUMP( BPF_JMP | BPF_JEQ | BPF_K, tmp_fd, /* RET_ALLOW */ 25, /* lbl_3 */ 0 ),
//  lbl_3:
    /* load syscall argument 0 in accumulator */
    BPF_STMT( BPF_LD | BPF_W | BPF_ABS, offsetof(struct seccomp_data, args[0])),
    BPF_JUMP( BPF_JMP | BPF_JEQ | BPF_K, full_snapshot_fd, /* RET_ALLOW */ 23, /* lbl_4 */ 0 ),
//  lbl_4:
    /* load syscall argument 0 in accumulator */
    BPF_STMT( BPF_LD | BPF_W | BPF_ABS, offsetof(struct seccomp_data, args[0])),
    BPF_JUMP( BPF_JMP | BPF_JEQ | BPF_K, incremental_snapshot_fd, /* RET_ALLOW */ 21, /* RET_KILL_PROCESS */ 20 ),
//  check_fsync:
    /* load syscall argument 0 in accumulator */
    BPF_STMT( BPF_LD | BPF_W | BPF_ABS, offsetof(struct seccomp_data, args[0])),
    BPF_JUMP( BPF_JMP | BPF_JEQ | BPF_K, logfile_fd, /* RET_ALLOW */ 19, /* RET_KILL_PROCESS */ 18 ),
//  check_fchmod:
    /* load syscall argument 1 in accumulator */
    BPF_STMT( BPF_LD | BPF_W | BPF_ABS, offsetof(struct seccomp_data, args[1])),
    BPF_JUMP( BPF_JMP | BPF_JEQ | BPF_K, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH, /* RET_ALLOW */ 17, /* RET_KILL_PROCESS */ 16 ),
//  check_ftruncate:
    /* load syscall argument 0 in accumulator */
    BPF_STMT( BPF_LD | BPF_W | BPF_ABS, offsetof(struct seccomp_data, args[0])),
    BPF_JUMP( BPF_JMP | BPF_JEQ | BPF_K, tmp_fd, /* lbl_5 */ 4, /* lbl_6 */ 0 ),
//  lbl_6:
    /* load syscall argument 0 in accumulator */
    BPF_STMT( BPF_LD | BPF_W | BPF_ABS, offsetof(struct seccomp_data, args[0])),
    BPF_JUMP( BPF_JMP | BPF_JEQ | BPF_K, full_snapshot_fd, /* lbl_5 */ 2, /* lbl_7 */ 0 ),
//  lbl_7:
    /* load syscall argument 0 in accumulator */
    BPF_STMT( BPF_LD | BPF_W | BPF_ABS, offsetof(struct seccomp_data, args[0])),
    BPF_JUMP( BPF_JMP | BPF_JEQ | BPF_K, incremental_snapshot_fd, /* lbl_5 */ 0, /* RET_KILL_PROCESS */ 10 ),
//  lbl_5:
    /* load syscall argument 1 in accumulator */
    BPF_STMT( BPF_LD | BPF_W | BPF_ABS, offsetof(struct seccomp_data, args[1])),
    BPF_JUMP( BPF_JMP | BPF_JEQ | BPF_K, 0, /* RET_ALLOW */ 9, /* RET_KILL_PROCESS */ 8 ),
//  check_lseek:
    /* load syscall argument 0 in accumulator */
    BPF_STMT( BPF_LD | BPF_W | BPF_ABS, offsetof(struct seccomp_data, args[0])),
    BPF_JUMP( BPF_JMP | BPF_JEQ | BPF_K, tmp_fd, /* RET_ALLOW */ 7, /* lbl_8 */ 0 ),
//  lbl_8:
    /* load syscall argument 0 in accumulator */
    BPF_STMT( BPF_LD | BPF_W | BPF_ABS, offsetof(struct seccomp_data, args[0])),
    BPF_JUMP( BPF_JMP | BPF_JEQ | BPF_K, full_snapshot_fd, /* RET_ALLOW */ 5, /* lbl_9 */ 0 ),
//  lbl_9:
    /* load syscall argument 0 in accumulator */
    BPF_STMT( BPF_LD | BPF_W | BPF_ABS, offsetof(struct seccomp_data, args[0])),
    BPF_JUMP( BPF_JMP | BPF_JEQ | BPF_K, incremental_snapshot_fd, /* RET_ALLOW */ 3, /* RET_KILL_PROCESS */ 2 ),
//  check_read:
    /* load syscall argument 0 in accumulator */
    BPF_STMT( BPF_LD | BPF_W | BPF_ABS, offsetof(struct seccomp_data, args[0])),
    BPF_JUMP( BPF_JMP | BPF_JEQ | BPF_K, tmp_fd, /* RET_ALLOW */ 1, /* RET_KILL_PROCESS */ 0 ),
//  RET_KILL_PROCESS:
    /* KILL_PROCESS is placed before ALLOW since it's the fallthrough case. */
    BPF_STMT( BPF_RET | BPF_K, SECCOMP_RET_KILL_PROCESS ),
//  RET_ALLOW:
    /* ALLOW has to be reached by jumping */
    BPF_STMT( BPF_RET | BPF_K, SECCOMP_RET_ALLOW ),
  };
  fd_memcpy( out, filter, sizeof( filter ) );
}

#endif
