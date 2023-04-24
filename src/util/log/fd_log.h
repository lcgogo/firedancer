#ifndef HEADER_fd_src_util_log_fd_log_h
#define HEADER_fd_src_util_log_fd_log_h

/* Note: fd must be booted to use the APIs in this module */

/* The fd_log conceptually produces up to two log message streams for
   an application.  One is the ephemeral log message stream (aka
   "stderr") and the other is permanent log message stream ("the log
   file").  Messages to "stderr" are abbreviated as somebody watching
   this stream realtime typically already knows the stream context in
   great detail (the host, the user, the application, etc).  Messages to
   the "log file" are much more detailed and thus suitable long time
   archival purposes.

   In producing these streams, writes to the log file are prioritized
   over writes to stderr.  Further, writes to these streams are done
   quasi-atomically at message granularity to reduce the risk that
   concurrent log messages from different threads will get mixed
   together.

   Default behaviors are:

   - FD_LOG_DEBUG messages are not written to either stream (the
     argument list is still processed though so that any side effects of
     the argument list are not lost).

   - FD_LOG_INFO messages are written in detailed form to the log file
     (if the fd_log log file is setup).

   - FD_LOG_NOTICE is FD_LOG_INFO + messages are written in summary
     form to stderr.

   - FD_LOG_WARNING is FD_LOG_NOTICE + the log file and stderr are
     flushed to minimize the risk of this message and any preceeding not
     making it out before thread resumption.

   - FD_LOG_ERR is FD_LOG_WARNING + the program will be exited with
     an error code of 1.

   - FD_LOG_CRIT and above are FD_LOG_WARNING + the program will
     do a backtrace if possible to the log file and stderr and, after a
     brief delay to let any pending fd_log writes complete, aborts the
     program (which typically also produces a core dump).

   These log level names mirror the Linux syslog levels.

   Useful concepts / terms:

   - An application is a collection of 1 or more thread groups that have
     common log.

   - A thread group is a collection of 1 or more threads.  (It typically
     is a process but there are unhosted situations when a more
     generalized notion of process is required.)

   - The log has a single wall clock for timestamping log messages.

   - Log messages timestamps reflect the time when log message creation
     starts.

   - Back-to-back reads of the wallclock by a thread should be
     monotonically increasing such that the order in which that thread's
     log messages were generated is accurately reflected by the
     timestamps.

   - Concurrent reads of the wallclock by different threads should be
     reasonably well sychronized such that ordering of events between
     communicating threads is accurately reflected by the timestamps.

   - A thread runs on a cpu.

   - A CPU has an architecture (x86 cores, ASIC cores, FPGAs, GPU MPUs,
     etc).

   - Multiple CPU architectures might be used by an application.

   - A host is a collection of cpus for which shared memory style
     communication primitives are reasonably efficient.

   - CPUs in a host need not share a common memory address space.

   - CPUs in a host need not share a common architecture.

   - Threads in a thread group run on the same host.

   - Threads in a thread group run on the same architecture.

   - Threads in a thread group share a common address space.

   - Threads in a thread group share a common group global variables.

   - A thread group will be part of one application for its lifetime.

   - A thread will be part of only one thread group for its lifetime.

   - A thread will run on only one host for its lifetime.

   - A thread will run on only one architecture for its lifetime.

   - An application thread's thread id is unique over all running
     threads in an application.

   - An application thread's thread id reasonably cheaply identifies the
     thread group to which the thread belongs.

   - Typically, the set of threads in a thread group will be constant
     for the lifetime of the thread group (but this is not strictly
     required).

   - Typically, the set of threads groups in an application will be
     constant for the lifetime of the application (but this is not
     strictly required).

   - Typically, a thread will run on only one CPU for its lifetime
     (but this is not strictly required).

   - Typically, a CPU will only be responsible for the execution of at
     most one application thread at any given time (but this is not
     strictly required).

   The above implies:

   * The synchronization of concurrent clock reads between two
     communicating application threads should be tighter than the
     latency for these two threads to communicate (e.g. T_send < T_recv
     is preserved).

   * The range over which the this can be done (i.e. the range of which
     the wallclock can be distributed with good synchronization and
     reasonably cheaply read) is the range over which application
     threads can be distributed.

   * There exist efficient forms of address space translation /
     virtualization to facilitate shared memory style communication
     between application threads on a host.

   * Communications between threads on different hosts is done via
     message passing.

   * Communications between threads on the same host can be done either
     by message passing or via shared memory. */

