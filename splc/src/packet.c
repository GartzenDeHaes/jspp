#include <spl/net/packet.h>
#include <stdio.h>

static byte packetPeekByte(packet* pkt)
{
	if (packetIsEof(pkt))
	{
		return 0;
	}
	
	return (byte)pkt->m_buf->m_buf[pkt->m_readpos];
}

static byte packetNextByte(packet* pkt)
{
	char ch = packetPeekByte(pkt);	
	pkt->m_readpos++;
	return ch;
}

packet* packetNew(stringbuffer* buf)
{
	packet* pkt = (packet *)malloc(sizeof(packet));
	if (pkt == NULL)
	{
		return NULL;
	}
	
	pkt->m_buf = buf;
	pkt->m_readpos = 0;
	pkt->m_readRevByteOrder = false;
	pkt->m_isError = false;
	
	return pkt;
}

void packetDelete(packet* pkt)
{
	free(pkt);
}

bool packetIsEof(packet* pkt)
{
	return pkt->m_readpos >= pkt->m_buf->m_used;
}

bool packetIsError(packet* pkt)
{
	return pkt->m_isError;
}

bool packetIsEofOrError(packet* pkt)
{
	return packetIsEof(pkt) || packetIsError(pkt);
}

char packetNextType(packet* pkt)
{
	char type = packetPeekByte(pkt);
	ASSERT(type == 'I' || type == 'S' || type == 'd' || type == 'L');
	return type;
}

bool packetIsValidNext(packet* pkt)
{
	char type = packetPeekByte(pkt);
	return type == 'I' || type == 'S' || type == 'd' || type == 'L';
}

int32 packetReadInt(packet* pkt)
{
	char datatype = packetNextByte(pkt);
	if ('I' != datatype )
	{
		pkt->m_isError = true;
		return 0;
	}
#if LITTLE_ENDIAN
	return (packetNextByte(pkt) << 24) | (packetNextByte(pkt) << 16) | (packetNextByte(pkt) << 8) | (packetNextByte(pkt));
#else
	return packetNextByte(pkt) | (packetNextByte(pkt) << 8) | (packetNextByte(pkt) << 16) | (packetNextByte(pkt) << 24);
#endif
}

int64 packetReadLong(packet* pkt)
{
	char datatype = packetNextByte(pkt);
	if ('L' != datatype )
	{
		pkt->m_isError = true;
		return 0;
	}
#if LITTLE_ENDIAN
	return (int64)(((uint64)packetNextByte(pkt) << 56) | ((uint64)packetNextByte(pkt) << 48) | ((uint64)packetNextByte(pkt) << 40) | ((uint64)packetNextByte(pkt) << 32) |
		((uint64)packetNextByte(pkt) << 24) | ((uint64)packetNextByte(pkt) << 16) | ((uint64)packetNextByte(pkt) << 8) | ((uint64)packetNextByte(pkt)));
#else
	return (int64)((uint64)packetNextByte(pkt) | ((uint64)packetNextByte(pkt) << 8) | ((uint64)packetNextByte(pkt) << 16) | ((uint64)packetNextByte(pkt) << 24) |
		((uint64)packetNextByte(pkt) << 32) | ((uint64)packetNextByte(pkt) << 40) | ((uint64)packetNextByte(pkt) << 48) | ((uint64)packetNextByte(pkt) << 56);
#endif
}

double packetReadDouble(packet* pkt)
{
	stringbuffer* buf;
	int len;
	double val;
	int x;
	byte datatype = packetNextByte(pkt);
	
	if ( (byte)'d' != datatype )
	{
		pkt->m_isError = true;
		return 0;
	}
	
	len = packetNextByte(pkt);
	
	if (NULL == (buf = stringbufferNew(len+1)))
	{
		pkt->m_isError = true;
		return 0;		
	}

	for (x = 0; x < len; x++)
	{
		stringbufferAppendCh(buf, (char)packetNextByte(pkt));
	}
	
	sscanf(buf->m_buf, "%lf", &val);
	
	stringbufferDelete(buf);
	
	return val;
}

string* packetReadString(packet* pkt)
{
	int charsize;
	int numchars;
	stringbuffer* buf;
	string* str;
	int x;
	byte datatype = packetNextByte(pkt);
	
	if ( (byte)'S' != datatype )
	{
		pkt->m_isError = true;
		return NULL;
	}

	numchars = (packetNextByte(pkt) << 8) | (packetNextByte(pkt));
	charsize = (int)packetNextByte(pkt);

	if (NULL == (buf = stringbufferNew(80)))
	{
		pkt->m_isError = true;
		return 0;		
	}

	if ( charsize == 1 )
	{
		for ( x = 0; x < numchars; x++ )
		{
			stringbufferAppendCh(buf, (char)packetNextByte(pkt));
		}
	}
	else if ( charsize == 2 )
	{
		for ( x = 0; x < numchars; x++ )
		{
			stringbufferAppendCh(buf, (char)packetNextByte(pkt));
			packetNextByte(pkt);
		}
	}
	else
	{
		for ( x = 0; x < numchars; x++ )
		{
			stringbufferAppendCh(buf, (char)packetNextByte(pkt));
			packetNextByte(pkt);
			packetNextByte(pkt);
			packetNextByte(pkt);
		}
	}
	
	str = stringbufferToString(buf);
	stringbufferDelete(buf);
	
	return str;
}
