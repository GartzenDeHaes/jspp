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
#ifndef _packet_h
#define _packet_h

#include <spl/types.h>
#include <spl/debug.h>
#include <spl/net/socket.h>
#include <spl/stringbuffer.h>

typedef struct _packet
{
	stringbuffer* m_buf;
	int m_readpos;
	bool m_readRevByteOrder;
	bool m_isError;
} packet;

packet* packetNew(stringbuffer* buf);
void packetDelete(packet* pkt);

bool packetIsEof(packet* pkt);
bool packetIsError(packet* pkt);
bool packetIsEofOrError(packet* pkt);
bool packetIsValidNext(packet* pkt);
char packetNextType(packet* pkt);

int32 packetReadInt(packet* pkt);
int64 packetReadLong(packet* pkt);
double packetReadDouble(packet* pkt);
string* packetReadString(packet* pkt);

#endif
