#ifdef HAVE_CONFIG_H
#include <jspp/config.h>
#endif
#ifdef _WIN32
#include <spl/configwin32.h>
#endif
#ifdef CONSOLE
#include <stdio.h>
#endif
#include <stdlib.h>
#include <math.h>
#ifdef HAVE_TIME_H
#include <time.h>
#endif
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#include <spl/debug.h>
#include <jspp/intrinsics.h>
#include <spl/cstring.h>
#include <spl/strbuf.h>
#include <spl/rand.h>

/*
 *  object
 */

enum InterpErrors object_parseInt
(
	qinterp *qip, 
	const void *mp, 
	variant *ret
)
{
	variant *vararg = getArgumentN( qip, 0 );
	
	varValidateMem( ret );
	varValidateMem( vararg );
	
	if ( ! varAssign( ret, vararg ) )
	{
		return IERR_OUT_OF_MEM;
	}
	if (! varCast( ret, TYPE_INT ) )
	{
		return IERR_TYPE_MISMATCH;
	}
	return IERR_NONE;
}

enum InterpErrors object_parseFloat
(
	qinterp *qip, 
	const void *mp, 
	variant *ret
)
{
	variant *vararg = getArgumentN( qip, 0 );
	
	varValidateMem( ret );
	varValidateMem( vararg );
	
	if ( ! varAssign( ret, vararg ) )
	{
		return IERR_OUT_OF_MEM;
	}
	if (! varCast( ret, TYPE_FLOAT ) )
	{
		return IERR_TYPE_MISMATCH;
	}
	return IERR_NONE;
}

enum InterpErrors object_toString
(
	qinterp *qip, 
	const void *mp, 
	variant *ret
)
{
	variant *vararg = getArgumentN( qip, 0 );
	
	varValidateMem( ret );
	varValidateMem( vararg );
	
	if ( ! varAssign( ret, vararg ) )
	{
		return IERR_OUT_OF_MEM;
	}
	if (! varCast( ret, TYPE_STRING ) )
	{
		return IERR_TYPE_MISMATCH;
	}
	return IERR_NONE;
}

enum InterpErrors object_toStringI
(
	qinterp *qip, 
	const void *mp, 
	variant *ret
)
{
	variant *varthis = getArgumentN( qip, 0 );
	
	varValidateMem( ret );
	varValidateMem( varthis );
	
	if ( ! varAssign( ret, varthis ) )
	{
		return IERR_OUT_OF_MEM;
	}
	if (! varCast( ret, TYPE_STRING ) )
	{
		return IERR_TYPE_MISMATCH;
	}
	return IERR_NONE;
}

enum InterpErrors object_typename(qinterp *qip, const void *mp, variant *ret)
{
	variant *vararg = getArgumentN( qip, 0 );
	
	varValidateMem( ret );
	varValidateMem( vararg );

	varAssignString( ret, varTypeName( vararg ) );

	return IERR_NONE;
}

enum InterpErrors object_length(qinterp *qip, const void *mp, variant *ret)
{
	variant *vararg = getArgumentN( qip, 0 );
	
	varValidateMem( ret );
	varValidateMem( vararg );

	if ( TYPE_STRING == vararg->type )
	{
		varAssignInt( ret, vararg->data.sp->len );
	}
	else
	{
		varAssignInt( ret, 4 );
	}

	return IERR_NONE;
}

/*
 *  array
 */

struct arraydata
{
	int dims;
	vector *array;
	int dimlens[ARRAY_MAX_DIMS];
};

void array_finializer( void *vadp )
{
	struct arraydata *adp = (struct arraydata *)vadp;
	int x, count;

	ASSERT_MEM( vadp, sizeof( struct arraydata ) );
	vectorValidateMem( adp->array );

	count = vectorCount( adp->array );
	for ( x = 0; x < count; x++ )
	{
		variant *var = (variant *)vectorElementAt( adp->array, x );
		if ( NULL != var )
		{
			varValidateMem( var );
			varDestruct( var );
		}
	}
	vectorDestruct( adp->array );

	free( adp );
}

void array_checkmem( void *vadp )
{
	struct arraydata *adp = (struct arraydata *)vadp;
	int x, count;

	DEBUG_NOTE_MEM( vadp );
	vectorCheckMem( adp->array );

	count = vectorCount( adp->array );
	for ( x = 0; x < count; x++ )
	{
		variant *var = (variant *)vectorElementAt( adp->array, x );
		if ( NULL != var )
		{
			varCheckMem( var );
		}
	}
}

enum InterpErrors arrayHelperConstruct(jsinst *inst, int dims, ...)
{
	va_list marker;
	struct arraydata *adp;
	int x, elements = 1;
	long zero = 0;

