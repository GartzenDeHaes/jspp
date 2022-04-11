/*
 *   This file is part of the Standard Portable Library (SPL).
 *
 *   SPL is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   SPL is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with SPL.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef _sock_h
#define _sock_h

#include <spl/types.h>
#include <spl/debug.h>
#include <spl/string.h>

#include <sys/types.h>

#if defined(_WIN32) || defined(_WIN64)
#define _WINSOCKAPI_
#include <spl/cleanwindows.h>
#include <winsock2.h>
#endif

#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_SYS_FCNTL_H
#include <sys/fcntl.h>
#endif
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif
#ifdef HAVE_NETINET_TCP_H
#include <netinet/tcp.h>
#endif
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif

#if !defined(SOCKET) && !defined(_WIN32) && !defined(_WIN64)
typedef int SOCKET;
#endif
#if !defined(_WINDOWS)
typedef struct sockaddr_in SOCKADDR_IN;
#endif
#if !defined(_WIN32) && !defined(_WIN64)
#define closesocket(A) close(A)
#define INVALID_SOCKET -1
#endif
#if !defined(SD_BOTH)
#define SD_BOTH SHUT_RDWR
#endif
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif
#ifdef HAVE_TIME_H
#include <time.h>
#endif

#ifdef __TANDEM
typedef size_t socklen_t;
#endif

#define SOCKBUF_SIZE 1024

typedef struct _socketBase
{
	int m_errorStatus;	   /* from errno */
	SOCKET m_fd;
	SOCKADDR_IN m_saAddr;
	bool m_closed;
} socketBase;

socketBase* socketFdNew(SOCKET fd);
socketBase* socketNew(int port);
socketBase* socketServerNew();
void socketDelete(socketBase *sock);

void socketClose(socketBase *sock);

bool socketSetRecvTimeout(socketBase *sock, int toMS);
bool socketSetNonBlocking(socketBase *sock);
bool socketSetBlocking(socketBase *sock);
bool socketSetLingerOn(socketBase *sock);

bool socketSend (socketBase* sock, const byte* data, const int offset, const int sendLen);
bool socketSendChars (socketBase* sock, const char* data);
int socketRecv (socketBase* sock, byte* buf, const int offset, const int recvLen);
int socketRecvByte(socketBase* sock);

int socketGetBytesAvail(socketBase* sock);
int socketGetErrorCode(const socketBase* sock);
string* socketGetRemoteIp(socketBase* sock);

#endif
