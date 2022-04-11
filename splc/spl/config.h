/* spl/autoconf/config.h.  Generated from config.h.in by configure.  */
/* spl/autoconf/config.h.in.  Generated from configure.ac by autoheader.  */


/* On Unix-like systems config.h.in is converted by "configure" into config.h.
Some other environments also support the use of "configure". PCRE is written in
Standard C, but there are a few non-standard things it can cope with, allowing
it to run on SunOS4 and other "close to standard" systems.

If you are going to build PCRE "by hand" on a system without "configure" you
should copy the distributed config.h.generic to config.h, and then set up the
macro definitions the way you need them. You must then add -DHAVE_CONFIG_H to
all of your compile commands, so that config.h is included at the start of
every source.

Alternatively, you can avoid editing by using -D on the compiler command line
to set the macro values. In this case, you do not have to set -DHAVE_CONFIG_H.

PCRE uses memmove() if HAVE_MEMMOVE is set to 1; otherwise it uses bcopy() if
HAVE_BCOPY is set to 1. If your system has neither bcopy() nor memmove(), set
them both to 0; an emulation function will be used. */

/* By default, the \R escape sequence matches any Unicode line ending
   character or sequence of characters. If BSR_ANYCRLF is defined, this is
   changed so that backslash-R matches only CR, LF, or CRLF. The build- time
   default can be overridden by the user of PCRE at runtime. On systems that
   support it, "configure" can be used to override the default. */
/* #undef BSR_ANYCRLF */

/* Define to 1 if the `closedir' function returns void instead of `int'. */
/* #undef CLOSEDIR_VOID */

/* If you are compiling for a system that uses EBCDIC instead of ASCII
   character codes, define this macro as 1. On systems that can use
   "configure", this can be done via --enable-ebcdic. PCRE will then assume
   that all input strings are in EBCDIC. If you do not define this macro, PCRE
   will assume input strings are ASCII or UTF-8 Unicode. It is not possible to
   build a version of PCRE that supports both EBCDIC and UTF-8. */
/* #undef EBCDIC */

/* Define to 1 if you have the `alarm' function. */
#define HAVE_ALARM 1

/* Define to 1 if you have the <arpa/inet.h> header file. */
#if !defined(_WIN32) && !defined(_WIN64)
#define HAVE_ARPA_INET_H 1
#endif

/* Define to 1 if you have the <assert.h> header file. */
#define HAVE_ASSERT_H 1

/* Define to 1 if the compiler supports atomic builtins. */
/* #undef HAVE_ATOMIC_BUILTINS */

/* Define to 1 if you have the `bcopy' function. */
#define HAVE_BCOPY 1

/* Define to 1 if you have the <bits/type_traits.h> header file. */
/* #undef HAVE_BITS_TYPE_TRAITS_H */

/* Define to 1 if you have the <bzlib.h> header file. */
/* #undef HAVE_BZLIB_H */

/* Define to 1 if you have the declaration of `strerror_r', and to 0 if you
   don't. */
#define HAVE_DECL_STRERROR_R 0

/* Define to 1 if you have the <dirent.h> header file. */
#define HAVE_DIRENT_H 1

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Define to 1 if you don't have `vprintf' but do have `_doprnt.' */
/* #undef HAVE_DOPRNT */

/* Define to 1 if you have the <errno.h> header file. */
#define HAVE_ERRNO_H 1

/* Define to 1 if you have the <fcntl.h> header file. */
#define HAVE_FCNTL_H 1

/* Define to 1 if you have the <float.h> header file. */
#define HAVE_FLOAT_H 1

/* Define to 1 if you have the `getcwd' function. */
#define HAVE_GETCWD 1

/* Define to 1 if you have the `gethostbyname' function. */
/* #undef HAVE_GETHOSTBYNAME */

/* Define to 1 if you have the `inet_ntoa' function. */
#define HAVE_INET_NTOA 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the <io.h> header file. */
/* #undef HAVE_IO_H */

/* Define to 1 if you have the `isatty' function. */
#define HAVE_ISATTY 1

/* Define to 1 if you have the <limits.h> header file. */
#define HAVE_LIMITS_H 1

/* Define to 1 if the system has the type `long long'. */
#define HAVE_LONG_LONG 1

/* Define to 1 if you have the `memmove' function. */
#define HAVE_MEMMOVE 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the `memset' function. */
#define HAVE_MEMSET 1

