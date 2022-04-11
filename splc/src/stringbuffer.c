#include <spl/stringbuffer.h>
#include <stdio.h>

static bool stringbufferExtend(stringbuffer* buf)
{
	int newlen;
	char *newbuf;

	ASSERT_MEM(buf->m_buf, buf->m_len);

	newlen = buf->m_len*2;
	newbuf = (char *)malloc(newlen);

	if (newbuf == NULL)
	{
		return false;
	}
	memcpy(newbuf, buf->m_buf, buf->m_len);
	buf->m_len = newlen;
	free(buf->m_buf);
	buf->m_buf = newbuf;
	
	ASSERT_MEM(buf->m_buf, buf->m_len);
	
	return true;
}

static stringbuffer* stringbufferInit(int size)
{
	stringbuffer* buf = (stringbuffer *)malloc(sizeof(stringbuffer));
	if (buf == NULL)
	{
		return NULL;
	}
	
	if (NULL == (buf->m_buf = (char *)malloc(size)))
	{
		free(buf);
		return NULL;
	}
	
	buf->m_len = size;
	buf->m_used = 0;
	buf->m_buf[0] = '\0';
	
	return buf;
}

stringbuffer* stringbufferNew()
{
	return stringbufferInit(32);
}

stringbuffer* stringbufferSizeNew(int size)
{
	return stringbufferInit(size);
}

void stringbufferDelete(stringbuffer *buf)
{
	free(buf->m_buf);
	free(buf);
}

void stringbufferClear(stringbuffer *buf)
{
	buf->m_used = 0;
	buf->m_buf[0] = '\0';
}

bool stringbufferAppendCh(stringbuffer* buf, const char cp)
{
	byte b[2];
	b[0] = (byte)cp;
	b[1] = '\0';
	return stringbufferAppendBuf(buf, b, 1);
}

bool stringbufferAppendEx(stringbuffer* buf, const char* cp, int start, int len)
{
	return stringbufferAppendBuf(buf, (const byte*)&cp[start], len);
}

bool stringbufferAppendBuf(stringbuffer* buf, const byte* cp, const int len)
{
	int x;
	
	/*ASSERT_MEM(buf->m_buf, buf->m_len);*/
	
	if (len == 0)
	{
		return true;
	}
	
	while (len + buf->m_used + 1 > buf->m_len)
	{
		if (! stringbufferExtend(buf))
		{
			return false;
		}
	}
	
	for (x = 0; x < len; x++)
	{
		buf->m_buf[buf->m_used + x] = cp[x];
	}
	
	buf->m_buf[buf->m_used + x] = '\0';
	buf->m_used += len;

	ASSERT(buf->m_buf[buf->m_used] == '\0');
	
	return true;
}

bool stringbufferAppend(stringbuffer *buf, const char *cp)
{
	return stringbufferAppendBuf(buf, (const byte *)cp, (int)strlen(cp));
}

string* stringbufferToString(stringbuffer *buf)
{
	return stringNew(buf->m_buf);
}

bool stringbufferAppendInt(stringbuffer* buf, const int i)
{
	char cbuf[32];
	sprintf(cbuf, "%d", i);
	return stringbufferAppend(buf, cbuf);
}

bool stringbufferAppendLong(stringbuffer* buf, const int64 i)
{
	char cbuf[64];
	sprintf(cbuf, "%lld", i);
	return stringbufferAppend(buf, cbuf);
}

bool stringbufferAppendReal(stringbuffer* buf, const double d)
{
	char cbuf[64];
	sprintf(cbuf, "%.15lf", d);
	return stringbufferAppend(buf, cbuf);
}

bool stringbufferAppendString(stringbuffer* buf, string* s)
{
	return stringbufferAppend(buf, s->m_cstr);
}

bool stringbufferAppendXmlEncode(stringbuffer* buf, const char* cp, bool isAttribute)
{
	if (! StrRequiresXmlEncoding(cp))
	{
		return stringbufferAppend(buf, cp);
	}
	
	while (*cp != '\0')
	{
		switch (*cp)
		{
			case '&':
				stringbufferAppend(buf, "&amp;");
				break;
			case '\'':
				stringbufferAppend(buf, "&#39;");
				break;
			case '<':
				stringbufferAppend(buf, "&lt;");
				break;
			case '>':
				stringbufferAppend(buf, "&gt;");
				break;
			case '"':
				stringbufferAppend(buf, "&quot;");
				break;
			case '\r':
				if (isAttribute)
				{
					stringbufferAppend(buf, "&#10;");
				}
				else
				{
					stringbufferAppendCh(buf, *cp);
				}
				break;
			case '\n':
				if (isAttribute)
				{
					stringbufferAppend(buf, "&#13;");
				}
				else
				{
					stringbufferAppendCh(buf, *cp);
				}
				break;
			default:
				stringbufferAppendCh(buf, *cp);
				break;
		}
		
		cp++;
	}
	
	return true;
}

#ifdef DEBUG
void stringbufferValidateMem(stringbuffer* buf)
{
	ASSERT_MEM(buf, sizeof(stringbuffer));
	ASSERT_MEM(buf->m_buf, buf->m_len);
}
#endif
