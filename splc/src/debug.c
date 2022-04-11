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

#include <spl/types.h>

#if defined(DEBUG) || defined(_DEBUG)

#include <stdio.h>
#include <stdlib.h>

void realfree(void *vp)
{
	free( vp );
}

#ifdef HAVE_SYS_ERRNO_H
#include <sys/errno.h>
#endif
#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif
#ifdef HAVE_ASSERT_H
#include <assert.h>
#endif
#ifdef HAVE_MEMORY_H
#include <memory.h>
#endif

#define _CrtDumpMemoryLeaks() false
#define _CrtCheckMemory() true
#define _CrtIsValidHeapPointer() true
#define _CrtIsValidPointer(a,b,c) true

#include <string.h>

#define DEBUG_MEM_FILL (char)0xCA

static volatile int mallocsize = 0;
static volatile int midnum = 0;
static volatile bool isshutdown = false;

struct allocblock
{
	int16 majic;
	int id;
	char checkbit;
	bool isarray;	/* new something[] reservse the first 4 bytes */
	int size;
	const char *filename;
	int lineno;
	struct allocblock *next;
	struct allocblock *prev;
	char checkbit2;
	char data[4];
};

struct memcheckpoint
{
	int count;
	int size;
};

#define ALLOCBLOC_MAJIC -31069

static struct allocblock m_abhead = { ALLOCBLOC_MAJIC, -1, 0, 0, 0, NULL, 0, NULL, NULL, DEBUG_MEM_FILL, DEBUG_MEM_FILL, DEBUG_MEM_FILL, DEBUG_MEM_FILL, DEBUG_MEM_FILL };
static struct allocblock m_abtail = { ALLOCBLOC_MAJIC, -1, 0, 0, 0, NULL, 0, NULL, NULL, DEBUG_MEM_FILL, DEBUG_MEM_FILL, DEBUG_MEM_FILL, DEBUG_MEM_FILL, DEBUG_MEM_FILL };

static struct memcheckpoint m_memcheckpoint;

#define validateBlock(a) (a != NULL && (a->checkbit2 == DEBUG_MEM_FILL &&	\
		a->majic == ALLOCBLOC_MAJIC &&										\
		a->data[a->size] == DEBUG_MEM_FILL &&								\
		a->data[a->size+1] == DEBUG_MEM_FILL &&								\
		a->data[a->size+2] == DEBUG_MEM_FILL &&								\
		a->data[a->size+3] == DEBUG_MEM_FILL &&								\
		(a->checkbit == 0 || a->checkbit == 1)))

static void _debugLockHeap()
{
}

static void _debugUnlockHeap()
{
}

void _debugFreeHeap()
{
	struct allocblock *bp = m_abhead.next;
	if ( NULL == bp )
	{
		return;
	}
	while ( &m_abtail != bp )
	{
		assert(bp->next->prev == bp);
		assert( validateBlock( bp ) );
		bp = bp->next;
		free( bp->prev );
	}

	assert(m_abhead.next == &m_abtail);
	assert(m_abhead.prev == NULL);
	assert(m_abtail.prev == &m_abhead);
	assert(m_abtail.next == NULL);
}

void _debugTearDown(bool reallyfree)
{
	struct allocblock *bp = m_abhead.next;

	while( bp != NULL && &m_abtail != bp )
	{
		struct allocblock *next;
		assert( validateBlock( bp ) );
		next = bp->next;
		if ( reallyfree )
		{
			free( bp );
		}
		bp = next;
	}

	if ( reallyfree )
	{
		assert(m_abhead.next == &m_abtail);
		assert(m_abhead.prev == NULL);
		assert(m_abtail.prev == &m_abhead);
		assert(m_abtail.next == NULL);
	}
	isshutdown = true;
}

