#include <spl/net/tcpsocket.h>
#include <spl/stringbuffer.h>
#include "httpget.h"

string* httpget(const char* server, int port, const char* pathAndParams)
{
	int recvSize;
	tcpsocket* sock;
	byte bbuf[512];
	string* ret = NULL;
	stringbuffer* buf = stringbufferNew();
	if (NULL == buf)
	{
		printf("httpget create buffer failed\n");
		return NULL;
	}
	
	sock = tcpsocketNew(server, port);
	if (NULL == sock)
	{
		printf("httpget tcpsocketNew failed\n");
		stringbufferDelete(buf);
		return NULL;
	}
	
	socketSetBlocking(sock->m_sock);
	socketSetRecvTimeout(sock->m_sock, 15 * 1000);

	tcpsocketConnect(sock);

	stringbufferAppend(buf, "GET ");
	stringbufferAppend(buf, pathAndParams);
	stringbufferAppend(buf, " HTTP/1.0\r\n\r\n");
	
	socketSendChars(sock->m_sock, buf->m_buf);
	
	recvSize = socketRecv(sock->m_sock, bbuf, 0, sizeof(bbuf));
	
	socketClose(sock->m_sock);
	
	if (recvSize > 0)
	{
		bbuf[recvSize] = '\0';
		ret = stringNew(bbuf);
	}
	
	stringbufferDelete(buf);
	tcpsocketDelete(sock);
	
	return ret;
}
