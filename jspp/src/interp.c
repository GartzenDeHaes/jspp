#ifdef HAVE_CONFIG_H
#include <jspp/config.h>
#endif
#include <stdlib.h>
#include <spl/debug.h>
#include <jspp/interp.h>
#include <jspp/intrinsics.h>
#include <spl/rand.h>

enum InterpErrors _interp( qinterp *qip );
enum InterpErrors callStaticMethod( qinterp *qip, const string *fqname );
bool qinterpLoadIntrinsics( qinterp *qip );
enum InterpErrors callInstMethod( qinterp *qip, jsinst *inst, const string *mname, const bool nopop);
bool libPrepare( program *pp );

qinterp *qinterpConstruct()
{
	qinterp *qip = (qinterp *)malloc( sizeof( qinterp ) );
	if ( NULL == qip )
	{
		return NULL;
	}
	qip->programs = vectorConstruct( sizeof( program *) );
	if ( NULL == qip->programs )
	{
		free( qip );
		return NULL;
	}
	qip->classes = hashtableConstruct();
	if ( NULL == qip->classes )
	{
		vectorDestruct( qip->programs );
		free( qip );
		return NULL;
	}
	qip->tempdata1.type = TYPE_VOID;
	qip->tempdata1.data.i = 0;

	qip->frames = vectorConstruct( sizeof( stackframe *) );
	if ( NULL == qip->frames )
	{
		hashtableDestruct( qip->classes );
		vectorDestruct( qip->programs );
		free( qip );
		return NULL;		
	}
	if ( NULL == (qip->stack = vectorConstruct( sizeof( variant ) )) )
	{
		vectorDestruct( qip->frames );
		hashtableDestruct( qip->classes );
		vectorDestruct( qip->programs );
		free( qip );
		return NULL;
	}
	if ( NULL == (qip->stkframecache = vectorConstruct(sizeof(stackframe *))) )
	{
		vectorDestruct( qip->stack );
		vectorDestruct( qip->frames );
		hashtableDestruct( qip->classes );
		vectorDestruct( qip->programs );
		free( qip );
		return NULL;
	}
	qip->curframe = NULL;

	qip->null.type = TYPE_NULL;
	qip->null.data.i = 0;

	if ( ! qinterpLoadIntrinsics( qip ) )
	{
		vectorDestruct( qip->stkframecache );
		vectorDestruct( qip->stack );
		vectorDestruct( qip->frames );
		hashtableDestruct( qip->classes );
		vectorDestruct( qip->programs );
		free( qip );
		return NULL;		
	}
	qip->linenum = 0;
	return qip;
}

void qinterpDestruct( qinterp *qip )
{
	int x;
	int count = vectorCount(qip->programs);
	qinterpValidateMem( qip );
	for ( x = 0; x < count; x++ )
	{
		program *pp = (program *)vectorElementAt( qip->programs, x );
		programDestruct( pp );
	}
	vectorDestruct( qip->programs );

	count = vectorCount(qip->frames);
	for ( x = 0; x < count; x++ )
	{
		stackframe *sfp = (stackframe *)vectorElementAt( qip->frames, x );
		free( sfp );
	}
	count = vectorCount(qip->stack);
	for ( x = 0; x < count; x++ )
	{
		varClear( vectorDataElementAt( qip->stack, x ) );
	}

	count = vectorCount(qip->stkframecache);
	for ( x = 0; x < count; x++ )
	{
		stackframe *sf = (stackframe *)vectorElementAt( qip->stkframecache, x );
		ASSERT_MEM( sf, sizeof(struct stackframe) );
		vectorValidateMem( sf->locals );
		ASSERT( vectorCount(sf->locals) == 0 );
		VARCLEAR( &sf->retval );
		vectorDestruct( sf->locals );
		vectorDestruct( sf->blockframes );
		free( sf );
	}

	vectorDestruct( qip->stkframecache );
	vectorDestruct( qip->frames );
	vectorDestruct( qip->stack );
	hashtableDestruct( qip->classes );
	varClear( &qip->tempdata1 );
	free( qip );
}

