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
#ifndef _string_buffer_h
#define _string_buffer_h

#include <spl/types.h>
#include <spl/debug.h>
#include <spl/string.h>

typedef struct _stringbuffer
{
	char *m_buf;
	int m_used;
	int m_len;	
} stringbuffer;

stringbuffer* stringbufferNew();
stringbuffer* stringbufferSizeNew(int size);
void stringbufferDelete(stringbuffer* buf);

void stringbufferClear(stringbuffer* buf);
bool stringbufferAppend(stringbuffer* buf, const char* cp);
bool stringbufferAppendXmlEncode(stringbuffer* buf, const char* cp, bool isAttribute);
bool stringbufferAppendCh(stringbuffer* buf, const char cp);
bool stringbufferAppendEx(stringbuffer* buf, const char* cp, int start, int len);
bool stringbufferAppendBuf(stringbuffer* buf, const byte* cp, const int len);
bool stringbufferAppendInt(stringbuffer* buf, const int i);
bool stringbufferAppendLong(stringbuffer* buf, const int64 i);
bool stringbufferAppendReal(stringbuffer* buf, const double d);
bool stringbufferAppendString(stringbuffer* buf, string* s);
string* stringbufferToString(stringbuffer *buf);

#ifdef DEBUG
void stringbufferValidateMem(stringbuffer* buf);
#else
#define stringbufferValidateMem(b)
#endif

#endif