	ASSERT( dims > 0 );

	inst->nativedata = malloc( sizeof(struct nativedataHolder) );
	if ( NULL == inst->nativedata )
	{
		return IERR_OUT_OF_MEM;
	}
	inst->nativedata->data = adp = malloc( sizeof(struct arraydata) );
	if ( NULL == adp )
	{
		return IERR_OUT_OF_MEM;
	}
	inst->nativedata->finializer = array_finializer;
	inst->nativedata->memcheck = array_checkmem;

	adp->dims = dims;

	adp->array = vectorConstruct( sizeof(variant *) );
	if ( NULL == adp->array )
	{
		free( adp );
		free( inst->nativedata );
		inst->nativedata = NULL;
		return IERR_OUT_OF_MEM;
	}
	va_start( marker, dims );
	for ( x = 1; x <= dims; x++ )
	{
		int len = va_arg(marker, int);

		if ( 0 > len )
		{
			va_end( marker );
			return IERR_INDEX_OUT_OF_BOUNDS;
		}
		adp->dimlens[x-1] = len;
		elements *= len;
	}
	va_end( marker );
	if ( ! vectorSetElementAt( adp->array, &zero, elements-1 ) )
	{
		vectorDestruct( adp->array );
		free( adp );
		free( inst->nativedata );
		inst->nativedata = NULL;
		return IERR_OUT_OF_MEM;
	}
	inst->magic = ARRAY_MAGIC;
	return IERR_NONE;
}

enum InterpErrors array_arrayX
(
	qinterp *qip, 
	const void *vmp,	// not used
	variant *ret
)
{
	variant *varthis = getArgumentN( qip, 0 );
	int dims = getArgumentCount( qip )-1;
	jsinst *inst = varthis->data.ip;
	struct arraydata *adp;
	int x, elements = 1;
	long zero = 0;

	ASSERT_MEM( vmp, sizeof(struct jsmethod) );
	varValidateMem( varthis );
	varValidateMem( ret );

	ASSERT( TYPE_INST == varthis->type );
	ASSERT( dims > 0 );

	inst->nativedata = malloc( sizeof(struct nativedataHolder) );
	if ( NULL == inst->nativedata )
	{
		return IERR_OUT_OF_MEM;
	}
	inst->nativedata->data = adp = malloc( sizeof(struct arraydata) );
	if ( NULL == adp )
	{
		return IERR_OUT_OF_MEM;
	}
	inst->nativedata->finializer = array_finializer;

	adp->dims = dims;

	adp->array = vectorConstruct( sizeof(variant *) );
	if ( NULL == adp->array )
	{
		free( adp );
		free( inst->nativedata );
		inst->nativedata = NULL;
		return IERR_OUT_OF_MEM;
	}
	for ( x = 1; x <= dims; x++ )
	{
		variant *varlen = getArgumentN( qip, x );
		int len;

		varValidateMem( varlen );
		if ( ! varGetInt( varlen, &len ) )
		{
			return IERR_TYPE_MISMATCH;
		}
		if ( 0 > len )
		{
			return IERR_INDEX_OUT_OF_BOUNDS;
		}
		adp->dimlens[x-1] = len;
		elements *= len;
	}
	if ( ! vectorSetElementAt( adp->array, &zero, elements-1 ) )
	{
		vectorDestruct( adp->array );
		free( adp );
		free( inst->nativedata );
		inst->nativedata = NULL;
		return IERR_OUT_OF_MEM;
	}
	inst->magic = ARRAY_MAGIC;
	return IERR_NONE;
}

enum InterpErrors arrayHelperSet(jsinst *array, variant *data, int dimcount, ...)
{
	va_list marker;
	int x, element = 0;
	struct arraydata *adp = array->nativedata->data;
	int *indexes;
	variant *vardata;

	ASSERT_MEM( adp, sizeof(struct arraydata) );
	vectorValidateMem( adp->array );

	indexes = (int *)malloc( (int)sizeof(int) * dimcount );
	if ( NULL == indexes )
	{
		return IERR_OUT_OF_MEM;
	}
	if ( dimcount != adp->dims )
	{
		free( indexes );
		return IERR_WRONG_ARRAY_INDEXS;
	}
	va_start( marker, dimcount );
	for ( x = 1; x <= dimcount; x++ )
	{
		int idx = va_arg(marker, int);
		indexes[x-1] = idx;
	}
	va_end( marker );
	for ( x = 0; x < dimcount; x++ )
	{
		element += indexes[x] * adp->dimlens[x];
	}
	free( indexes );
	vardata = (variant *)vectorElementAt( adp->array, element );
	if ( NULL == vardata )
	{
		vardata = varConstruct();
		if ( NULL == vardata )
		{
			return IERR_OUT_OF_MEM;
		}
		varCast( vardata, TYPE_NULL );
		vectorSetElementAt( adp->array, &vardata, element );
	}
	else
	{
		varValidateMem( vardata );
	}
	varAssign( vardata, data );
	return IERR_NONE;
}