bool qinterpPrepare( qinterp *qip, program *pp )
{
	int x;
	int count;
	enum InterpErrors err;

	ASSERT( pp->errornum == 0 );

	count = vectorCount(pp->lclasses);
	for ( x = 0; x < count; x++ )
	{
		jsclass *jsp = (jsclass *)vectorElementAt( pp->lclasses, x );
		jsclassValidateMem( jsp );
		if ( ! hashtableAdd( qip->classes, jsp->name, jsp ) )
		{
			return false;
		}
		if ( jsp->basename->len > 0 )
		{
			jsp->baseclass = programFindClass( pp, jsp->basename->cstr );
			if ( NULL == jsp->baseclass )
			{
				jsp->baseclass = qinterpFindClass( qip, jsp->basename );
				if ( NULL == jsp->baseclass )
				{
					return false;
				}
			}
			if ( ! jsclassInitVtab ( jsp ) )
			{
				return false;
			}
		}
		jsp->constdata = pp->cdata;

		/* init static values */
		err = qinterpRun( qip, jsp->name->cstr, "@@constructor", 0, NULL );
		if ( IERR_NONE != err )
		{
			return false;
		}
	}
	return vectorAdd( qip->programs, &pp );
}

void qinterpRemoveProgram( qinterp *qip, program *pp )
{
	int count = vectorCount( qip->programs );
	int x;
	for ( x = 0; x < count; x++ )
	{
		if ( vectorElementAt(qip->programs, x) == pp )
		{
			vectorRemoveAt(qip->programs, x);
			break;
		}
	}
	count = vectorCount(pp->lclasses);
	for ( x = 0; x < count; x++ )
	{
		jsclass *jsp = (jsclass *)vectorElementAt( pp->lclasses, x );
		if ( ! hashtableRemove( qip->classes, jsp->name ) )
		{
			/* this is bad ... */
		}
	}
}

variant *qinterpResolve( qinterp *qip, const enum ArgSource am, const int32 arg, variant *imvar )
{
	qinterpValidateMem( qip );

	switch ( am )
	{
	case MODE_IM:
		VARCLEAR( imvar );
		imvar->type = TYPE_INT;
		imvar->data.i = arg;
		return imvar;

	case MODE_CONST:
		return (variant *)vectorDataElementAt( qip->curframe->ctx->constdata, arg );
		/*break;*/

	case MODE_CLASS:
		jsclassValidateMem( qip->curframe->ctx );
		ASSERT( arg < vectorCount(qip->curframe->ctx->staticlocations) );
		return (variant *)vectorElementAt( qip->curframe->ctx->staticlocations, arg );

	case MODE_INST:
		{
			variant *var = vectorDataElementAt(qip->curframe->locals, 0);
			varValidateMem( var );
			ASSERT( TYPE_INST == var->type );
			return vectorDataElementAt(var->data.ip->fields, arg);
		}

	case MODE_BLOCK:
		return (variant *)vectorDataElementAt(qip->curframe->locals, arg);

	default:
		ASSERT(0);
		return NULL;

	}
#ifdef _WIN32
	return NULL;	/* make compiler happy, very happy indeed. */
#endif
}

enum InterpErrors qinterpRun
( 
	qinterp *qip, 
	const char *startcls, 
	const char *mainmethod, const int argcount,
	variant **varret 
)
{
	string *name;
	string *methodname;
	string *sig;
	enum InterpErrors err;
	int x;

	name = stringConstructChar( startcls );
	if ( NULL == name )
	{
		return IERR_OUT_OF_MEM;
	}
	methodname = stringConstructChar( mainmethod );
	if ( NULL == methodname )
	{
		stringDestruct( name );
		return IERR_OUT_OF_MEM;
	}
	if ( ! stringAppendChar( name, "." ) )
	{
		stringDestruct( name );
		return IERR_OUT_OF_MEM;
	}
	sig = buildMethodSig( methodname, 1 );
	if ( NULL == sig )
	{
		stringDestruct( name );
		return IERR_OUT_OF_MEM;
	}
	for(x = 0; x < argcount; x++ )
	{
		methodSigAppendArg( sig );
	}
	if ( ! stringAppend( name, sig ) )
	{
		stringDestruct( sig );
		stringDestruct( name );
		return IERR_OUT_OF_MEM;
	}
	stringDestruct( sig );

	err = callStaticMethod( qip, name );
	
	stringDestruct( name );
	stringDestruct( methodname );

	if ( 0 != vectorCount( qip->stack ) )
	{
		*varret = varConstructDup( (variant *)vectorDataPeek( qip->stack ) );
		varClear( (variant *)vectorDataPeek(qip->stack) );
		vectorPop( qip->stack );
	}
	return err;
}