void *_debugMalloc( const int len, const char *filename, const int lineno, const bool isarray  )
{
	struct allocblock *ab;

	assert( ! isshutdown && len > 0 );

	if ( NULL == m_abhead.next )
	{
		m_abhead.next = &m_abtail;
		m_abtail.prev = &m_abhead;
	}
	_debugLockHeap( );

	mallocsize += sizeof( struct allocblock ) + len;

	ab = (struct allocblock *)malloc( sizeof( struct allocblock ) + len );
	if( NULL == ab )
	{
		assert(false);
	}

	ab->majic = ALLOCBLOC_MAJIC;
	ab->id = midnum++;
	/*if (ab->id == 126)
	{
		printf("hi\n");
	}*/
	ab->data[len] = DEBUG_MEM_FILL;
	ab->data[len+1] = DEBUG_MEM_FILL;
	ab->data[len+2] = DEBUG_MEM_FILL;
	ab->data[len+3] = DEBUG_MEM_FILL;
	ab->filename = filename;
	ab->lineno = lineno;
	ab->size = len;
	ab->checkbit = 0;
	ab->checkbit2 = DEBUG_MEM_FILL;
	ab->isarray = isarray;
	ab->next = m_abhead.next;
	ab->next->prev = ab;
	ab->prev = &m_abhead;
	m_abhead.next = ab;

	assert( validateBlock( ab ) );

	_debugUnlockHeap( );
	return ab->data;
}

static struct allocblock *_debugScanForBlock( const void *vp )
{
	struct allocblock *bp;
	if ( 0 == vp || NULL == vp )
	{
		return NULL;
	}

	bp = (struct allocblock *)(((byte *)vp) - (sizeof(struct allocblock)));
	if ( ALLOCBLOC_MAJIC == bp->majic && vp == bp->data )
	{
		if(validateBlock(bp))
		{
			return bp;
		}
	}

	bp = (struct allocblock *)(((byte *)vp) - (sizeof(struct allocblock)-7));
	if ( ALLOCBLOC_MAJIC == bp->majic && vp == bp->data )
	{
		if(validateBlock(bp))
		{
			return bp;
		}
	}

	bp = (struct allocblock *)(((byte *)vp) - (sizeof(struct allocblock)-4));
	if ( ALLOCBLOC_MAJIC == bp->majic && vp == bp->data )
	{
		if(validateBlock(bp))
		{
			return bp;
		}
	}

	bp = (struct allocblock *)(((byte *)vp) - (sizeof(struct allocblock)-8));
	if ( ALLOCBLOC_MAJIC == bp->majic && vp == bp->data )
	{
		if(validateBlock(bp))
		{
			return bp;
		}
	}

	bp = m_abtail.prev;
	while ( bp != &m_abhead )
	{
		if ( vp == bp->data )
		{
			return bp;
		}
		if ( vp > (const void *)bp->data )
		{
			if (vp < (void *)(bp->data + bp->size))
			{
				return bp;
			}
		}
		bp = bp->prev;
	}

	return NULL;
}

struct allocblock *findBlock( const void *vp )
{
	struct allocblock *bp = _debugScanForBlock( vp );

	if ( bp == NULL )
	{
		bp = m_abhead.next;
		while ( NULL != bp )
		{
			if( bp->checkbit2 != DEBUG_MEM_FILL )
			{
				assert(bp->checkbit2 == DEBUG_MEM_FILL);
			}
			if ( vp >= (void *)bp->data && vp < (void *)(bp->data + bp->size) )
			{
				/* *offsetSize = bp->size - (int)((char *)vp - (char*)bp->data); */
				return bp;
			}
			bp = bp->next;
		}
	}
	else
	{
		return bp;
	}
	return NULL;
}

void _debugValidateHeap()
{
	struct allocblock *bp = m_abhead.next;

	_debugLockHeap(  );

	while ( NULL != bp && bp != &m_abtail )
	{
		assert 
		( 
			validateBlock( bp ) &&
			bp->next->prev == bp &&
			bp->prev->next == bp
		);
		bp = bp->next;
	}
	
	assert(_CrtCheckMemory());
	
	if ( isshutdown )
	{
		return;
	}
	_debugUnlockHeap(  );
}