/* Define to 1 if you have the <mysql.h> header file. */
/* #undef HAVE_MYSQL_H */

/* Define to 1 if you have the <mysql/mysql.h> header file. */
/* #undef HAVE_MYSQL_MYSQL_H */

/* Define to 1 if you have the <ndir.h> header file, and it defines `DIR'. */
/* #undef HAVE_NDIR_H */

/* Define to 1 if you have the <netdb.h> header file. */
#if !defined(_WIN32) && !defined(_WIN64)
#define HAVE_NETDB_H 1
#endif

#if !defined(_WIN32) && !defined(_WIN64)
/* Define to 1 if you have the <netinet/in.h> header file. */
#define HAVE_NETINET_IN_H 1
#endif

/* Define to 1 if you have the <netinet/tcp.h> header file. */
/* #undef HAVE_NETINET_TCP_H */

/* Define to 1 if you have the <pthread.h> header file. */
/* #undef HAVE_PTHREAD_H */

/* Define to 1 if you have the <pwd.h> header file. */
#define HAVE_PWD_H 1

/* Define to 1 if you have the <readline/history.h> header file. */
/* #undef HAVE_READLINE_HISTORY_H */

/* Define to 1 if you have the <readline/readline.h> header file. */
/* #undef HAVE_READLINE_READLINE_H */

/* Define to 1 if you have the `select' function. */
#define HAVE_SELECT 1

/* Define to 1 if you have the <semaphore.h> header file. */
/* #undef HAVE_SEMAPHORE_H */

/* Define to 1 if you have the `socket' function. */
#define HAVE_SOCKET 1

/* Define to 1 if you have the <socket.h> header file. */
/*define HAVE_SOCKET_H 1*/

/* Define to 1 if you have the <spthread.h> header file. */
/*#define HAVE_SPTHREAD_H 1*/

/* Define to 1 if you have the <sqldb.h> header file. */
/* #undef HAVE_SQLDB_H */

/* Define to 1 if you have the <sqlfront.h> header file. */
/* #undef HAVE_SQLFRONT_H */

/* Define to 1 if `stat' has the bug that it succeeds when given the
   zero-length file name argument. */
/* #undef HAVE_STAT_EMPTY_STRING_BUG */

/* Define to 1 if stdbool.h conforms to C99. */
/* #undef HAVE_STDBOOL_H */

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the `strdup' function. */
#define HAVE_STRDUP 1

/* Define to 1 if you have the `strerror' function. */
#define HAVE_STRERROR 1

/* Define to 1 if you have the `strerror_r' function. */
/* #undef HAVE_STRERROR_R */

/* Define to 1 if you have the <string> header file. */
#define HAVE_STRING 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the `strncasecmp' function. */
#define HAVE_STRNCASECMP 1

/* Define to 1 if you have the `strtoll' function. */
#define HAVE_STRTOLL 1

/* Define to 1 if you have the `strtoq' function. */
/* #undef HAVE_STRTOQ */

/* Define to 1 if you have the <sys/dirent.h> header file. */
/* #undef HAVE_SYS_DIRENT_H */

/* Define to 1 if you have the <sys/dir.h> header file, and it defines `DIR'.
   */
/* #undef HAVE_SYS_DIR_H */

/* Define to 1 if you have the <sys/errno.h> header file. */
/* #undef HAVE_SYS_ERRNO_H */

/* Define to 1 if you have the <sys/fcntl.h> header file. */
/* #undef HAVE_SYS_FCNTL_H */

/* Define to 1 if you have the <sys/ioctl.h> header file. */
#if !defined(_WIN32) && !defined(_WIN64)
#define HAVE_SYS_IOCTL_H 1
#endif

/* Define to 1 if you have the <sys/ndir.h> header file, and it defines `DIR'.
   */
/* #undef HAVE_SYS_NDIR_H */

/* Define to 1 if you have the <sys/select.h> header file. */
/* #undef HAVE_SYS_SELECT_H */

/* Define to 1 if you have the <sys/socket.h> header file. */
#if !defined(_WIN32) && !defined(_WIN64)
#define HAVE_SYS_SOCKET_H 1
#endif

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/time.h> header file. */
#if !defined(_WIN32) && !defined(_WIN64)
#define HAVE_SYS_TIME_H 1
#endif

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <sys/utsname.h> header file. */
#define HAVE_SYS_UTSNAME_H 1

