AC_INIT(src/interp.c)
AC_CONFIG_HEADERS(jspp/config.h)

AM_INIT_AUTOMAKE(jspp,0.1.3)

AC_PROG_CC

AC_PROG_CXX

AC_PROG_INSTALL

AC_PROG_LIBTOOL

AC_CHECK_LIB(m, pow)
#AC_CHECK_LIB(tds, dblogon)
#AC_CHECK_LIB(sybdb, dblogon)
#AC_CHECK_LIB(spl, socketConstruct)
#AC_CHECK_LIB(pthread, pthread_create)

AC_CHECK_FUNCS([memmove])
AC_CHECK_FUNCS([memset]) 
AC_CHECK_FUNCS([select])
AC_CHECK_FUNCS([socket])
AC_CHECK_FUNCS([strdup]) 
AC_CHECK_FUNCS([strerror])
AC_CHECK_FUNCS([strncasecmp]) 
AC_CHECK_FUNCS([bcopy])

AC_CHECK_HEADERS([arpa/inet.h])
AC_CHECK_HEADERS([fcntl.h])
AC_CHECK_HEADERS([netdb.h]) 
AC_CHECK_HEADERS([netinet/in.h])
AC_CHECK_HEADERS([netinet/tcp.h])
AC_CHECK_HEADERS([io.h])
AC_CHECK_HEADERS([sys/ioctl.h]) 
AC_CHECK_HEADERS([socket.h])
AC_CHECK_HEADERS([sys/socket.h])
AC_CHECK_HEADERS([sys/time.h])
AC_CHECK_HEADERS([wchar.h])
AC_CHECK_HEADERS([sqlfront.h])
AC_CHECK_HEADERS([sqldb.h])
AC_CHECK_HEADERS([sys/fcntl.h])
AC_CHECK_HEADERS([unistd.h])
AC_CHECK_HEADERS([termcap.h])
AC_CHECK_HEADERS([spthread.h])
AC_CHECK_HEADERS([pthread.h])
AC_CHECK_HEADERS([errno.h])
AC_CHECK_HEADERS([time.h])
AC_CHECK_HEADERS([sys/time.h])
AC_CHECK_HEADERS([string.h])
AC_CHECK_HEADERS([memory.h])

PKG_CHECK_MODULES(spl, [libspl-1.0 >= 0.1])

AC_CANONICAL_HOST
AM_CONDITIONAL(PLATFORM_OSS, test "$host_os" = nsk)

AC_C_CONST
AC_C_INLINE
AC_C_VOLATILE
#AC_FUNC_MALLOC
AC_FUNC_MEMCMP
AC_FUNC_MKTIME
#AC_FUNC_REALLOC
AC_FUNC_SELECT_ARGTYPES
AC_FUNC_VPRINTF
AC_HEADER_STDBOOL
AC_HEADER_TIME
AC_PROG_GCC_TRADITIONAL
AC_STRUCT_TM
AC_TYPE_SIZE_T 
AC_OUTPUT(Makefile libjspp-1.0.pc libjsppd-1.0.pc debug/Makefile release/Makefile)