void _debugFree( void *vp )
{
	struct allocblock *bp = NULL;

	if ( isshutdown )
	{
		/*free( (byte *)vp - sizeof(struct allocblock) );*/
		return;
	}

	assert(NULL != vp);

	_debugValidateHeap();
	_debugLockHeap(  );

	bp = _debugScanForBlock( vp );
	if ( NULL == bp )
	{
		if ( NULL != (bp = _debugScanForBlock( ((byte *)vp)-4 )) )
		{
			assert( _CrtIsValidPointer( ((byte *)vp)-4, 1, 1 ) );
		}
		else if ( NULL != (bp = _debugScanForBlock( ((byte *)vp)-8 )) )
		{
			assert( _CrtIsValidPointer( ((byte *)vp)-8, 1, 1 ) );
		}
		else
		{
			assert(false);
		}
	}
	else
	{
		assert( _CrtIsValidPointer( vp, 1, 1 ) );
	}
	
	assert( validateBlock(bp) );

	assert
	( 
		vp == bp->data  && 
		-1 == m_abhead.id && 
		0 == m_abhead.checkbit &&
		NULL != bp &&
		bp->prev->next == bp &&
		_CrtIsValidPointer( bp, sizeof(struct allocblock), 1 )
	);

	/*if ( bp->id == 2887 )
	{
		printf("hi\n");
	}*/

	bp->next->prev = bp->prev;
	bp->prev->next = bp->next;
	
	assert
	( 
		validateBlock( bp ) && 
		bp->prev != NULL &&
		((&m_abhead == bp->prev)?1:validateBlock( bp->prev )) &&
		validateBlock( bp->next ) && 
		&m_abhead != bp
	);

	memset( bp, DEBUG_MEM_FILL, sizeof(struct allocblock) + bp->size );

	assert( _CrtIsValidPointer( bp, sizeof(struct allocblock), 1 ) );

	mallocsize -= bp->size;

	_debugUnlockHeap(  );
	_debugValidateHeap();
	free( bp );
	_debugValidateHeap();
}

void _debugAssert( const int cond, const char *filename, const int lineno )
{
	if ( cond == 0 )
	{
#ifndef DEBUG
		assert( cond );
#else
		printf( "assertion failed in %s on line %d.\n", filename, lineno );
		exit(20);
#endif
	}
}

int _debugCheckPtr( const void *ptr )
{
	struct allocblock *bp;
	bool ret;
	if ( isshutdown )
	{
		return 1;
	}
	
	_debugLockHeap(  );

	if ( NULL == (bp = findBlock( ptr )) )
	{
		if ( NULL != (bp = findBlock( ((byte *)ptr)-4 )) )
		{
			if ( ! bp->isarray )
			{
				_debugUnlockHeap(  );
				/*printf("pointer found, but should be array for padding?\n");*/
				return 0;
			}
		}
		else if ( NULL != (bp = findBlock( ((byte *)ptr)-8 )) )
		{
			/* Also happens on sub classes with abstract (virtual) base class */
			/*if ( ! bp->isarray )
			{
				debugUnlockHeap(  );
				printf("pointer found, but should be array for padding?\n");
				return 0;
			}*/
		}
		else
		{
			_debugUnlockHeap(  );
			/*printf("pointer not found.\n");*/
			return 0;
		}
		assert( _CrtIsValidPointer( ((byte *)ptr)-4, 1, 1 ) );
	}
	else
	{
		assert( _CrtIsValidPointer( ptr, 1, 1 ) );
	}
	ret = validateBlock( bp );
	_debugUnlockHeap(  );
	return ret;
}

int _debugCheckBlock( const void *ptr, const int size )
{
	struct allocblock *bp;
	if ( isshutdown )
	{
		return 1;
	}
	
	if (NULL == ptr)
	{
		assert( NULL != ptr );
	}

	_debugLockHeap(  );

	bp = _debugScanForBlock( ptr );
	if( bp == NULL )
	{
		const int vpsize = sizeof(void *);
		
		/* check for compiler padding, typical for arrays */
		if ( NULL == (bp = _debugScanForBlock( ((byte *)ptr)-vpsize ) ) ) 
		{
			_debugUnlockHeap(  );
			return 0;
		}
		/* This also happens for abstract base classes w/virtual members */
		/*if ( ! bp->isarray )
		{
			debugUnlockHeap(  );
			return 0;
		}*/

		assert( _CrtIsValidPointer( ((byte *)ptr)-vpsize, 1, 1 ) );
	}
	else
	{
		assert( _CrtIsValidPointer( ptr, 1, 1 ) );
	}
	if ( NULL == bp )
	{
		_debugUnlockHeap(  );
		return 0;
	}
	if ( bp->size != size || bp->data != ptr )
	{
		if ( bp->size < size )
		{
			_debugUnlockHeap(  );
			return 0;
		}
	}

	assert( validateBlock( bp ));
	_debugUnlockHeap(  );
	return bp->size >= size;
}