/* Define to 1 if you have the <sys/vfs.h> header file. */
/* #undef HAVE_SYS_VFS_H */

/* Define to 1 if you have the <tds.h> header file. */
/* #undef HAVE_TDS_H */

/* Define to 1 if you have the <termcap.h> header file. */
/* #undef HAVE_TERMCAP_H */

/* Define to 1 if you have the <time.h> header file. */
#define HAVE_TIME_H 1

/* Define to 1 if typeof works with your compiler. */
/* #undef HAVE_TYPEOF */

/* Define to 1 if you have the <types.h> header file. */
/* #undef HAVE_TYPES_H */

/* Define to 1 if you have the <type_traits.h> header file. */
/* #undef HAVE_TYPE_TRAITS_H */

/* Define to 1 if you have the <unistd.h> header file. */
#if !defined(_WIN32) && !defined(_WIN64)
#define HAVE_UNISTD_H 1
#endif

/* Define to 1 if the system has the type `unsigned long long'. */
#define HAVE_UNSIGNED_LONG_LONG 1

/* Define to 1 if you have the `vprintf' function. */
#define HAVE_VPRINTF 1

/* Define to 1 if you have the <wchar.h> header file. */
#define HAVE_WCHAR_H 1

/* Define to 1 if you have the <wctype.h> header file. */
#define HAVE_WCTYPE_H 1

/* Define to 1 if you have the <windows.h> header file. */
/* #undef HAVE_WINDOWS_H */

/* Define to 1 if you have the <zlib.h> header file. */
/* #undef HAVE_ZLIB_H */

/* Define to 1 if you have the `_atoi64' function. */
/* #undef HAVE__ATOI64 */

/* Define to 1 if the system has the type `_Bool'. */
/* #undef HAVE__BOOL */

/* Define to 1 if you have the `_isatty' function. */
/* #undef HAVE__ISATTY */

/* Define to 1 if you have the `_strtoi64' function. */
/* #undef HAVE__STRTOI64 */

#define HAVE_SYS_WAIT_H
#define HAVE_SYS_STAT_H
#define HAVE_SYS_UIO_H

/* The value of LINK_SIZE determines the number of bytes used to store links
   as offsets within the compiled regex. The default is 2, which allows for
   compiled patterns up to 64K long. This covers the vast majority of cases.
   However, PCRE can also be compiled to use 3 or 4 bytes instead. This allows
   for longer patterns in extreme cases. On systems that support it,
   "configure" can be used to override this default. */
#define LINK_SIZE 2

/* Define to 1 if `lstat' dereferences a symlink specified with a trailing
   slash. */
#define LSTAT_FOLLOWS_SLASHED_SYMLINK 1

/* Define to the sub-directory in which libtool stores uninstalled libraries.
   */
#define LT_OBJDIR ".libs/"

/* The value of MATCH_LIMIT determines the default number of times the
   internal match() function can be called during a single execution of
   pcre_exec(). There is a runtime interface for setting a different limit.
   The limit exists in order to catch runaway regular expressions that take
   for ever to determine that they do not match. The default is set very large
   so that it does not accidentally catch legitimate cases. On systems that
   support it, "configure" can be used to override this default default. */
#define MATCH_LIMIT 10000000

/* The above limit applies to all calls of match(), whether or not they
   increase the recursion depth. In some environments it is desirable to limit
   the depth of recursive calls of match() more strictly, in order to restrict
   the maximum amount of stack (or heap, if NO_RECURSE is defined) that is
   used. The value of MATCH_LIMIT_RECURSION applies only to recursive calls of
   match(). To have any useful effect, it must be less than the value of
   MATCH_LIMIT. The default is to use the same value as MATCH_LIMIT. There is
   a runtime method for setting a different limit. On systems that support it,
   "configure" can be used to override the default. */
#define MATCH_LIMIT_RECURSION MATCH_LIMIT

/* This limit is parameterized just in case anybody ever wants to change it.
   Care must be taken if it is increased, because it guards against integer
   overflow caused by enormously large patterns. */
#define MAX_NAME_COUNT 10000

/* This limit is parameterized just in case anybody ever wants to change it.
   Care must be taken if it is increased, because it guards against integer
   overflow caused by enormously large patterns. */
