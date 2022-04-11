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
#ifndef _debug_h
#define _debug_h

#include <spl/types.h>
#include <stdlib.h>
#include <assert.h>

#if defined _DEBUG || defined DEBUG

#ifndef DEBUG
#define DEBUG
#endif

#ifndef _DEBUG
#define _DEBUG
#endif

#undef malloc
#undef free

#ifndef NULL
#define NULL ((void *)0)
#endif

void _unitAssert( const char *msg, const int cond, const char *filename, const int lineno );
void _unitTest( const char *msg, const int cond);

void *_debugMalloc( const int, const char *filename, const int lineno, const bool isarray );
void *_rpl_malloc( const int size, char *filename, const int lineno, const bool isarray );
void _debugFree( void * );
void _debugAssert( const int cond, const char* filename, const int lineno );
int _debugCheckPtr( const void *ptr );
int _debugCheckBlock( const void *ptr, const int size );
void _debugClearMemCheckPoints();
void _debugNoteMemBlock( const void *vp );
int _debugCheckMem();
int _debugAssertMemFree();
void _debugCheckpointHeap();
int _debugAssertCheckPoint();
void _debugTearDown( bool reallyfree );
void _debugDumpMemLeaks();
void _debugValidateHeap();
/*void _debugLockHeap();
void _debugUnlockHeap();*/
void _debugFreeHeap();
void _debugEnableHeapLocking();

#define malloc(size) _debugMalloc(size, __FILE__, __LINE__, false)
#define free(ptr) _debugFree(ptr)
void realfree(void *vp);

#undef ASSERT
#define ASSERT(cond) _debugAssert( cond, __FILE__, __LINE__ )
#define ASSERT_PTR(ptr) ASSERT(_debugCheckPtr( ptr ))
#define ASSERT_MEM(ptr, size) ASSERT(_debugCheckBlock( ptr, size ))
#define ASSERT_MEM_FREE() ASSERT(_debugAssertMemFree())

#define DEBUG_CLEAR_MEM_CHECK_POINTS() _debugClearMemCheckPoints()
#define DEBUG_NOTE_MEM_ALLOCATION(a) _debugNoteMemBlock( a )
#define DEBUG_NOTE_MEM(a) _debugNoteMemBlock( a )
#define DEBUG_ASSERT_MEM_NOTED() ASSERT(_debugCheckMem())

#define DEBUG_CHECK_POINT_HEAP() _debugCheckpointHeap()
#define DEBUG_ASSERT_CHECK_POINT() ASSERT(_debugAssertCheckPoint())
#define DEBUG_TEAR_DOWN(a) _debugTearDown(a)
#define DEBUG_DUMP_MEM_LEAKS() _debugDumpMemLeaks()
/*#define DEBUG_LOCK_HEAP() _debugLockHeap()
#define DEBUG_UNLOCK_HEAP() _debugUnlockHeap()*/
#define DEBUG_FREE_HEAP() _debugFreeHeap()
#define DEBUG_ENABLE_HEAP_LOCK() _debugEnableHeapLocking()

#define UNIT_TEST(msg,cond) _unitTest(msg,cond)
#define UNIT_ASSERT(msg,cond) _unitAssert( msg, cond, __FILE__, __LINE__ )
#define UNIT_ASSERT_PTR(msg,ptr) UNIT_ASSERT(msg,_debugCheckPtr( ptr ))
#define UNIT_ASSERT_MEM(msg,ptr,size) UNIT_ASSERT(msg,_debugCheckBlock( ptr, size ))
#define UNIT_ASSERT_MEM_FREE(msg) UNIT_ASSERT(msg,_debugAssertMemFree())
#define UNIT_ASSERT_MEM_NOTED(msg) UNIT_ASSERT(msg,_debugCheckMem())
#define UNIT_ASSERT_CHECK_POINT(msg) UNIT_ASSERT(msg,_debugAssertCheckPoint())

#else

void *rpl_malloc( const int size );

#ifndef ASSERT
#define ASSERT(cond)
#endif
#define ASSERT_PTR(ptr) 
#define ASSERT_MEM(ptr, size) 
#define ASSERT_MEM_FREE()

#define DEBUG_CLEAR_MEM_CHECK_POINTS() 
#define DEBUG_NOTE_MEM_ALLOCATION(a)
#define DEBUG_NOTE_MEM(a)
#define DEBUG_ASSERT_MEM_NOTED() 

#define DEBUG_CHECK_POINT_HEAP()
#define DEBUG_ASSERT_CHECK_POINT()
#define DEBUG_TEAR_DOWN(a)
#define DEBUG_DUMP_MEM_LEAKS()

#define UNIT_TEST(cond,msg)
#define UNIT_ASSERT(msg,cond)
#define UNIT_ASSERT_PTR(msg,ptr) 
#define UNIT_ASSERT_MEM(msg,ptr,size) 
#define UNIT_ASSERT_MEM_FREE(msg) 
#define UNIT_ASSERT_MEM_NOTED(msg) 
#define UNIT_ASSERT_CHECK_POINT(msg)
#define DEBUG_ENABLE_HEAP_LOCK()

#define DEBUG_VALIDATE()
/*#define DEBUG_LOCK_HEAP()
#define DEBUG_UNLOCK_HEAP()*/
#define DEBUG_FREE_HEAP()

#ifdef DEBUG2
#define DEBUG_VALIDATE() _debugValidateHeap()
#define DEBUG_PTR2(ptr) ASSERT(_debugCheckPtr( ptr ))
#define ASSERT_MEM2(ptr, size) ASSERT(_debugCheckBlock( ptr, size ))
#else
#define DEBUG_VALIDATE()
#define DEBUG_PTR2(ptr)
#define ASSERT_MEM2(ptr, size)
#endif

#define realfree(a) free(a)

#endif
#endif