#include "../env/fd_env.h"

/* FD_LOG_NOTICE(( ... printf style arguments ... )) will send a message
   at the NOTICE level to the logger.  E.g. for a typical fd_log
   configuration:

     FD_LOG_NOTICE(( "%lu is the loneliest number", 1UL ));

   would log something like:

     NOTICE  01-23 04:56:07.890123 45678 f0 0 src/file.c(901): 1 is the loneliest number

   to the ephemeral log (stderr) and log something like:

     NOTICE  2023-01-23 04:56:07.890123456 GMT-06 45678:45678 user:host:f0 app:thread:0 src/file.c(901)[func]: 1 is the loneliest number

   to the permanent log (log file).  Similarly for the other log levels.
   Additional logger details are described at the top of this file.

   FD_LOG_NOTICE has a hexdump counterpart that essentially behaves
   like:

     void
     FD_LOG_HEXDUMP_NOTICE(( char const * tag,
                             void const * mem,
                             ulong        sz ));

   This logs pretty printed details about memory region to the log
   streams at the NOTICE log severity level.

   tag points to a cstr that is intended to be a human-readable /
   greppable tag describing the memory region.  As such, it is strongly
   recommended that tag points to a cstr containing only printable
   characters with no internal double quotes (but this is not enforced
   currently).  There are no length restrictions on the cstr but the
   logger under the hood might detectably truncate excessively long tags
   (e.g. strlen(tag) >> 32) due to internal implementation limitations.
   NULL and/or empty tags ("") are fine and will be detectably logged.

   mem points to the first byte of the memory region to hexdump and sz
   is the number of bytes in the region.  There are no limits on sz but
   the number of bytes logged might be limited due to internal
   implementation details (e.g. sz >> 1500 bytes).  NULL mem and/or 0 sz
   are fine and will be detectably logged.

   The lifetime the cstr and the memory region must be at least from the
   call entry to call return.

   E.g. for a typical fd_log configuration:

     FD_LOG_HEXDUMP_WARNING(( "bad_pkt", pkt, pkt_sz ));

   would log something like:

     WARNING 01-23 04:56:07.890123 75779 f0 0 src/file.c(901): HEXDUMP "bad_pkt" (96 bytes at 0x555555561a4e)
             0000:  30 31 32 33 34 35 36 37 38 39 41 42 43 44 45 46  0123456789ABCDEF
             0010:  47 48 49 4a 4b 4c 4d 4e 4f 50 51 52 53 54 55 56  GHIJKLMNOPQRSTUV
             0020:  57 58 59 5a 61 62 63 64 65 66 67 68 69 6a 6b 6c  WXYZabcdefghijkl
             0030:  6d 6e 6f 70 71 72 73 74 75 76 77 78 79 7a 20 7e  mnopqrstuvwxyz ~
             0040:  21 40 23 24 25 5e 26 2a 28 29 5f 2b 60 2d 3d 5b  !@#$%^&*()_+`-=[
             0050:  5d 5c 3b 27 2c 2e 2f 7b 7d 7c 3a 22 3c 3e 3f 00  ]\;',./{}|:"<>?.

   to the ephemeral log (stderr) and similarly to the permanent log.

   Similarly for hexdumping to other log levels.

   Note: fd_log_wallclock called outside the arg list to give it a
   linguistically strict point when it is called that is before logging
   activities commence. */

