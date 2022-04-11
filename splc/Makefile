CC=cc
CFLAGS=-c -I. -DDEBUG 
LDFLAGSNOSQL=-lc -lm -Wverbose 
LDFLAGS=$(LDFLAGSNOSQL) 

SPLOBJS = src/debug.o \
 src/commandline.o \
 src/datetime.o \
 src/file.o \
 src/hashtable.o \
 src/integer.o \
 src/packet.o \
 src/real.o \
 src/serversocket.o \
 src/socket.o \
 src/string.o \
 src/stringbuffer.o \
 src/tcpsocket.o \
 src/udpsocket.o \
 src/vector.o \
 src/syslog.o 

OBJECTS= $(SPLOBJS) \
 errors.o \
 httpget.o \
 preparedstatement.o \
 request.o \
 runcheck.o \
 sqllex.o \
 sqlparam.o \
 sqlprocedure.o \
 stmtmgr.o \
 stats.o \
 users.o \
 sqlite3.o \
 src/sqlitesrc.o  

all: sqls defs2xml

sqls: defs2xml $(OBJECTS)
	$(CC) $(LDFLAGS) -o sqls $(OBJECTS) 
	info_define all >defines.txt
	./defs2xml

defs2xml: defs2xml.o $(OBJECTS)
	$(CC) $(LDFLAGSNOSQL) -o defs2xml defs2xml.o $(SPLOBJS)

src/debug.o:	src/debug.c spl/debug.h spl/types.h
	$(CC) $(CFLAGS) src/debug.c -o $@

src/commandline.o:	src/commandline.c
	$(CC) $(CFLAGS) src/commandline.c -o $@

src/datetime.o:	src/datetime.c
	$(CC) $(CFLAGS) src/datetime.c -o $@

errors.o:	errors.c
	$(CC) $(CFLAGS) errors.c -o $@

src/file.o:	src/file.c
	$(CC) $(CFLAGS) src/file.c -o $@

src/hashtable.o:	src/hashtable.c spl/collection/hashtable.h
	$(CC) $(CFLAGS) src/hashtable.c -o $@

src/integer.o:	src/integer.c
	$(CC) $(CFLAGS) src/integer.c -o $@

src/packet.o:	src/packet.c
	$(CC) $(CFLAGS) src/packet.c -o $@

preparedstatement.o:	preparedstatement.c
	$(CC) $(CFLAGS) preparedstatement.c -o $@

src/real.o:	src/real.c
	$(CC) $(CFLAGS) src/real.c -o $@

request.o:	request.c
	$(CC) $(CFLAGS) request.c -o $@

runcheck.o:	runcheck.c
	$(CC) $(CFLAGS) runcheck.c -o $@

src/serversocket.o:	src/serversocket.c spl/net/socket.h spl/net/serversocket.h
	$(CC) $(CFLAGS) src/serversocket.c -o $@

src/socket.o:	src/socket.c spl/net/socket.h
	$(CC) $(CFLAGS) src/socket.c -o $@

httpget.o:
	$(CC) $(CFLAGS) httpget.c -o $@

sqllex.o:	sqllex.c
	$(CC) $(CFLAGS) sqllex.c -o $@

sqlparam.o:	sqlparam.c
	$(CC) $(CFLAGS) sqlparam.c -o $@

sqlprocedure.o:
	$(CC) $(CFLAGS) sqlprocedure.c -o $@

stmtmgr.o:	stmtmgr.c
	$(CC) $(CFLAGS) stmtmgr.c -o $@

stats.o:	stats.c
	$(CC) $(CFLAGS) stats.c -o $@

src/string.o:	src/string.c spl/string.h
	$(CC) $(CFLAGS) src/string.c -o $@

src/stringbuffer.o:	src/stringbuffer.c spl/string.h spl/stringbuffer.h
	$(CC) $(CFLAGS) src/stringbuffer.c -o $@

src/tcpsocket.o:
	$(CC) $(CFLAGS) src/tcpsocket.c -o $@

src/udpsocket.o:
	$(CC) $(CFLAGS) src/udpsocket.c -o $@

users.o:
	$(CC) $(CFLAGS) users.c -o $@

src/vector.o:
	$(CC) $(CFLAGS) src/vector.c -o $@

sqlite3.o: sqlite3.c sqlite3.h
	$(CC) $(CFLAGS) -Wnowarn=262,1506 sqlite3.c -o $@

src/sqlitesrc.o:
	$(CC) $(CFLAGS) src/sqlitesrc.c -o $@

src/syslog.o:
	$(CC) $(CFLAGS) src/syslog.c -o $@

defs2xml.o:
	$(CC) $(CFLAGS) defs2xml.c -o $@	

clean:
	rm -rf *o
	rm -rf *.a
	rm -rf *.exe
	rm -rf sqls
	rm -rf defs2xml