enum InterpErrors array_itemX(qinterp *qip, const void *vmp, variant *ret)
{
	variant *varthis = getArgumentN( qip, 0 );
	int dims = getArgumentCount( qip ) - 1;
	int *indexes;
	variant *vardata;
	struct arraydata *adp;
	int x, element = 0;

	indexes = (int *)malloc( (int)sizeof(int) * dims );
	if ( NULL == indexes )
	{
		return IERR_OUT_OF_MEM;
	}
	ASSERT_MEM( vmp, sizeof(struct jsmethod) );
	varValidateMem( varthis );
	varValidateMem( ret );

	if ( TYPE_INST != varthis->type )
	{
		free( indexes );
		return IERR_TYPE_MISMATCH;
	}
	if ( ARRAY_MAGIC != varthis->data.ip->magic )
	{
		free( indexes );
		return IERR_TYPE_MISMATCH;
	}
	adp = varthis->data.ip->nativedata->data;
	ASSERT_MEM( adp, sizeof(struct arraydata) );
	vectorValidateMem( adp->array );

	if ( dims != adp->dims )
	{
		free( indexes );
		return IERR_WRONG_ARRAY_INDEXS;
	}
	for ( x = 1; x <= dims; x++ )
	{
		variant *varidx = getArgumentN( qip, x );
		int idx;

		varValidateMem( varidx );
		if ( ! varGetInt( varidx, &idx ) )
		{
			free( indexes );
			return IERR_TYPE_MISMATCH;
		}
		indexes[x-1] = idx;
	}
	for ( x = 0; x < dims; x++ )
	{
		element += indexes[x] * adp->dimlens[x];
	}
	free( indexes );
	vardata = (variant *)vectorElementAt( adp->array, element );
	if ( NULL == vardata )
	{
		vardata = varConstruct();
		if ( NULL == vardata )
		{
			return IERR_OUT_OF_MEM;
		}
		varCast( vardata, TYPE_NULL );
		vectorSetElementAt( adp->array, &vardata, element );
		varRef( ret, vardata );
	}
	else
	{
		varValidateMem( vardata );
		varRef( ret, vardata );
	}
	return IERR_NONE;
}

enum InterpErrors array_length
(
	qinterp *qip, 
	const void *vmp, 
	variant *ret
)
{
	variant *varthis = getArgumentN( qip, 0 );
	struct arraydata *adp;
	int dim = 0;

	ASSERT_MEM( vmp, sizeof(struct jsmethod) );	
	varValidateMem( ret );
	varValidateMem( varthis );
	
	if ( TYPE_INST != varthis->type )
	{
		return IERR_TYPE_MISMATCH;
	}
	if ( ARRAY_MAGIC != varthis->data.ip->magic )
	{
		return IERR_TYPE_MISMATCH;
	}
	adp = varthis->data.ip->nativedata->data;
	ASSERT_MEM( adp, sizeof(struct arraydata) );
	vectorValidateMem( adp->array );

	if ( getArgumentCount( qip ) == 2 )
	{
		variant *vardim = getArgumentN( qip, 1 );
		varValidateMem( vardim );
		if ( ! varGetInt( vardim, &dim ) )
		{
			return IERR_TYPE_MISMATCH;
		}
	}
	else
	{
		ASSERT( getArgumentCount( qip ) == 1 );
	}
	if ( dim >= adp->dims )
	{
		return IERR_INDEX_OUT_OF_BOUNDS;
	}
	varClear( ret );
	ret->type = TYPE_INT;
	ret->data.i = adp->dimlens[dim];

	return IERR_NONE;
}

enum InterpErrors array_dims
(
	qinterp *qip, 
	const void *vmp, 
	variant *ret
)
{
	variant *varthis = getArgumentN( qip, 0 );
	struct arraydata *adp;

	ASSERT_MEM( vmp, sizeof(struct jsmethod) );	
	varValidateMem( ret );
	varValidateMem( varthis );
	
	if ( TYPE_INST != varthis->type )
	{
		return IERR_TYPE_MISMATCH;
	}
	if ( ARRAY_MAGIC != varthis->data.ip->magic )
	{
		return IERR_TYPE_MISMATCH;
	}
	adp = varthis->data.ip->nativedata->data;
	ASSERT_MEM( adp, sizeof(struct arraydata) );
	vectorValidateMem( adp->array );

	varClear( ret );
	ret->type = TYPE_INT;
	ret->data.i = adp->dims;

	return IERR_NONE;
}

