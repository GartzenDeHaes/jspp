#ifndef _udpsocket_h
#define _udpsocket_h

#include <spl/types.h>

int udpsocketSend(const char* host, int port, byte* data, int datalen);

#endif
