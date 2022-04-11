#include <spl/net/serversocket.h>

serversocket* serversocketNew(int port, int numBuf)
{
	serversocket* sock = (serversocket *)malloc(sizeof(serversocket));
	if (sock == NULL)
	{
		return NULL;
	}
	
	if (NULL == (sock->m_sock = socketNew(port)))
	{
		free(sock);
		return NULL;
	}
	
	sock->m_sock->m_saAddr.sin_addr.s_addr = htonl (INADDR_ANY);

	if (bind(sock->m_sock->m_fd, (struct sockaddr *)&sock->m_sock->m_saAddr, sizeof(sock->m_sock->m_saAddr)) != 0)
	{
		serversocketDelete(sock);
		return NULL;
	}
	
	if (listen(sock->m_sock->m_fd, numBuf) != 0)
	{
		serversocketDelete(sock);
		return NULL;
	}
	
	return sock;	
}

void serversocketDelete(serversocket* sock)
{
	socketDelete(sock->m_sock);
	free(sock);
}

socketBase* serversocketAccept(serversocket* sock)
{
	SOCKET fd;
	SOCKADDR_IN saAddr;
	socketBase* cs;
	socklen_t size = sizeof(saAddr);

	sock->m_sock->m_errorStatus = 0;
	fd = accept(sock->m_sock->m_fd, (struct sockaddr *)&saAddr, &size);
	if (fd == INVALID_SOCKET)
	{
		sock->m_sock->m_errorStatus = errno;
		return NULL;
	}
	
	cs = socketFdNew(fd);
	memcpy(&cs->m_saAddr, &saAddr, sizeof(saAddr));
	/*lookupName(&saAddr, 5, hostName);*/
	
	return cs;
}

void serversocketClose(serversocket* sock)
{
	socketClose(sock->m_sock);
}