/*
 *  vector
 */

void vector_finializer( void *vadp )
{
	vector *vdp = (vector *)vadp;
	int x, count;

	vectorValidateMem( vdp );

	count = vectorCount( vdp );
	for ( x = 0; x < count; x++ )
	{
		variant *var = (variant *)vectorElementAt( vdp, x );
		if ( NULL != var )
		{
			varValidateMem( var );
			varDestruct( var );
		}
	}
	vectorDestruct( vdp );
}

void vector_checkmem( void *vadp )
{
	vector *vdp = (vector *)vadp;
	int x, count;

	DEBUG_NOTE_MEM( vadp );
	vectorCheckMem( vdp );

	count = vectorCount( vdp );
	for ( x = 0; x < count; x++ )
	{
		variant *var = (variant *)vectorElementAt( vdp, x );
		if ( NULL != var )
		{
			varCheckMem( var );
		}
	}
}

enum InterpErrors vector_vector(qinterp *qip, const void *mp, variant *ret)
{
	variant *varthis = getArgumentN( qip, 0 );
	jsinst *inst = varthis->data.ip;
	vector *vdp;

	ASSERT ( getArgumentCount( qip ) == 1 );

	ASSERT_MEM( mp, sizeof(struct jsmethod) );
	varValidateMem( varthis );
	varValidateMem( ret );

	ASSERT( TYPE_INST == varthis->type );

	inst->nativedata = malloc( sizeof(struct nativedataHolder) );
	if ( NULL == inst->nativedata )
	{
		return IERR_OUT_OF_MEM;
	}
	inst->nativedata->data = vdp = vectorConstruct( sizeof(variant *) );
	if ( NULL == vdp )
	{
		free( inst->nativedata );
		inst->nativedata = NULL;
		return IERR_OUT_OF_MEM;
	}
	inst->nativedata->finializer = vector_finializer;
	inst->nativedata->memcheck = vector_checkmem;

	inst->magic = VECTOR_MAGIC;
	return IERR_NONE;
}

enum InterpErrors vector_add(qinterp *qip, const void *mp, variant *ret)
{
	variant *varthis = getArgumentN( qip, 0 );
	variant *varitem = getArgumentN( qip, 1 );
	variant *newvar;
	jsinst *inst = varthis->data.ip;
	vector *vdp;

	ASSERT( TYPE_INST == varthis->type );
	qinterpValidateMem( qip );
	ASSERT_MEM( mp, sizeof(struct jsmethod) );
	varValidateMem( varthis );
	varValidateMem( varitem );
	varValidateMem( ret );
	jsinstValidateMem( inst );

	vdp = (vector *)inst->nativedata->data;
	vectorValidateMem( vdp );

	newvar = varConstructDup( varitem );
	if ( NULL == newvar )
	{
		return IERR_OUT_OF_MEM;
	}
	if ( ! vectorAdd( vdp, &newvar ) )
	{
		return IERR_OUT_OF_MEM;
	}
	varClear( ret );

	return IERR_NONE;
}

enum InterpErrors vector_size(qinterp *qip, const void *mp, variant *ret)
{
	variant *varthis = getArgumentN( qip, 0 );
	jsinst *inst = varthis->data.ip;
	vector *vdp;

	ASSERT( TYPE_INST == varthis->type );
	qinterpValidateMem( qip );
	ASSERT_MEM( mp, sizeof(struct jsmethod) );
	varValidateMem( varthis );
	varValidateMem( ret );
	jsinstValidateMem( inst );

	vdp = (vector *)inst->nativedata->data;
	vectorValidateMem( vdp );

	varAssignInt( ret, vectorCount( vdp ) );

	return IERR_NONE;
}

enum InterpErrors vector_elementat(qinterp *qip, const void *mp, variant *ret)
{
	variant *varthis = getArgumentN( qip, 0 );
	variant *varidx = getArgumentN( qip, 1 );
	variant *item;
	jsinst *inst = varthis->data.ip;
	vector *vdp;
	int idx;

	ASSERT( TYPE_INST == varthis->type );
	qinterpValidateMem( qip );
	ASSERT_MEM( mp, sizeof(struct jsmethod) );
	varValidateMem( varthis );
	varValidateMem( ret );
	jsinstValidateMem( inst );

	vdp = (vector *)inst->nativedata->data;
	vectorValidateMem( vdp );

	if ( ! varGetInt( varidx, &idx ) )
	{
		return IERR_TYPE_MISMATCH;
	}
	if ( (item = (variant *)vectorElementAt( vdp, idx )) == NULL )
	{
		return IERR_INDEX_OUT_OF_BOUNDS;
	}
	varRef( ret, item );
	return IERR_NONE;
}