#define FD_LOG_DEBUG(a)           do { long _fd_log_msg_now = fd_log_wallclock(); fd_log_private_1( 0, _fd_log_msg_now, __FILE__, __LINE__, __func__, fd_log_private_0           a ); } while(0)
#define FD_LOG_INFO(a)            do { long _fd_log_msg_now = fd_log_wallclock(); fd_log_private_1( 1, _fd_log_msg_now, __FILE__, __LINE__, __func__, fd_log_private_0           a ); } while(0)
#define FD_LOG_NOTICE(a)          do { long _fd_log_msg_now = fd_log_wallclock(); fd_log_private_1( 2, _fd_log_msg_now, __FILE__, __LINE__, __func__, fd_log_private_0           a ); } while(0)
#define FD_LOG_WARNING(a)         do { long _fd_log_msg_now = fd_log_wallclock(); fd_log_private_1( 3, _fd_log_msg_now, __FILE__, __LINE__, __func__, fd_log_private_0           a ); } while(0)
#define FD_LOG_ERR(a)             do { long _fd_log_msg_now = fd_log_wallclock(); fd_log_private_2( 4, _fd_log_msg_now, __FILE__, __LINE__, __func__, fd_log_private_0           a ); } while(0)
#define FD_LOG_CRIT(a)            do { long _fd_log_msg_now = fd_log_wallclock(); fd_log_private_2( 5, _fd_log_msg_now, __FILE__, __LINE__, __func__, fd_log_private_0           a ); } while(0)
#define FD_LOG_ALERT(a)           do { long _fd_log_msg_now = fd_log_wallclock(); fd_log_private_2( 6, _fd_log_msg_now, __FILE__, __LINE__, __func__, fd_log_private_0           a ); } while(0)
#define FD_LOG_EMERG(a)           do { long _fd_log_msg_now = fd_log_wallclock(); fd_log_private_2( 7, _fd_log_msg_now, __FILE__, __LINE__, __func__, fd_log_private_0           a ); } while(0)

#define FD_LOG_HEXDUMP_DEBUG(a)   do { long _fd_log_msg_now = fd_log_wallclock(); fd_log_private_1( 0, _fd_log_msg_now, __FILE__, __LINE__, __func__, fd_log_private_hexdump_msg a ); } while(0)
#define FD_LOG_HEXDUMP_INFO(a)    do { long _fd_log_msg_now = fd_log_wallclock(); fd_log_private_1( 1, _fd_log_msg_now, __FILE__, __LINE__, __func__, fd_log_private_hexdump_msg a ); } while(0)
#define FD_LOG_HEXDUMP_NOTICE(a)  do { long _fd_log_msg_now = fd_log_wallclock(); fd_log_private_1( 2, _fd_log_msg_now, __FILE__, __LINE__, __func__, fd_log_private_hexdump_msg a ); } while(0)
#define FD_LOG_HEXDUMP_WARNING(a) do { long _fd_log_msg_now = fd_log_wallclock(); fd_log_private_1( 3, _fd_log_msg_now, __FILE__, __LINE__, __func__, fd_log_private_hexdump_msg a ); } while(0)
#define FD_LOG_HEXDUMP_ERR(a)     do { long _fd_log_msg_now = fd_log_wallclock(); fd_log_private_2( 4, _fd_log_msg_now, __FILE__, __LINE__, __func__, fd_log_private_hexdump_msg a ); } while(0)
#define FD_LOG_HEXDUMP_CRIT(a)    do { long _fd_log_msg_now = fd_log_wallclock(); fd_log_private_2( 5, _fd_log_msg_now, __FILE__, __LINE__, __func__, fd_log_private_hexdump_msg a ); } while(0)
#define FD_LOG_HEXDUMP_ALERT(a)   do { long _fd_log_msg_now = fd_log_wallclock(); fd_log_private_2( 6, _fd_log_msg_now, __FILE__, __LINE__, __func__, fd_log_private_hexdump_msg a ); } while(0)
#define FD_LOG_HEXDUMP_EMERG(a)   do { long _fd_log_msg_now = fd_log_wallclock(); fd_log_private_2( 7, _fd_log_msg_now, __FILE__, __LINE__, __func__, fd_log_private_hexdump_msg a ); } while(0)