void _debugClearMemCheckPoints()
{
	struct allocblock *bp;

	if ( isshutdown )
	{
		return;
	}

	_debugLockHeap(  );

	bp = m_abhead.next;
	while ( NULL != bp )
	{
		assert( validateBlock( bp ) );
		if ( bp != NULL )
		{
			bp->checkbit = 0;
		}
		bp = bp->next;
	}

	_debugUnlockHeap(  );
}

void _debugNoteMemBlock( const void *vp )
{
	struct allocblock *bp;
	
	if ( isshutdown )
	{
		return;
	}
	
	_debugLockHeap(  );

	assert( _CrtIsValidPointer( vp, 1, 1 ) );

	bp = findBlock( vp );
	
	if ( NULL == bp )
	{
		if ( NULL != (bp = findBlock( ((byte *)vp)-4 )) )
		{
			assert ( bp->isarray );
		}
		else if ( NULL != (bp = findBlock( ((byte *)vp)-8 )) )
		{
			assert ( bp->isarray );
		}
	}	
	
	assert(NULL != bp && validateBlock( bp ) && ( vp == bp->data || ((char *)vp - 4) == bp->data || ((char *)vp - 8) == bp->data ) );
	bp->checkbit = 1;
	_debugUnlockHeap(  );
}

int _debugCheckMem()
{
	struct allocblock *bp;

	assert(_CrtCheckMemory());
	
	if ( isshutdown )
	{
		return 1;
	}

	_debugLockHeap(  );

	bp = m_abhead.next;
	while ( NULL != bp && &m_abtail != bp )
	{
		if ( bp != NULL )
		{
			assert(validateBlock( bp ));
			if( bp->checkbit != 1 )
			{
				return 0;
			}
		}
		bp = bp->next;
	}
	
	_debugUnlockHeap(  );
	return 1;
}

void _debugDumpMemLeaks()
{
	struct allocblock *bp = m_abhead.next;
	if ( isshutdown || NULL == bp )
	{
		return;
	}	
	_debugLockHeap(  );

	while ( bp != &m_abtail )
	{
		assert(validateBlock( bp ));
		if( bp->checkbit != 1 )
		{
			printf( "Memory leak %d: %d bytes allocated in %s on line %d\n", bp->id, bp->size, bp->filename, bp->lineno );
		}
		bp = bp->next;
	}
	_debugUnlockHeap(  );
}

int _debugAssertMemFree()
{
	return m_abhead.next == &m_abtail;
}

void fillCheckPointData( struct memcheckpoint *mp )
{
	struct allocblock *bp;

	mp->count = 0;
	mp->size = 0;

	_debugLockHeap(  );
	bp = m_abhead.next;

	while( NULL != bp && &m_abtail != bp )
	{
		if( bp != NULL )
		{
			mp->count++;
			mp->size += bp->size;
		}
		bp = bp->next;
	}
	_debugUnlockHeap(  );
}

void _debugCheckpointHeap()
{
	fillCheckPointData( &m_memcheckpoint );
}

int _debugAssertCheckPoint()
{
	struct memcheckpoint mp;
	fillCheckPointData( &mp );
	return mp.size == m_memcheckpoint.size && mp.count == m_memcheckpoint.count;
}

#ifdef DEBUG
void *rpl_malloc( const int size, char *filename, const int lineno, const bool isarray )
{
	return _debugMalloc(size, filename, lineno, isarray);
}
#else
void *rpl_malloc( const int size )
{
	return malloc( size );
}
#endif

#endif