enum InterpErrors vector_setelementat(qinterp *qip, const void *mp, variant *ret)
{
	variant *varthis = getArgumentN( qip, 0 );
	variant *varidx = getArgumentN( qip, 1 );
	variant *varitem = getArgumentN( qip, 2 );
	variant *newvar, *oldvar;
	jsinst *inst = varthis->data.ip;
	vector *vdp;
	int idx;

	ASSERT( TYPE_INST == varthis->type );
	qinterpValidateMem( qip );
	ASSERT_MEM( mp, sizeof(struct jsmethod) );
	varValidateMem( varthis );
	varValidateMem( varitem );
	varValidateMem( ret );
	jsinstValidateMem( inst );

	vdp = (vector *)inst->nativedata->data;
	vectorValidateMem( vdp );

	if ( ! varGetInt( varidx, &idx ) )
	{
		return IERR_TYPE_MISMATCH;
	}
	newvar = varConstructDup( varitem );
	if ( NULL == newvar )
	{
		return IERR_OUT_OF_MEM;
	}
	oldvar = (variant *)vectorElementAt( vdp, idx );
	varValidateMem( oldvar );
	varDestruct( oldvar );
	if ( ! vectorSetElementAt( vdp, &newvar, idx ) )
	{
		/* shouldn't happen */
		return IERR_OUT_OF_MEM;
	}
	varClear( ret );
	return IERR_NONE;
}

enum InterpErrors vector_clear(qinterp *qip, const void *mp, variant *ret)
{
	variant *varthis = getArgumentN( qip, 0 );
	jsinst *inst = varthis->data.ip;
	vector *vdp;
	int x, count;

	ASSERT( TYPE_INST == varthis->type );
	qinterpValidateMem( qip );
	ASSERT_MEM( mp, sizeof(struct jsmethod) );
	varValidateMem( varthis );
	varValidateMem( ret );
	jsinstValidateMem( inst );

	vdp = (vector *)inst->nativedata->data;
	vectorValidateMem( vdp );

	count = vectorCount( vdp );
	for ( x = 0; x < count; x++ )
	{
		variant *var = (variant *)vectorElementAt( vdp, x );
		varValidateMem( var );
		varDestruct( var );
	}
	vectorClear( vdp );
	varClear( ret );

	return IERR_NONE;
}

#ifdef CONSOLE

enum InterpErrors console_writeln(qinterp *qip, const void *mp, variant *ret)
{
	variant *vararg = getArgumentN( qip, 0 );
	string *str;

	qinterpValidateMem( qip );
	ASSERT( getArgumentCount( qip ) == 1 );
	ASSERT_MEM( mp, sizeof(struct jsmethod) );
	varValidateMem( vararg );
	varValidateMem( ret );

	str = varToString( vararg );
	if ( NULL == str )
	{
		return IERR_OUT_OF_MEM;
	}
	stringValidateMem( str );
	printf( "%s\n", str->cstr );
	stringDestruct( str );

	return IERR_NONE;
}

enum InterpErrors console_write(qinterp *qip, const void *mp, variant *ret)
{
	variant *vararg = getArgumentN( qip, 0 );
	string *str;

	qinterpValidateMem( qip );
	ASSERT( getArgumentCount( qip ) == 1 );
	ASSERT_MEM( mp, sizeof(struct jsmethod) );
	varValidateMem( vararg );
	varValidateMem( ret );

	str = varToString( vararg );
	if ( NULL == str )
	{
		return IERR_OUT_OF_MEM;
	}
	stringValidateMem( str );
	printf( "%s", str->cstr );
	stringDestruct( str );

	return IERR_NONE;
}

enum InterpErrors console_readln(qinterp *qip, const void *mp, variant *ret)
{
	char buf[255];

	ASSERT( getArgumentCount( qip ) == 0 );
	qinterpValidateMem( qip );
	ASSERT_MEM( mp, sizeof(struct jsmethod) );
	varValidateMem( ret );

	gets(buf);
	varAssignChar( ret, buf );

	return IERR_NONE;
}

#endif /*CONSOLE*/

/*
 *  StringBuffer
 */
void stringbuffer_finializer( void *vadp )
{
	strbuf *sbuf = (strbuf *)vadp;
	strbufValidateMem( sbuf );
	strbufDestruct( sbuf );
}

void stringbuffer_checkmem( void *vadp )
{
	strbuf *sbuf = (strbuf *)vadp;
	DEBUG_NOTE_MEM( vadp );
	strbufCheckMem( sbuf );
}

enum InterpErrors stringbuffer_stringbuffer(qinterp *qip, const void *mp, variant *ret)
{
	variant *varthis = getArgumentN( qip, 0 );
	jsinst *inst = varthis->data.ip;
	strbuf *sbuf;
	int size = 20;