/* FD_TEST is a single statement that evaluates condition c and, if c
   evaluates to false, will FD_LOG_ERR that the condition failed.  It is
   optimized for the case where c will is non-zero.  This is mostly
   meant for use in things like unit tests.  Due to linguistic
   limitations, c cannot contain things like double quotes, etc.  E.g.

     FD_TEST( broken_func_that_should_return_zero( arg1, arg2 )!=0 );

   would typically cause the program to exit with error code 1, logging
   something like:

     ERR     01-23 04:56:07.890123 45678 f0 0 src/foo.c(901): FAIL: broken_func_that_should_return_zero( arg1, arg2 )!=0

   to the ephemeral log (stderr) and something like:

     ERR     2023-01-23 04:56:07.890123456 GMT-06 45678:45678 user:host:f0 app:thread:0 src/foo.c(901)[func]: FAIL: broken_func_that_should_return_zero( arg1, arg2 )!=0

   to the permanent log.  And similarly for other log levels.

   This macro is robust. */

#define FD_TEST(c) do { if( FD_UNLIKELY( !(c) ) ) FD_LOG_ERR(( "FAIL: %s", #c )); } while(0)

/* Macros for doing hexedit / tcpdump-like logging of memory regions.
   E.g.

     FD_LOG_NOTICE(( "cache line %016lx\n\t"
                     "%02x: " FD_LOG_HEX16_FMT "\n\t"
                     "%02x: " FD_LOG_HEX16_FMT "\n\t"
                     "%02x: " FD_LOG_HEX16_FMT "\n\t"
                     "%02x: " FD_LOG_HEX16_FMT,
                     (ulong)mem,
                      0U, FD_LOG_HEX16_FMT_ARGS( mem    ),
                     16U, FD_LOG_HEX16_FMT_ARGS( mem+16 ),
                     32U, FD_LOG_HEX16_FMT_ARGS( mem+32 ),
                     48U, FD_LOG_HEX16_FMT_ARGS( mem+48 ) ));

   would log something like:

     NOTICE  01-23 04:56:07.890123 45678 f0 0 src/foo.c(901): cache line 0123456789abcd00
             00: 00 01 02 03 04 05 06 07  08 09 0a 0b 0c 0d 0e 0f
             10: 10 11 12 13 14 15 16 17  18 19 1a 1b 1c 1d 1e 1f
             20: 20 21 22 23 24 25 26 27  28 29 2a 2b 2c 2d 2e 2f
             30: 30 31 32 33 34 35 36 37  38 39 3a 3b 3c 3d 3e 3f

   to the ephermal log typically (and a more detailed message to the
   permanent log).  And similarly for the other log levels.  b should be
   safe against multiple evaluation. */

#define FD_LOG_HEX16_FMT "%02x %02x %02x %02x %02x %02x %02x %02x  %02x %02x %02x %02x %02x %02x %02x %02x"
#define FD_LOG_HEX16_FMT_ARGS(b)                                      \
  (uint)(((uchar const *)(b))[ 0]), (uint)(((uchar const *)(b))[ 1]), \
  (uint)(((uchar const *)(b))[ 2]), (uint)(((uchar const *)(b))[ 3]), \
  (uint)(((uchar const *)(b))[ 4]), (uint)(((uchar const *)(b))[ 5]), \
  (uint)(((uchar const *)(b))[ 6]), (uint)(((uchar const *)(b))[ 7]), \
  (uint)(((uchar const *)(b))[ 8]), (uint)(((uchar const *)(b))[ 9]), \
  (uint)(((uchar const *)(b))[10]), (uint)(((uchar const *)(b))[11]), \
  (uint)(((uchar const *)(b))[12]), (uint)(((uchar const *)(b))[13]), \
  (uint)(((uchar const *)(b))[14]), (uint)(((uchar const *)(b))[15])

