
lib_LTLIBRARIES = libjspp.la 

libjspp_la_SOURCES = \
 ../jspp/interp.h        ../jspp/jdes.h       ../jspp/jsocket.h  ../jspp/opcodes.h      ../jspp/program.h	\
 ../jspp/interperrors.h  ../jspp/jhttp_lib.h  ../jspp/lex.h      ../jspp/parse.h        ../jspp/variant.h	\
 ../jspp/intrinsics.h    ../jspp/jsclass.h    ../jspp/native.h   ../jspp/parseerrors.h  ../jspp/y.tab.h		\
 ../jspp/config.h	\
 ../src/interp.c       ../src/jdes.c       ../src/jsocket.c   ../src/libcommon.c  ../src/program.c		\
 ../src/interpinner.c  ../src/jhttp_lib.c  ../src/native.c     ../src/programdump.c	\
 ../src/intrinsics.c   ../src/jsclass.c    ../src/lex.c       ../src/parse.c      ../src/variant.c		

INCLUDES = -I..

library_includedir=$(includedir)/jspp
library_include_HEADERS = \
 ../jspp/interp.h        ../jspp/jdes.h       ../jspp/jsocket.h  ../jspp/opcodes.h      ../jspp/program.h	\
 ../jspp/interperrors.h  ../jspp/jhttp_lib.h  ../jspp/lex.h      ../jspp/parse.h        ../jspp/variant.h	\
 ../jspp/intrinsics.h    ../jspp/jsclass.h    ../jspp/native.h   ../jspp/parseerrors.h  ../jspp/y.tab.h		\
 ../jspp/config.h

libjsppdir = $(libdir)/libjspp
#libspl_DATA = ../termcap.src
#libspl_configdir = $(libdir)/libspl
#libspl_config_DATA = spl/config.h

if PLATFORM_OSS
CHECKLIBS = -spthread
else
CHECKLIBS = -lpthread
endif

libjspp_la_LDFLAGS= -version-info 1:0:0 -release 1:0:0

AM_CPPFLAGS = -DMTLIB -DCONSOLE 
#-D_XOPEN_SOURCE=1 -D_POSIX_C_SOURCE=1 -D_XOPEN_SOURCE_EXTENDED=1

bin_PROGRAMS = jspp
jspp_SOURCES = ../cli/mainishere.c
jspp_LDFLAGS = -ljspp -lspl -lc -ltds -lsybdb $(CHECKLIBS)