	ASSERT_MEM( mp, sizeof(struct jsmethod) );
	varValidateMem( varthis );
	varValidateMem( ret );
	ASSERT( TYPE_INST == varthis->type );

	if ( getArgumentCount( qip ) == 2 )
	{
		if ( ! varGetInt( getArgumentN( qip, 1 ), &size ) )
		{
			return IERR_TYPE_MISMATCH;
		}
	}
	else
	{
		ASSERT ( getArgumentCount( qip ) == 1 );
	}
	inst->nativedata = malloc( sizeof(struct nativedataHolder) );
	if ( NULL == inst->nativedata )
	{
		return IERR_OUT_OF_MEM;
	}
	inst->nativedata->data = sbuf = strbufConstructBuf( size );
	if ( NULL == sbuf )
	{
		free( inst->nativedata );
		inst->nativedata = NULL;
		return IERR_OUT_OF_MEM;
	}
	inst->nativedata->finializer = stringbuffer_finializer;
	inst->nativedata->memcheck = stringbuffer_checkmem;

	inst->magic = STRBUF_MAGIC;
	return IERR_NONE;
}

enum InterpErrors stringbuffer_append(qinterp *qip, const void *mp, variant *ret)
{
	variant *varthis = getArgumentN( qip, 0 );
	variant *vararg = getArgumentN( qip, 1 );
	jsinst *inst = varthis->data.ip;
	string *str;
	strbuf *sbuf;

	ASSERT( getArgumentCount( qip ) == 2 );
	ASSERT( TYPE_INST == varthis->type );
	qinterpValidateMem( qip );
	ASSERT_MEM( mp, sizeof(struct jsmethod) );
	varValidateMem( varthis );
	varValidateMem( vararg );
	varValidateMem( ret );
	jsinstValidateMem( inst );
	ASSERT( STRBUF_MAGIC == inst->magic );

	sbuf = (strbuf *)inst->nativedata->data;
	strbufValidateMem( sbuf );

	str = varToString( vararg );
	if ( NULL == str )
	{
		return IERR_OUT_OF_MEM;
	}
	if ( ! strbufAppend( sbuf, str->cstr ) )
	{
		stringDestruct( str );
		return IERR_OUT_OF_MEM;
	}
	stringDestruct( str );
	return IERR_NONE;
}

enum InterpErrors stringbuffer_tostring(qinterp *qip, const void *mp, variant *ret)
{
	variant *varthis = getArgumentN( qip, 0 );
	jsinst *inst = varthis->data.ip;
	string *str;
	strbuf *sbuf;

	ASSERT( getArgumentCount( qip ) == 1 );
	ASSERT( TYPE_INST == varthis->type );
	ASSERT( STRBUF_MAGIC == inst->magic );
	qinterpValidateMem( qip );
	ASSERT_MEM( mp, sizeof(struct jsmethod) );
	varValidateMem( varthis );
	varValidateMem( ret );
	jsinstValidateMem( inst );

	sbuf = (strbuf *)inst->nativedata->data;
	strbufValidateMem( sbuf );

	str = strbufToString( sbuf );
	if ( NULL == str )
	{
		return IERR_OUT_OF_MEM;
	}
	if ( ! varAssignString( ret, str ) )
	{
		stringDestruct( str );
		return IERR_OUT_OF_MEM;
	}
	return IERR_NONE;
}

/*
 *  DateTime
 */
void datetime_finializer( void *vadp )
{
	time_t *t = (time_t *)vadp;
	ASSERT_MEM( t, sizeof(time_t) );
	free( t );
}

void datetime_memcheck( void *vadp )
{
	DEBUG_NOTE_MEM(vadp);
}

enum InterpErrors datetime_datetime(qinterp *qip, const void *mp, variant *ret)
{
	variant *varthis = getArgumentN( qip, 0 );
	jsinst *inst = varthis->data.ip;
	time_t *t;

	ASSERT ( getArgumentCount( qip ) == 1 );

	ASSERT_MEM( mp, sizeof(struct jsmethod) );
	varValidateMem( varthis );
	varValidateMem( ret );
	ASSERT( TYPE_INST == varthis->type );

	inst->nativedata = malloc( sizeof(struct nativedataHolder) );
	if ( NULL == inst->nativedata )
	{
		return IERR_OUT_OF_MEM;
	}
	inst->nativedata->data = t = malloc( sizeof(time_t) );
	if ( NULL == t )
	{
		free( inst->nativedata );
		inst->nativedata = NULL;
		return IERR_OUT_OF_MEM;
	}
	time( t );
	inst->nativedata->finializer = datetime_finializer;
	inst->nativedata->memcheck = datetime_memcheck;

