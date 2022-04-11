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
#ifndef _tcpsocket_h
#define _tcpsocket_h

#include <spl/types.h>
#include <spl/debug.h>
#include <spl/net/socket.h>

typedef struct _tcpsocket
{
	socketBase* m_sock;
	string* m_address;
} tcpsocket;

tcpsocket* tcpsocketNew(const char *addr, int port);
void tcpsocketDelete(tcpsocket *sock);

bool tcpsocketConnect(tcpsocket *sock);

#endif