jsclass *qinterpFindClass( qinterp *qip, const string *clsname )
{
	qinterpValidateMem( qip );
	stringValidateMem( clsname );
	return (jsclass *)hashtableGet( qip->classes, clsname );
}

bool qinterpLoadIntrinsics( qinterp *qip )
{
	jsclass *jsp;
	jsmethod *mp;
	program *pp = programConstruct();

	/*
	 *  object
	 */
	jsp = programDefineClass( pp, "Object", "", NULL );
	if ( NULL == jsp )
	{
		programDestruct( pp );
		return false;
	}
	mp = programDefineMethod( pp, "@@constructor", true, ACC_PRIVATE );
	mp = programDefineMethod( pp, "@@constructor", false, ACC_PRIVATE );
	jsmethodAddArg( mp, "this", false );

	mp = programDefineNativeMethod( pp, object_parseInt, "parseInt", true, ACC_PUBLIC, true );
	jsmethodAddArg( mp, "val", false );
	mp = programDefineNativeMethod( pp, object_parseFloat, "parseFloat", true, ACC_PUBLIC, true );
	jsmethodAddArg( mp, "val", false );
	mp = programDefineNativeMethod( pp, object_toString, "toString", true, ACC_PUBLIC, true );
	jsmethodAddArg( mp, "val", false );
	mp = programDefineNativeMethod( pp, object_toStringI, "toString", false, ACC_PUBLIC, true );
	jsmethodAddArg( mp, "this", false );
	mp = programDefineNativeMethod( pp, object_typename, "typename", true, ACC_PUBLIC, true );
	jsmethodAddArg( mp, "typ", false );
	mp = programDefineNativeMethod( pp, object_length, "length", true, ACC_PUBLIC, true );
	jsmethodAddArg( mp, "obj", false );

	/*
	 *  array
	 */
	jsp = programDefineClass( pp, "Array", "", NULL );
	if ( NULL == jsp )
	{
		programDestruct( pp );
		return false;
	}
	mp = programDefineMethod( pp, "@@constructor", true, ACC_PRIVATE );
	mp = programDefineMethod( pp, "@@constructor", false, ACC_PRIVATE );
	jsmethodAddArg( mp, "this", false );
	mp = programDefineNativeMethod( pp, array_arrayX, "Array", false, ACC_PUBLIC, false );
	jsmethodAddArg( mp, "this", false );
	jsmethodAddArg( mp, "len", false );

	mp = programDefineNativeMethod( pp, array_arrayX, "Array", false, ACC_PUBLIC, false );
	jsmethodAddArg( mp, "this", false );
	jsmethodAddArg( mp, "dim1", false );
	jsmethodAddArg( mp, "dim2", false );

	mp = programDefineNativeMethod( pp, array_arrayX, "Array", false, ACC_PUBLIC, false );
	jsmethodAddArg( mp, "this", false );
	jsmethodAddArg( mp, "dim0", false );
	jsmethodAddArg( mp, "dim1", false );
	jsmethodAddArg( mp, "dim2", false );

	mp = programDefineNativeMethod( pp, array_arrayX, "Array", false, ACC_PUBLIC, false );
	jsmethodAddArg( mp, "this", false );
	jsmethodAddArg( mp, "dim0", false );
	jsmethodAddArg( mp, "dim1", false );
	jsmethodAddArg( mp, "dim2", false );
	jsmethodAddArg( mp, "dim3", false );

	mp = programDefineNativeMethod( pp, array_arrayX, "Array", false, ACC_PUBLIC, false );
	jsmethodAddArg( mp, "this", false );
	jsmethodAddArg( mp, "index0", false );
	jsmethodAddArg( mp, "index1", false );
	jsmethodAddArg( mp, "index2", false );
	jsmethodAddArg( mp, "index3", false );
	jsmethodAddArg( mp, "index4", false );

	mp = programDefineNativeMethod( pp, array_arrayX, "Array", false, ACC_PUBLIC, false );
	jsmethodAddArg( mp, "this", false );
	jsmethodAddArg( mp, "index0", false );
	jsmethodAddArg( mp, "index1", false );
	jsmethodAddArg( mp, "index2", false );
	jsmethodAddArg( mp, "index3", false );
	jsmethodAddArg( mp, "index4", false );
	jsmethodAddArg( mp, "index5", false );

	mp = programDefineNativeMethod( pp, array_arrayX, "Array", false, ACC_PUBLIC, false );
	jsmethodAddArg( mp, "this", false );
	jsmethodAddArg( mp, "index0", false );
	jsmethodAddArg( mp, "index1", false );
	jsmethodAddArg( mp, "index2", false );
	jsmethodAddArg( mp, "index3", false );
	jsmethodAddArg( mp, "index4", false );
	jsmethodAddArg( mp, "index5", false );
	jsmethodAddArg( mp, "index6", false );

	mp = programDefineNativeMethod( pp, array_arrayX, "Array", false, ACC_PUBLIC, false );
	jsmethodAddArg( mp, "this", false );
	jsmethodAddArg( mp, "index0", false );
	jsmethodAddArg( mp, "index1", false );
	jsmethodAddArg( mp, "index2", false );
	jsmethodAddArg( mp, "index3", false );
	jsmethodAddArg( mp, "index4", false );
	jsmethodAddArg( mp, "index5", false );
	jsmethodAddArg( mp, "index6", false );
	jsmethodAddArg( mp, "index7", false );

	mp = programDefineNativeMethod( pp, array_arrayX, "Array", false, ACC_PUBLIC, false );
	jsmethodAddArg( mp, "this", false );
	jsmethodAddArg( mp, "index0", false );
	jsmethodAddArg( mp, "index1", false );
	jsmethodAddArg( mp, "index2", false );
	jsmethodAddArg( mp, "index3", false );
	jsmethodAddArg( mp, "index4", false );
	jsmethodAddArg( mp, "index5", false );
	jsmethodAddArg( mp, "index6", false );
	jsmethodAddArg( mp, "index7", false );
	jsmethodAddArg( mp, "index8", false );

	mp = programDefineNativeMethod( pp, array_arrayX, "Array", false, ACC_PUBLIC, false );
	jsmethodAddArg( mp, "this", false );
	jsmethodAddArg( mp, "index0", false );
	jsmethodAddArg( mp, "index1", false );
	jsmethodAddArg( mp, "index2", false );
	jsmethodAddArg( mp, "index3", false );
	jsmethodAddArg( mp, "index4", false );
	jsmethodAddArg( mp, "index5", false );
	jsmethodAddArg( mp, "index6", false );
	jsmethodAddArg( mp, "index7", false );
	jsmethodAddArg( mp, "index8", false );
	jsmethodAddArg( mp, "index9", false );

	mp = programDefineNativeMethod( pp, array_length, "length", false, ACC_PUBLIC, true );
	jsmethodAddArg( mp, "this", false );
	jsmethodAddArg( mp, "index", false );

	mp = programDefineNativeMethod( pp, array_length, "length", false, ACC_PUBLIC, true );
	jsmethodAddArg( mp, "this", false );

	mp = programDefineNativeMethod( pp, array_dims, "dims", false, ACC_PUBLIC, true );
	jsmethodAddArg( mp, "this", false );

	mp = programDefineNativeMethod( pp, array_itemX, "[]", false, ACC_PUBLIC, true );
	jsmethodAddArg( mp, "this", false );
	jsmethodAddArg( mp, "index0", false );

	mp = programDefineNativeMethod( pp, array_itemX, "[]", false, ACC_PUBLIC, true );
	jsmethodAddArg( mp, "this", false );
	jsmethodAddArg( mp, "index0", false );
	jsmethodAddArg( mp, "index1", false );

	mp = programDefineNativeMethod( pp, array_itemX, "[]", false, ACC_PUBLIC, true );
	jsmethodAddArg( mp, "this", false );
	jsmethodAddArg( mp, "index0", false );
	jsmethodAddArg( mp, "index1", false );
	jsmethodAddArg( mp, "index2", false );

	mp = programDefineNativeMethod( pp, array_itemX, "[]", false, ACC_PUBLIC, true );
	jsmethodAddArg( mp, "this", false );
	jsmethodAddArg( mp, "index0", false );
	jsmethodAddArg( mp, "index1", false );
	jsmethodAddArg( mp, "index2", false );
	jsmethodAddArg( mp, "index3", false );

	mp = programDefineNativeMethod( pp, array_itemX, "[]", false, ACC_PUBLIC, true );
	jsmethodAddArg( mp, "this", false );
	jsmethodAddArg( mp, "index0", false );
	jsmethodAddArg( mp, "index1", false );
	jsmethodAddArg( mp, "index2", false );
	jsmethodAddArg( mp, "index3", false );
	jsmethodAddArg( mp, "index4", false );

	mp = programDefineNativeMethod( pp, array_itemX, "[]", false, ACC_PUBLIC, true );
	jsmethodAddArg( mp, "this", false );
	jsmethodAddArg( mp, "index0", false );
	jsmethodAddArg( mp, "index1", false );
	jsmethodAddArg( mp, "index2", false );
	jsmethodAddArg( mp, "index3", false );
	jsmethodAddArg( mp, "index4", false );
	jsmethodAddArg( mp, "index5", false );

	mp = programDefineNativeMethod( pp, array_itemX, "[]", false, ACC_PUBLIC, true );
	jsmethodAddArg( mp, "this", false );
	jsmethodAddArg( mp, "index0", false );
	jsmethodAddArg( mp, "index1", false );
	jsmethodAddArg( mp, "index2", false );
	jsmethodAddArg( mp, "index3", false );
	jsmethodAddArg( mp, "index4", false );
	jsmethodAddArg( mp, "index5", false );
	jsmethodAddArg( mp, "index6", false );

	mp = programDefineNativeMethod( pp, array_itemX, "[]", false, ACC_PUBLIC, true );
	jsmethodAddArg( mp, "this", false );
	jsmethodAddArg( mp, "index0", false );
	jsmethodAddArg( mp, "index1", false );
	jsmethodAddArg( mp, "index2", false );
	jsmethodAddArg( mp, "index3", false );
	jsmethodAddArg( mp, "index4", false );
	jsmethodAddArg( mp, "index5", false );
	jsmethodAddArg( mp, "index6", false );
	jsmethodAddArg( mp, "index7", false );

	mp = programDefineNativeMethod( pp, array_itemX, "[]", false, ACC_PUBLIC, true );
	jsmethodAddArg( mp, "this", false );
	jsmethodAddArg( mp, "index0", false );
	jsmethodAddArg( mp, "index1", false );
	jsmethodAddArg( mp, "index2", false );
	jsmethodAddArg( mp, "index3", false );
	jsmethodAddArg( mp, "index4", false );
	jsmethodAddArg( mp, "index5", false );
	jsmethodAddArg( mp, "index6", false );
	jsmethodAddArg( mp, "index7", false );
	jsmethodAddArg( mp, "index8", false );

	mp = programDefineNativeMethod( pp, array_itemX, "[]", false, ACC_PUBLIC, true );
	jsmethodAddArg( mp, "this", false );
	jsmethodAddArg( mp, "index0", false );
	jsmethodAddArg( mp, "index1", false );
	jsmethodAddArg( mp, "index2", false );
	jsmethodAddArg( mp, "index3", false );
	jsmethodAddArg( mp, "index4", false );
	jsmethodAddArg( mp, "index5", false );
	jsmethodAddArg( mp, "index6", false );
	jsmethodAddArg( mp, "index7", false );
	jsmethodAddArg( mp, "index8", false );
	jsmethodAddArg( mp, "index9", false );

	/*
	 *  vector
	 */
	jsp = programDefineClass( pp, "Vector", "", NULL );
	if ( NULL == jsp )
	{
		programDestruct( pp );
		return false;
	}
	mp = programDefineMethod( pp, "@@constructor", true, ACC_PRIVATE );
	mp = programDefineMethod( pp, "@@constructor", false, ACC_PRIVATE );
	jsmethodAddArg( mp, "this", false );

	mp = programDefineNativeMethod( pp, vector_vector, "Vector", false, ACC_PUBLIC, false );
	jsmethodAddArg( mp, "this", false );

	mp = programDefineNativeMethod( pp, vector_add, "add", false, ACC_PUBLIC, false );
	jsmethodAddArg( mp, "this", false );
	jsmethodAddArg( mp, "item", false );

	mp = programDefineNativeMethod( pp, vector_size, "size", false, ACC_PUBLIC, true );
	jsmethodAddArg( mp, "this", false );

	mp = programDefineNativeMethod( pp, vector_size, "length", false, ACC_PUBLIC, true );
	jsmethodAddArg( mp, "this", false );

	mp = programDefineNativeMethod( pp, vector_elementat, "elementAt", false, ACC_PUBLIC, true );
	jsmethodAddArg( mp, "this", false );
	jsmethodAddArg( mp, "idx", false );

	mp = programDefineNativeMethod( pp, vector_elementat, "[]", false, ACC_PUBLIC, true );
	jsmethodAddArg( mp, "this", false );
	jsmethodAddArg( mp, "idx", false );

	mp = programDefineNativeMethod( pp, vector_setelementat, "setElementAt", false, ACC_PUBLIC, false );
	jsmethodAddArg( mp, "this", false );
	jsmethodAddArg( mp, "idx", false );
	jsmethodAddArg( mp, "item", false );

	mp = programDefineNativeMethod( pp, vector_clear, "clear", false, ACC_PUBLIC, false );
	jsmethodAddArg( mp, "this", false );

#ifdef CONSOLE
	jsp = programDefineClass( pp, "Console", "", NULL );
	if ( NULL == jsp )
	{
		programDestruct( pp );
		return false;
	}
	mp = programDefineMethod( pp, "@@constructor", true, ACC_PRIVATE );
	mp = programDefineNativeMethod( pp, console_writeln, "writeln", true, ACC_PUBLIC, false );
	jsmethodAddArg( mp, "arg", false );
	mp = programDefineNativeMethod( pp, console_write, "write", true, ACC_PUBLIC, false );
	jsmethodAddArg( mp, "arg", false );
	mp = programDefineNativeMethod( pp, console_readln, "readln", true, ACC_PUBLIC, true );
	
#endif

	/*
	 *  stringbuffer
	 */
	jsp = programDefineClass( pp, "StringBuffer", "", NULL );
	if ( NULL == jsp )
	{
		programDestruct( pp );
		return false;
	}
	mp = programDefineMethod( pp, "@@constructor", true, ACC_PRIVATE );
	mp = programDefineMethod( pp, "@@constructor", false, ACC_PRIVATE );
	jsmethodAddArg( mp, "this", false );

	mp = programDefineNativeMethod( pp, stringbuffer_stringbuffer, "StringBuffer", false, ACC_PUBLIC, false );
	jsmethodAddArg( mp, "this", false );

	mp = programDefineNativeMethod( pp, stringbuffer_stringbuffer, "StringBuffer", false, ACC_PUBLIC, false );
	jsmethodAddArg( mp, "this", false );
	jsmethodAddArg( mp, "size", false );

	mp = programDefineNativeMethod( pp, stringbuffer_append, "append", false, ACC_PUBLIC, false );
	jsmethodAddArg( mp, "this", false );
	jsmethodAddArg( mp, "arg", false );

	mp = programDefineNativeMethod( pp, stringbuffer_tostring, "toString", false, ACC_PUBLIC, true );
	jsmethodAddArg( mp, "this", false );

	/*
	 *  datetime
	 */
	jsp = programDefineClass( pp, "DateTime", "", NULL );
	if ( NULL == jsp )
	{
		programDestruct( pp );
		return false;
	}
	mp = programDefineMethod( pp, "@@constructor", true, ACC_PRIVATE );
	mp = programDefineMethod( pp, "@@constructor", false, ACC_PRIVATE );
	jsmethodAddArg( mp, "this", false );

	mp = programDefineNativeMethod( pp, datetime_datetime, "DateTime", false, ACC_PUBLIC, false );
	jsmethodAddArg( mp, "this", false );

	mp = programDefineNativeMethod( pp, datetime_diffms, "diffms", false, ACC_PUBLIC, true );
	jsmethodAddArg( mp, "this", false );
	jsmethodAddArg( mp, "dtm", false );

	mp = programDefineNativeMethod( pp, datetime_now, "now", true, ACC_PUBLIC, true );

	mp = programDefineNativeMethod( pp, datetime_tomilliseconds, "toMilliseconds", false, ACC_PUBLIC, true );
	jsmethodAddArg( mp, "this", false );

	/*
	 *  math
	 */
	initRandom();
	jsp = programDefineClass( pp, "Math", "", NULL );
	if ( NULL == jsp )
	{
		programDestruct( pp );
		return false;
	}
	mp = programDefineMethod( pp, "@@constructor", true, ACC_PRIVATE );
	mp = programDefineMethod( pp, "@@constructor", false, ACC_PRIVATE );
	jsmethodAddArg( mp, "this", false );

	mp = programDefineNativeMethod( pp, math_rand, "rand", true, ACC_PUBLIC, true );
	
	mp = programDefineNativeMethod( pp, math_exp, "exp", true, ACC_PUBLIC, true );
	jsmethodAddArg( mp, "num", false );

	/*
	 *  Hashtable
	 */
	initRandom();
	jsp = programDefineClass( pp, "Hashtable", "", NULL );
	if ( NULL == jsp )
	{
		programDestruct( pp );
		return false;
	}
	mp = programDefineMethod( pp, "@@constructor", true, ACC_PRIVATE );
	mp = programDefineMethod( pp, "@@constructor", false, ACC_PRIVATE );
	jsmethodAddArg( mp, "this", false );

	mp = programDefineNativeMethod( pp, hashtable_hashtable, "Hashtable", false, ACC_PUBLIC, false );
	jsmethodAddArg( mp, "this", false );
	mp = programDefineNativeMethod( pp, hashtable_put, "put", false, ACC_PUBLIC, false );
	jsmethodAddArg( mp, "this", false );
	jsmethodAddArg( mp, "key", false );
	jsmethodAddArg( mp, "val", false );
	mp = programDefineNativeMethod( pp, hashtable_get, "get", false, ACC_PUBLIC, true );
	jsmethodAddArg( mp, "this", false );
	jsmethodAddArg( mp, "key", false );
	mp = programDefineNativeMethod( pp, hashtable_clear, "clear", false, ACC_PUBLIC, false );
	jsmethodAddArg( mp, "this", false );

	if ( ! libPrepare( pp ) )
	{
		programDestruct( pp );
		return false;
	}

	if ( ! qinterpPrepare( qip, pp ) )
	{
		programDestruct( pp );
		return false;
	}
	return true;
}