	inst->magic = DATETIME_MAGIC;
	return IERR_NONE;
}

enum InterpErrors datetime_now(qinterp *qip, const void *mp, variant *ret)
{
	jsinst *inst;
	enum InterpErrors err;
	string *clsname;

	ASSERT ( getArgumentCount( qip ) == 0 );
	ASSERT_MEM( mp, sizeof(struct jsmethod) );
	varValidateMem( ret );

	clsname = stringConstructChar( "DateTime" );
	if ( NULL == clsname )
	{
		return IERR_OUT_OF_MEM;
	}
	err = createJSClassInstance( qip, clsname, &inst, 0 );
	stringDestruct( clsname );
	if ( IERR_NONE != err )
	{
		return err;
	}
	varAssignInst( ret, inst );
	/*jsinstDeref( inst );*/
	ASSERT( 2 == inst->refcount );
	return IERR_NONE;
}

enum InterpErrors datetime_diffms(qinterp *qip, const void *mp, variant *ret)
{
	variant *varthis = getArgumentN( qip, 0 );
	variant *vararg = getArgumentN( qip, 1 );
	jsinst *inst = varthis->data.ip;
	jsinst *arginst;
	time_t *t;

	ASSERT( getArgumentCount( qip ) == 2 );
	ASSERT( TYPE_INST == varthis->type );
	qinterpValidateMem( qip );
	ASSERT_MEM( mp, sizeof(struct jsmethod) );
	varValidateMem( varthis );
	varValidateMem( vararg );
	varValidateMem( ret );
	jsinstValidateMem( inst );

	t = (time_t *)inst->nativedata->data;
	ASSERT_MEM( t, sizeof(time_t) );

	if ( TYPE_INST != vararg->type )
	{
		return IERR_TYPE_MISMATCH;
	}
	arginst = vararg->data.ip;
	if ( DATETIME_MAGIC != arginst->magic )
	{
		return IERR_TYPE_MISMATCH;
	}
	varAssignInt( ret, (int)(*t - *(time_t *)arginst->nativedata->data) );

	return IERR_NONE;
}

enum InterpErrors datetime_tomilliseconds(qinterp *qip, const void *mp, variant *ret)
{
	variant *varthis = getArgumentN( qip, 0 );
	jsinst *inst = varthis->data.ip;
	time_t *t;

	ASSERT( getArgumentCount( qip ) == 1 );
	ASSERT( TYPE_INST == varthis->type );
	qinterpValidateMem( qip );
	ASSERT_MEM( mp, sizeof(struct jsmethod) );
	varValidateMem( varthis );
	varValidateMem( ret );
	jsinstValidateMem( inst );
	ASSERT ( DATETIME_MAGIC == inst->magic );

	t = (time_t *)inst->nativedata->data;
	ASSERT_MEM( t, sizeof(time_t) );

	varAssignInt( ret, (int)(*t) );

	return IERR_NONE;
}

/*
 *  math
 */

enum InterpErrors math_rand(qinterp *qip, const void *mp, variant *ret)
{
	ASSERT( getArgumentCount( qip ) == 0 );
	ASSERT_MEM( mp, sizeof(struct jsmethod) );
	varValidateMem( ret );

	varAssignFloat( ret, (float)uniform() );
	return IERR_NONE;
}

enum InterpErrors math_exp(qinterp *qip, const void *mp, variant *ret)
{
	variant *vararg = getArgumentN( qip, 0 );
	float farg;

	ASSERT( getArgumentCount( qip ) == 1 );
	ASSERT_MEM( mp, sizeof(struct jsmethod) );
	varValidateMem( ret );
	varValidateMem( vararg );

	if ( ! varGetFloat( vararg, &farg ) )
	{
		return IERR_TYPE_MISMATCH;
	}

	varAssignFloat( ret, (float)exp(farg) );
	return IERR_NONE;
}

/*
 *  hashtable
 */
void hashtable_finializer( void *vadp )
{
	int x, count;
	hashtable *ht = (hashtable *)vadp;
	vector *vec = hashtableValues( ht );
	variant *vardata;

	count = vectorCount( vec );
	for ( x = 0; x < count; x++ )
	{
		vardata = (variant *)vectorElementAt( vec, x );
		varValidateMem( vardata );
		varDestruct( vardata );
	}
	vectorDestruct( vec );
	hashtableDestruct( ht );
}

void hashtable_memcheck( void *vadp )
{
	int x, count;
	hashtable *ht = (hashtable *)vadp;
	vector *vec = hashtableValues( ht );
	variant *vardata;

	DEBUG_NOTE_MEM( vadp );
	hashtableCheckMem( ht );
	count = vectorCount( vec );
	for ( x = 0; x < count; x++ )
	{
		vardata = (variant *)vectorElementAt( vec, x );
		varCheckMem( vardata );
	}
	vectorDestruct( vec );
}

