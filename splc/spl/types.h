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

#ifndef _types_h
#define _types_h

#if defined(_WIN32) || defined(_WIN64)
#include <spl/configwin32.h>
#else
#include <spl/config.h>
#endif

#if HAVE_TYPES_H
#include <types.h>
#endif
#if HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#if HAVE_STRING_H
#include <string.h>
#endif

#define _TANDEM		/* I've mixed __ and _ in the code */

#include <sys/types.h>
#include <wchar.h>

#ifdef __TANDEM
#include <tdmstd.h>
#include <tal.h>
#endif

#if !defined(LITTLE_ENDIAN) && !defined(BIG_ENDIAN)
/*#define BIG_ENDIAN 1*/
/* itanium is little end */
#define LITTLE_ENDIAN 1
#endif
	typedef signed long long int64;
	typedef unsigned long long uint64;
	/*typedef signed long long uint64;*/
	typedef signed int int32;
	typedef signed short int16;
	typedef char int8;
	typedef unsigned char byte;

	typedef unsigned int uint32;
	typedef unsigned short uint16;
	typedef unsigned char uint8;

	typedef float float32;
	typedef double float64;

#if !defined(_WIN32) && !defined(_WIN64)
	char *itoa( int i, char *buf, const int buflen );
	/*int iswspace( char ch );*/
#endif
	
#ifndef _tcp_TYPES
#define _tcp_TYPES
	typedef unsigned char  u_char;
	typedef unsigned short u_short;
	typedef unsigned int   u_int;
	typedef unsigned long  u_long;
	typedef unsigned short ushort;
#endif /* _tcp_TYPES */

#ifndef __cplusplus
#define false 0
#define true 1
typedef int bool;
#endif

#define	IL_MIN_INT32				((int32)0x80000000)
#define	IL_MAX_INT32				((int32)0x7FFFFFFF)
#define	IL_MAX_UINT32				((uint32)0xFFFFFFFFL)
#define	IL_MIN_INT64				((int64)0x8000000000000000LL)
#define	IL_MAX_INT64				((int64)0x7FFFFFFFFFFFFFFFLL)
#define	IL_MAX_UINT64				((int64)0x7FFFFFFFFFFFFFFFLL)

#define	READ_BYTE(buf, offset)	((int32)(((int8 *)(buf))[(offset)]))
#define	READ_BYTE_SHIFT(buf,offset,shift) (((uint32)(((uint8 *)(buf))[(offset)])) << (shift))

#endif