#define FD_LOG_HEX20_FMT "%02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x"
#define FD_LOG_HEX20_FMT_ARGS(b)                                      \
  FD_LOG_HEX16_FMT_ARGS(b),                                           \
  (uint)(((uchar const *)(b))[16]), (uint)(((uchar const *)(b))[17]), \
  (uint)(((uchar const *)(b))[18]), (uint)(((uchar const *)(b))[19])

#define FD_LOG_NAME_MAX (40UL)

FD_PROTOTYPES_BEGIN

/* APPLICATION LOGICAL IDENTIFIERS ************************************/

/* fd_log_app_id() returns an integer application id of the application
   to which the caller belongs.  An application id is intended, at a
   minimum, to uniquely identify all concurrently running applications
   in the enterprise.  This is cheap after the first call. */

FD_FN_PURE ulong fd_log_app_id( void );

/* fd_log_app() returns a non-NULL pointer to a cstr describing the
   application to which the caller belongs.  This is typically something
   provided to the caller when the caller started.  This is cheap after
   the first call and the lifetime of the returned string is infinite
   from the caller's point of view.  strlen(fd_log_app()) is in
   [1,FD_LOG_NAME_MAX). */

FD_FN_CONST char const * fd_log_app( void ); /* Pointer is CONST, cstr pointed at is PURE */

/* fd_log_thread_id() returns the caller's integer thread id.  A thread
   id is intended, at a minimum, to be unique over all concurrently
   running threads in the application.  This is cheap after the first
   call. */

ulong fd_log_thread_id( void );

/* fd_log_thread() returns a non-NULL pointer to a cstr describing the
   caller.  This defaults to some target specific default essentially
   determined at the caller's startup and can be explicitly set by the
   caller.  This is cheap after the first call within a thread and the
   lifetime of the returned pointer is until the next time the name is
   set or the caller terminates.  strlen(fd_log_thread()) is in
   [1,FD_LOG_NAME_MAX). */

char const * fd_log_thread( void );

/* fd_log_thread_set() sets the caller's description to the cstr
   pointed to by name.  A NULL name and/or an empty name ("") indicate
   to reset to the description that would have been assigned if the
   caller started at the time this is called.  name is not changed by
   the function and the fd_log does not retain any interest in name
   after return.  The actual resulting description will be truncated to
   a strlen of FD_LOG_NAME_MAX-1 if name is longer and potentially
   sanitized in other ways as necessary for the log. */

void
fd_log_thread_set( char const * name );

/* APPLICATION PHYSICAL IDENTIFIERS ***********************************/

/* fd_log_host_id() returns an integer host id of the host on which the
   caller is running.  A host id is intended, at a minimum, to uniquely
   identify a host enterprise wide.  This cheap after the first call. */

FD_FN_PURE ulong fd_log_host_id( void );

/* fd_log_host() returns a non-NULL pointer to a cstr describing the
   host on which the caller is running.  In simple cases, this defaults
   to the hostname.  In general cases, this is something provided to the
   caller at that caller's startup.  This is cheap after the first call
   and the lifetime of the returned string is infinite from the caller's
   point of view.  strlen(fd_log_host()) is in [1,FD_LOG_NAME_MAX). */

FD_FN_CONST char const * fd_log_host( void ); /* ptr is CONST, cstr pointed at is PURE */

/* fd_log_cpu_id() returns an integer cpu id of one of the cpus on
   where the caller was allowed to run when first called by a thread (or
   boot if the caller is the one that booted fd).  A cpu id is intended
   to uniquely identify a cpu on a host (e.g. for a host with
   homogeneous x86 cores, idx from /proc/cpuinfo).  This is cheap after
   the first call. */

ulong fd_log_cpu_id( void );

/* fd_log_cpu() returns a non-NULL pointer to a cstr describing the cpu
   on which the caller is running.  This defaults to some target
   specific default determined when first called on a thread (or boot if
   the caller is the one that booted fd).  This is cheap after the first
   call by a thread and the returned string is infinite from the
   caller's point of view.  strlen(fd_log_cpu()) is in
   [1,FD_LOG_NAME_MAX). */