program *qinterpGetIntrinsics( qinterp *qip )
{
	return (program *)vectorElementAt( qip->programs, 0 );
}

char *qinterpGetErrorMessage( const enum InterpErrors err, char *buf, const int buflen )
{
	switch( err )
	{
	case IERR_NONE:
		strcpy( buf, "No error" );
		break;
	case IERR_OUT_OF_MEM:
		strcpy( buf, "Out of memory" );
		break;
	case IERR_MAIN_CLASS_NOT_FOUND:
		strcpy( buf, "main() not found" );
		break;
	case IERR_CLASS_NOT_FOUND:
		strcpy( buf, "Undefined class" );
		break;
	case IERR_UNKNOWN_METHOD:
		strcpy( buf, "Undefined method" );
		break;
	case IERR_UNKNOWN_OPCODE:
		strcpy( buf, "Internal error: unknown opcode" );
		break;
	case IERR_FIELD_NOT_FOUND:
		strcpy( buf, "Undefined field" );
		break;
	case IERR_TYPE_MISMATCH:
		strcpy( buf, "Type mismatch" );
		break;
	case IERR_INTERNAL_ERROR1:
		strcpy( buf, "Internal error: 1" );
		break;
	case IERR_INTERNAL_ERROR2:
		strcpy( buf, "Internal error: 2" );
		break;
	case IERR_BAD_BREAK:
		strcpy( buf, "Bad break" );
		break;
	case IERR_BAD_CONTINUE:
		strcpy( buf, "Bad continue" );
		break;
	case IERR_EXCEPTION:
		strcpy( buf, "Unhandled exception" );
		break;
	case IERR_CALL_REQUIRES_THIS:
		strcpy( buf, "Internal error: call requires this pointer" );
		break;
	case IERR_INDEX_OUT_OF_BOUNDS:
		strcpy( buf, "Index out of bounds" );
		break;
	case IERR_WRONG_ARRAY_INDEXS:
		strcpy( buf, "Incorrect array index reference" );
		break;
	default:
		strcpy( buf, "Unknown error" );
		break;
	}
	return buf;
}

