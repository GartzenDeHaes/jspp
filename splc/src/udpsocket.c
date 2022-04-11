#include <spl/net/socket.h>
#include <spl/net/udpsocket.h>

int udpsocketSend(const char* host, int port, byte* data, int datalen)
{
	struct sockaddr_in server;
	struct hostent* hp;
	unsigned int addr;
	SOCKET sock;
	
    if (isalpha(host[0]))
    {   
		/* server address is a name */
        hp = gethostbyname(host);
    }
    else
    { 
		/* Convert nnn.nnn address to a usable one */
        addr = inet_addr(host);
        hp = gethostbyaddr((char *)&addr, 4, AF_INET);
    }

	if (hp == NULL)
	{
		return -1;
	}
	
    memset(&server, 0, sizeof(server));

    memcpy(&(server.sin_addr), hp->h_addr, hp->h_length);

    server.sin_family = hp->h_addrtype;

    server.sin_port = htons(port);

    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	
	if (sock < 0)
	{
		return -1;
	}
	
	return sendto(sock, data, datalen, 0, (struct sockaddr *)&server, sizeof(server));
}