char const * fd_log_cpu( void );

/* fd_log_cpu_set() sets the description of the cpu on which the caller
   is running on to the cstr pointed to by name.  A NULL name and/or an
   empty name ("") indicate to reset to the description that would have
   been assigned if the caller started at the time this is called.  name
   is not changed by the function and the fd_log does not retain any
   interest in name after return.  The actual resulting description will
   be truncated to a strlen of FD_LOG_NAME_MAX-1 if name is longer and
   potentially sanitized in other ways as necessary for the log. */

void
fd_log_cpu_set( char const * name );

/* THREAD GROUP RELATED IDENTIFIERS ***********************************/

/* fd_log_group_id() returns the thread group id of the thread group to
   which the caller belongs.  The thread group id is intended, at a
   minimum, to be unique over all thread groups on a host.  In simple
   cases, this is the OS pid of the process to which the caller belongs.
   In general cases, this is typically something provided to the caller
   when the caller started.  This is cheap after the first call.

   For sanity, this should be at least 2 (e.g. in POSIX group_id is
   equivalent to pid and pids<=1 are special such that a user is highly
   likely to assume group ids <= 1 are special). */

FD_FN_PURE ulong fd_log_group_id( void );

/* fd_log_group() returns a non-NULL pointer to a cstr describing the
   thread group to which the caller belongs.  In simple cases, this
   defaults to an abbreviated version of argv[0].  In general cases,
   this is typically something provided to the caller when the caller
   started.  This is cheap after the first call and the lifetime of the
   returned string is infinite from the caller's point of view.  The
   actual pointer and cstr is the same for all threads in the group. */

FD_FN_CONST char const * fd_log_group( void ); /* ptr is CONST, cstr pointed at is PURE */

/* fd_log_tid() returns the caller's thread group thread id.  A thread
   group thread id is intended, at a minimum, to be unique over all
   running threads in a thread group.  In simple cases, this is the
   caller's OS tid.  In general cases, this is typically something
   provided to the thread when that thread started.  This is cheap after
   the first call. */

ulong fd_log_tid( void );

/* fd_log_user() returns a non-NULL pointer to a cstr describing the
   user that created the thread group to which the caller belongs.  In
   simple cases, this defaults to the LOGNAME / login that started the
   process running the caller.  In general cases, this is something
   provided to the caller at that caller's startup.  This is cheap after
   the first call and the lifetime of the returned string is infinite
   from the caller's point of view.  strlen(fd_log_user()) is in
   [1,FD_LOG_NAME_MAX). */

FD_FN_CONST char const * fd_log_user( void ); /* ptr is CONST, cstr pointed at is PURE */

/* FIXME: TID DESC? UID? UID_SET? */

/* Logging helper APIs ************************************************/

/* fd_log_wallclock() reads the host's wallclock as ns since the UNIX
   epoch GMT.  On x86, this uses clock_gettime/CLOCK_REALTIME under the
   hood and is reasonably cheap (~25-50 ns nowadays).  But it still may
   involve system calls under the hood and is much slower than, say,
   RTSDC. */

long fd_log_wallclock( void );

/* fd_log_wallclock_cstr( t, buf ) pretty prints the wallclock
   measurement t as:
     "YYYY-MM-DD hh:mm:ss.nnnnnnnnn GMT+TZ".
   or in cases where conversion is not locally practical:
     "         ssssssssss.nnnnnnnnn s UNIX"
   buf must be a character buffer of at least
   FD_LOG_WALLCLOCK_CSTR_BUF_SZ bytes.  Returns buf and buf will be
   populated with the desired cstr on return. */

#define FD_LOG_WALLCLOCK_CSTR_BUF_SZ (37UL)

char *
fd_log_wallclock_cstr( long   t,
                       char * buf );