#ifdef DEBUG

void qinterpValidateMem( const qinterp *qip )
{
	int x, y, count;
	program *pp;

	ASSERT_MEM( qip, sizeof( qinterp ) );
	hashtableValidateMem( qip->classes );
	vectorValidateMem( qip->programs );
	vectorValidateMem( qip->frames );
	vectorValidateMem( qip->stack );

	count = vectorCount( qip->programs );
	for ( x = 0; x < count; x++ )
	{
		pp = (program *)vectorElementAt( qip->programs, x );
		programValidateMem( pp );
	}
	count = vectorCount(qip->frames);
	for ( x = 0; x < count; x++ )
	{
		stackframe *sfp = (stackframe *)vectorElementAt( qip->frames, x );
		ASSERT_MEM( sfp, sizeof(struct stackframe) );
		vectorValidateMem( sfp->locals );
		varValidateContentMem( &sfp->retval );
		for ( y = 0; y < vectorCount( sfp->locals ); y++ )
		{
			variant *var = (variant *)vectorDataElementAt( sfp->locals, y );
			varValidateContentMem( var );
		}
	}
	count = vectorCount( qip->stack );
	for ( x = 0; x < count; x++ )
	{
		variant *var = (variant *)vectorDataElementAt( qip->stack, x );
		varValidateContentMem( var );
	}
	vectorCheckMem( qip->stkframecache );
	count = vectorCount(qip->stkframecache);
	for ( x = 0; x < count; x++ )
	{
		stackframe *sf = (stackframe *)vectorElementAt( qip->stkframecache, x );
		ASSERT_MEM( sf, sizeof(stackframe) );
		vectorValidateMem( sf->locals );
		vectorValidateMem( sf->blockframes );
	}
}

