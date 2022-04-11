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
#include <spl/net/socket.h>
#include <ctype.h>

#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif

static void socketInit(socketBase *sock)
{
	sock->m_saAddr.sin_family = AF_INET;
	sock->m_saAddr.sin_port = htons(0);
	sock->m_saAddr.sin_addr.s_addr = htonl(0);
	sock->m_errorStatus = 0;
	sock->m_closed = false;
}

socketBase* socketFdNew(SOCKET fd)
{
	socketBase* sock = socketServerNew();
	if (sock == NULL)
	{
		return NULL;
	}
	
	sock->m_fd = fd;
	return sock;
}

socketBase *socketServerNew()
{
	socketBase *sock = (socketBase *)malloc(sizeof(socketBase));
	if (sock == NULL)
	{
		return NULL;
	}
	sock->m_fd = 0;
	socketInit(sock);
	return sock;
}

socketBase *socketNew(int port)
{
	socketBase *sock = socketServerNew();
	
	sock->m_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock->m_fd < 0)
	{
		free(sock);
		return NULL;
	}
	sock->m_saAddr.sin_port = htons(port);
	return sock;
}

void socketDelete(socketBase *sock)
{
	if (sock->m_fd > 0)
	{
		socketClose (sock);
	}
	free(sock);
}

void socketClose(socketBase *sock) 
{ 
	if (! sock->m_closed) 
	{ 
		sock->m_closed = true; 
		if (0 != shutdown(sock->m_fd, SD_BOTH))
		{
			sock->m_errorStatus = errno;
		}
		closesocket(sock->m_fd); 
	}
}

bool socketSetRecvTimeout(socketBase* sock, int toMS)
{
	int ret = setsockopt(sock->m_fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&toMS, sizeof(int));
#if defined(_WIN32) || defined(_WIN32)
	if ( ret != NO_ERROR )
#else
	if ( ret < 0 )
#endif
	{
		return false;
	}
	
	return true;
}

int socketGetErrorCode(const socketBase *sock)
{ 
	return sock->m_errorStatus; 
}

string* socketGetRemoteIp(socketBase *sock)
{
	SOCKADDR_IN name;
	char buf[17];

	socklen_t namelen = sizeof(name);

	if (0 != getpeername(sock->m_fd, (struct sockaddr *)&name, &namelen))
	{
		return NULL;
	}
	
	StrCpyLen(buf, inet_ntoa(name.sin_addr), sizeof(buf));
	return stringNew(buf);
}

bool socketSetLingerOn(socketBase *sock)
{
	struct linger l;

	l.l_onoff = 1;
	l.l_linger = 5;
	if (setsockopt (sock->m_fd, SOL_SOCKET, SO_LINGER, (char *)&l, sizeof(struct linger)) != 0)
	{
		return false;
	}
	
	return true;
}

bool socketSetBlocking(socketBase *sock)
{
#if defined(_WIN32) || defined(_WIN64)
	u_long arg = 0;
	if (ioctlsocket(sock->m_fd, FIONBIO, &arg) != 0)
	{
		return false;
	}
#else
	int flags;

	if ((flags = fcntl(sock->m_fd, F_GETFL, 0)) < 0 ||
			fcntl(sock->m_fd, F_SETFL, flags & ~O_NONBLOCK) < 0)
	{
		return false;
	}	
#endif

	return true;
}

bool socketSetNonBlocking(socketBase *sock)
{
#if defined(_WIN32) || defined(_WIN64)
	u_long arg = 1;
	if (ioctlsocket(sock->m_fd, FIONBIO, &arg) != 0)
	{
		return false;
	}
#else
	int flags;

	if ((flags = fcntl(sock->m_fd, F_GETFL, 0)) < 0 ||
			fcntl(sock->m_fd, F_SETFL, flags | O_NONBLOCK) < 0)
	{
		return false;
	}	
#endif

	return true;
}

bool socketSend (socketBase *sock, const byte* data, const int offset, const int sendLen)
{
	int total = 0;

#ifndef __TANDEM
	total = send (sock->m_fd, &(((const char *)data)[offset]), sendLen, 0);
	if (total < 0)
	{
#if defined(_WIN32) || defined(_WIN64)
		sock->m_errorStatus = WSAGetLastError();
#else
		sock->m_errorStatus = errno;
#endif
		return false;
	}
#else
	int count;
			
	while (total < sendLen)
	{
		int len = sendLen - total;
		if (len > 8192)
		{
			len = 8192;
		}
		
		count = send (sock->m_fd, &(((const char *)data)[offset + total]), len, 0);
		if (count < 0)
		{
#ifdef _WIN32
			sock->m_errorStatus = WSAGetLastError();
#else
			sock->m_errorStatus = errno;
#endif
			break;
		}
		
		if (count != len)
		{
#ifdef _WIN32
			sock->m_errorStatus = WSAGetLastError();
#else
			sock->m_errorStatus = errno;
#endif
			break;
		}
		
		total += count;
	}
#endif
	
	return total == sendLen;
}

bool socketSendChars (socketBase *sock, const char* data)
{
	return socketSend(sock, (const byte *)data, 0, (int)strlen(data));
}

int socketRecv (socketBase *sock, byte* buf, const int offset, const int recvLen)
{
	int count = recv (sock->m_fd, &(((char *)buf)[offset]), recvLen, 0);
	if ( 0 > count )
	{
#if defined(_WIN32) || defined(_WIN64)
		sock->m_errorStatus = WSAGetLastError();
#else
		sock->m_errorStatus = errno;
#endif
		return -1;
	}
	return count;
}

int socketRecvByte(socketBase *sock)
{
	byte b;
	int len;
	if(0 >= (len = socketRecv(sock, &b, 0, 1)))
	{
		return len;
	}
	
	return (int)b;
}

int socketGetBytesAvail(socketBase* sock)
{
#if defined(_WIN32) || defined(_WIN64)
	u_long arg = 0;
	if (ioctlsocket(sock->m_fd, FIONREAD, &arg) == 0)
	{
		return (int)arg;
	}
	else
	{
		sock->m_errorStatus = WSAGetLastError();
		return -1;
	}
#else
	int arg = 0;
	if ( ioctl(sock->m_fd, FIONREAD, &arg) != 0 )
	{
		sock->m_errorStatus = errno;
		return -1;
	}
	return arg;
#endif	
}