/* fd_log_sleep puts the calling thread to sleep for dt ns.  dt<=0 is
   assumed to be a sched_yield request.  Returns the amount of sleep
   remaining if the sleep was interrupted. */

long
fd_log_sleep( long dt );

/* fd_log_wait_until waits until fd_log_wallclock() is at least then.
   Returns the time on the clock when the wait ended (will be at least
   then).  This makes a best effort to be a good citizen and sleep /
   yield / hyperthreading friendly the caller while also being as
   precise on the wait as possible (i.e. limited by the overhead
   fd_log_wallclock).  That is, as the time remaining to wait decreases,
   the wait gets progressively more precise and CPU intensive.  If
   remaining is the number of ns remaining in the wait, then:

               remaining <~   1 us: spin
       1 us <~ remaining <~ 100 ms: hyper threading friendly spin
     100 ms <~ remaining <~   1  s: yielding spin
       1  s <~ remaining          : sleep until ~100 ms remaining

   If (as is usually the case) fd_log_sleep precision is much better
   than <<~100 ms accurate, FD_YIELD() delays take <<~100ms and
   FD_SPIN_PAUSE() << 1 us, the return value will be an accurate read of
   the fd_log_wallclock at the time of return and within the overhead of
   fd_log_wallclock. */

long
fd_log_wait_until( long then );

/* fd_log_flush() manually flushes the log (e.g. log a bunch of low
   priority messages and then flush to ensure the bunch gets written out
   before proceeding). */

void
fd_log_flush( void );

/* These all the logging levels to be configured at runtime.  These do
   no validation of there inputs so the values may not behave like the
   caller things (e.g. stderr<logfile will be treated as
   stderr==logfile, flush<stderr will be treated as flush==stderr,
   core<4 will be treated as 4).  colorize returns the colorization mode
   of the ephemeral log.  Currently, zero indicates no colorization of
   the ephmeral log and non-zero indicates to colorize it. */

int fd_log_colorize( void );
int fd_log_level_logfile ( void );
int fd_log_level_stderr  ( void );
int fd_log_level_flush   ( void );
int fd_log_level_core    ( void );

void fd_log_colorize_set     ( int mode  );
void fd_log_level_logfile_set( int level );
void fd_log_level_stderr_set ( int level );
void fd_log_level_flush_set  ( int level );
void fd_log_level_core_set   ( int level );

/* These functions are for fd_log internal use only. */

char const *
fd_log_private_0( char const * fmt, ... ) __attribute__((format(printf,1,2))); /* Type check the fmt string at compile time */

void
fd_log_private_1( int          level,
                  long         now,
                  char const * file,
                  int          line,
                  char const * func,
                  char const * msg );

void
fd_log_private_2( int          level,
                  long         now,
                  char const * file,
                  int          line,
                  char const * func,
                  char const * msg ) __attribute__((noreturn)); /* Let compiler know this will not be returning */

char const * 
fd_log_private_hexdump_msg( char const * tag,
                            void const * mem,
                            ulong        sz );

void
fd_log_private_boot( int *    pargc,
                     char *** pargv );

void
fd_log_private_halt( void );

/* These are exposed to allow the user to override the values set at
   boot/halt time.  If these are used, they are usually a sign of
   working around a higher level architectural or operational issue. */

void fd_log_private_app_id_set   ( ulong app_id    );
void fd_log_private_thread_id_set( ulong thread_id );
void fd_log_private_host_id_set  ( ulong host_id   );
void fd_log_private_cpu_id_set   ( ulong cpu_id    );
void fd_log_private_group_id_set ( ulong group_id  );
void fd_log_private_tid_set      ( ulong tid       );

void fd_log_private_app_set  ( char const * app   ); /* Not thread safe */
void fd_log_private_host_set ( char const * host  ); /* Not thread safe */
void fd_log_private_group_set( char const * group ); /* Not thread safe */
void fd_log_private_user_set ( char const * user  ); /* Not thread safe */

FD_PROTOTYPES_END

#endif /* HEADER_fd_src_util_log_fd_log_h */