enum InterpErrors hashtable_hashtable(qinterp *qip, const void *mp, variant *ret)
{
	variant *varthis = getArgumentN( qip, 0 );
	jsinst *inst = varthis->data.ip;

	ASSERT ( getArgumentCount( qip ) == 1 );

	ASSERT_MEM( mp, sizeof(struct jsmethod) );
	varValidateMem( varthis );
	varValidateMem( ret );
	ASSERT( TYPE_INST == varthis->type );

	inst->nativedata = malloc( sizeof(struct nativedataHolder) );
	if ( NULL == inst->nativedata )
	{
		return IERR_OUT_OF_MEM;
	}
	inst->nativedata->data = hashtableConstruct();
	if ( NULL == inst->nativedata->data )
	{
		free( inst->nativedata );
		inst->nativedata = NULL;
		return IERR_OUT_OF_MEM;
	}
	inst->nativedata->finializer = hashtable_finializer;
	inst->nativedata->memcheck = hashtable_memcheck;

	inst->magic = HASHTABLE_MAJIC;
	return IERR_NONE;
}

enum InterpErrors hashtable_put(qinterp *qip, const void *mp, variant *ret)
{
	variant *varthis = getArgumentN( qip, 0 );
	variant *varkey = getArgumentN( qip, 1 );
	variant *varval = getArgumentN( qip, 2 );
	variant *vardup;
	jsinst *inst = varthis->data.ip;
	hashtable *ht;
	string *strkey;

	ASSERT ( getArgumentCount( qip ) == 3 );

	ASSERT_MEM( mp, sizeof(struct jsmethod) );
	varValidateMem( varthis );
	varValidateMem( varkey);
	varValidateMem( varval );
	varValidateMem( ret );
	ASSERT( TYPE_INST == varthis->type );
	ASSERT( HASHTABLE_MAJIC == inst->magic );

	ht = (hashtable *)inst->nativedata->data;
	hashtableValidateMem( ht );

	vardup = varConstructDup( varval );
	if ( NULL == vardup )
	{
		return IERR_OUT_OF_MEM;
	}
	strkey = varToString( varkey );
	if ( NULL == strkey )
	{
		varDestruct( vardup );
		return IERR_OUT_OF_MEM;
	}
	if ( ! hashtableAdd( ht, strkey, vardup ) )
	{
		varDestruct( vardup );
		stringDestruct( strkey );
		return IERR_OUT_OF_MEM;
	}
	stringDestruct( strkey );
	return IERR_NONE;
}

enum InterpErrors hashtable_get(qinterp *qip, const void *mp, variant *ret)
{
	variant *varthis = getArgumentN( qip, 0 );
	variant *varkey = getArgumentN( qip, 1 );
	jsinst *inst = varthis->data.ip;
	hashtable *ht;
	variant *varval;
	string *strkey;

	ASSERT ( getArgumentCount( qip ) == 2 );

	ASSERT_MEM( mp, sizeof(struct jsmethod) );
	varValidateMem( varthis );
	varValidateMem( varkey);
	varValidateMem( ret );
	ASSERT( TYPE_INST == varthis->type );
	ASSERT( HASHTABLE_MAJIC == inst->magic );

	ht = (hashtable *)inst->nativedata->data;
	hashtableValidateMem( ht );

	strkey = varToString( varkey );
	if ( NULL == strkey )
	{
		return IERR_OUT_OF_MEM;
	}
	varval = (variant *)hashtableGet( ht, strkey );
	stringDestruct( strkey );

	if ( varval == NULL )
	{
		varAssign( ret, &qip->null );
	}
	else
	{
		varAssign( ret, varval );
	}

	return IERR_NONE;
}

enum InterpErrors hashtable_clear(qinterp *qip, const void *mp, variant *ret)
{
	variant *varthis = getArgumentN( qip, 0 );
	jsinst *inst = varthis->data.ip;

	ASSERT ( getArgumentCount( qip ) == 1 );

	ASSERT_MEM( mp, sizeof(struct jsmethod) );
	varValidateMem( varthis );
	varValidateMem( ret );
	ASSERT( TYPE_INST == varthis->type );
	ASSERT( HASHTABLE_MAJIC == inst->magic );

	hashtableDestruct( (hashtable *)inst->nativedata->data );
	inst->nativedata->data = hashtableConstruct();
	if ( NULL == inst->nativedata->data )
	{
		return IERR_OUT_OF_MEM;
	}
	return IERR_NONE;
}