void qinterpCheckMem( const qinterp *qip )
{
	int x, y, count;
	program *pp;

	DEBUG_NOTE_MEM_ALLOCATION(qip);
	hashtableCheckMem( qip->classes );
	vectorCheckMem( qip->programs );
	vectorCheckMem( qip->frames );
	vectorCheckMem( qip->stack );

	count = vectorCount( qip->programs );
	for ( x = 0; x < count; x++ )
	{
		pp = (program *)vectorElementAt( qip->programs, x );
		programCheckMem( pp );
	}
	count = vectorCount( qip->frames );
	for ( x = 0; x < count; x++ )
	{
		int count2;
		stackframe *sfp = (stackframe *)vectorElementAt( qip->frames, x );
		DEBUG_NOTE_MEM_ALLOCATION( sfp );
		vectorCheckMem( sfp->locals );
		varCheckContentMem( &sfp->retval );
		count2 = vectorCount( sfp->locals );
		for ( y = 0; y < count2; y++ )
		{
			variant *var = (variant *)vectorDataElementAt( sfp->locals, y );
			varCheckContentMem( var );
		}
		vectorCheckMem( sfp->blockframes );
		count2 = vectorCount( sfp->blockframes );
		for ( y = 0; y < count2; y++ )
		{
			blockframe *bfp = (blockframe *)vectorDataElementAt( sfp->blockframes, y );
			DEBUG_NOTE_MEM_ALLOCATION( bfp );
		}
		varCheckContentMem( &sfp->retval );
	}
	count = vectorCount( qip->stack );
	for ( x = 0; x < count; x++ )
	{
		variant *var = (variant *)vectorDataElementAt( qip->stack, x );
		varCheckContentMem( var );
	}

	vectorCheckMem( qip->stkframecache );
	count = vectorCount(qip->stkframecache);
	for ( x = 0; x < count; x++ )
	{
		stackframe *sf = (stackframe *)vectorElementAt( qip->stkframecache, x );
		DEBUG_NOTE_MEM_ALLOCATION( sf );
		vectorCheckMem( sf->locals );
		vectorCheckMem( sf->blockframes );
	}
}

#endif