#define MAX_NAME_SIZE 32

/* The value of NEWLINE determines the newline character sequence. On systems
   that support it, "configure" can be used to override the default, which is
   10. The possible values are 10 (LF), 13 (CR), 3338 (CRLF), -1 (ANY), or -2
   (ANYCRLF). */
#define NEWLINE 10

/* PCRE uses recursive function calls to handle backtracking while matching.
   This can sometimes be a problem on systems that have stacks of limited
   size. Define NO_RECURSE to get a version that doesn't use recursion in the
   match() function; instead it creates its own stack by steam using
   pcre_recurse_malloc() to obtain memory from the heap. For more detail, see
   the comments and other stuff just above the match() function. On systems
   that support it, "configure" can be used to set this in the Makefile (use
   --disable-stack-for-recursion). */
/* #undef NO_RECURSE */

/* Name of package */
#define PACKAGE "spl"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "http://sourceforge.net/projects/spl"

/* Define to the full name of this package. */
#define PACKAGE_NAME "spl"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "spl 0.4.7"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "spl"

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION "0.4.7"


/* If you are compiling for a system other than a Unix-like system or
   Win32, and it needs some magic to be inserted before the definition
   of a function that is exported by the library, define this macro to
   contain the relevant magic. If you do not define this macro, it
   defaults to "extern" for a C compiler and "extern C" for a C++
   compiler on non-Win32 systems. This macro apears at the start of
   every exported function that is part of the external API. It does
   not appear on functions that are "external" in the C sense, but
   which are internal to the library. */
/* #undef PCRE_EXP_DEFN */

/* Define if linking statically (TODO: make nice with Libtool) */
#define PCRE_STATIC 1

/* When calling PCRE via the POSIX interface, additional working storage is
   required for holding the pointers to capturing substrings because PCRE
   requires three integers per substring, whereas the POSIX interface provides
   only two. If the number of expected substrings is small, the wrapper
   function uses space on the stack, because this is faster than using
   malloc() for each call. The threshold above which the stack is no longer
   used is defined by POSIX_MALLOC_THRESHOLD. On systems that support it,
   "configure" can be used to override this default. */
#define POSIX_MALLOC_THRESHOLD 10

/* Define to the type of arg 1 for `select'. */
#define SELECT_TYPE_ARG1 int

/* Define to the type of args 2, 3 and 4 for `select'. */
#define SELECT_TYPE_ARG234 (int *)

/* Define to the type of arg 5 for `select'. */
#define SELECT_TYPE_ARG5 (struct timeval *)

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Define to 1 if strerror_r returns char *. */
/* #undef STRERROR_R_CHAR_P */

/* Define to allow pcregrep to be linked with libbz2, so that it is able to
   handle .bz2 files. */
/* #undef SUPPORT_LIBBZ2 */

/* Define to allow pcretest to be linked with libreadline. */
/* #undef SUPPORT_LIBREADLINE */

/* Define to allow pcregrep to be linked with libz, so that it is able to
   handle .gz files. */
/* #undef SUPPORT_LIBZ */

/* Define to enable support for Unicode properties */
/* #undef SUPPORT_UCP */

/* Define to enable support for the UTF-8 Unicode encoding. This will work
   even in an EBCDIC environment, but it is incompatible with the EBCDIC
   macro. That is, PCRE can support *either* EBCDIC code *or* ASCII/UTF-8, but
   not both at once. */
/* #undef SUPPORT_UTF8 */

/* Define to 1 if you can safely include both <sys/time.h> and <time.h>. */
#define TIME_WITH_SYS_TIME 1

/* Define to 1 if your <sys/time.h> declares `struct tm'. */
/* #undef TM_IN_SYS_TIME */

/* Version number of package */
#define VERSION "0.4.7"

/* Define to empty if `const' does not conform to ANSI C. */
/* #undef const */

/* Define to `__inline__' or `__inline' if that's what the C compiler
   calls it, or to nothing if 'inline' is not supported under any name.  */
#ifndef __cplusplus
#define inline 
#endif

/* Define to `unsigned int' if <sys/types.h> does not define. */
/* #undef size_t */

/* Define to __typeof__ if your compiler spells it that way. */
/* #undef typeof */

/* Define to empty if the keyword `volatile' does not work. Warning: valid
   code using `volatile' can become incorrect without. Disable with care. */
/* #undef volatile */
