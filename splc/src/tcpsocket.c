#include <spl/net/tcpsocket.h>
#include <ctype.h>

#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif

#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif

#ifdef __TANDEM
/*#include <spt_extensions.h>*/
#endif

tcpsocket* tcpsocketNew(const char *addr, int port)
{
	tcpsocket* sock = (tcpsocket *)malloc(sizeof(tcpsocket));
	if (sock == NULL)
	{
		return NULL;
	}

	if (NULL == (sock->m_address = stringNew(addr)))
	{
		free(sock);
		return NULL;
	}

	if (NULL == (sock->m_sock = socketNew(port)))
	{
		stringDelete(sock->m_address);
		free(sock);
		return NULL;
	}
	
	return sock;
}

void tcpsocketDelete(tcpsocket *sock)
{
	socketClose(sock->m_sock);
	socketDelete(sock->m_sock);
	stringDelete(sock->m_address);
	free(sock);
}

#include <stdio.h>

bool tcpsocketConnect(tcpsocket *sock)
{
	ASSERT_MEM(sock, sizeof(tcpsocket));
	ASSERT_MEM(sock->m_sock, sizeof(socketBase));
	ASSERT_MEM(sock->m_sock, sizeof(socketBase));
	ASSERT_MEM(sock->m_address, sizeof(string));
	ASSERT_PTR(sock->m_address->m_cstr);
	
	if (isdigit(stringCharAt(sock->m_address, 0)))
	{
		sock->m_sock->m_saAddr.sin_addr.s_addr = inet_addr(stringGetChars(sock->m_address));
	}
	else
	{
		const char *host = stringGetChars(sock->m_address);
		struct hostent *phostent = gethostbyname(host);
		if (phostent == NULL)
		{
			printf("gethostbyname failed\n");
			return false;
		}
		memcpy ((char *)&(sock->m_sock->m_saAddr.sin_addr), phostent->h_addr, phostent->h_length);
	}
	if (connect (sock->m_sock->m_fd, (struct sockaddr *)&sock->m_sock->m_saAddr, sizeof (SOCKADDR_IN)) != 0)
	{
		sock->m_sock->m_errorStatus = errno;
		return false;
	}
	
	return true;
}
