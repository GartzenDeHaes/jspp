#ifdef HAVE_CONFIG_H
#include <jspp/config.h>
#endif
#include <stdlib.h>
#include <spl/debug.h>
#include <jspp/program.h>

program *programConstruct()
{
	program *pp = malloc( sizeof ( program ) );
	if ( NULL == pp )
	{
		return NULL;
	}
	pp->sourcecode = vectorConstruct( sizeof( string *) );
	if ( NULL == pp->sourcecode )
	{
		free( pp );
		return NULL;
	}
	pp->classes = hashtableConstruct();
	if ( NULL == pp->classes )
	{
		vectorDestruct( pp->sourcecode );
		free( pp );
		return NULL;
	}
	pp->lclasses = vectorConstruct( sizeof(jsclass *) );
	if ( NULL == pp->lclasses )
	{
		vectorDestruct( pp->sourcecode );
		hashtableDestruct( pp->classes );
		free( pp );
		return NULL;
	}
	pp->cdata = vectorConstruct( sizeof( variant ) );
	if ( NULL == pp->cdata )
	{
		vectorDestruct( pp->lclasses );
		vectorDestruct( pp->sourcecode );
		hashtableDestruct( pp->classes );
		free( pp );
		return NULL;
	}
	pp->symstack = vectorConstruct( sizeof(symtab *) );
	if ( NULL == pp->symstack )
	{
		vectorDestruct( pp->cdata );
		vectorDestruct( pp->lclasses );
		vectorDestruct( pp->sourcecode );
		hashtableDestruct( pp->classes );
		free( pp );
		return NULL;
	}
	pp->curclass = NULL;
	pp->curmethod = NULL;
	pp->intrinsics = NULL;
	pp->errornum = 0;
	pp->errorline = -1;
	pp->errmsg = NULL;
	pp->isrhs = false;
	pp->isdebuging = false;
	return pp;
}

void programDestruct( program *pp )
{
	int x, count;
	jsclass *clsp;

	programValidateMem( pp );

	count = vectorCount( pp->sourcecode );
	for ( x = 0; x < count; x++ )
	{
		string *sp = (string *)vectorElementAt( pp->sourcecode, x );
		if ( NULL != sp )
		{
			stringDestruct( sp );
		}
	}
	count = vectorCount(pp->lclasses);
	for ( x = 0; x < count; x++ )
	{
		clsp = (jsclass *)vectorElementAt( pp->lclasses, x );
		jsclassDestruct( clsp );
	}
	count = vectorCount(pp->cdata);
	for ( x = 0; x < count; x++ )
	{
		varClear( vectorDataElementAt( pp->cdata, x ) );
	}
	count = vectorCount( pp->symstack );
	for ( x = 0; x < count; x++ )
	{
		int y, sycount;
		symtab *stp = (symtab *)vectorElementAt( pp->symstack, x );
		hashtableDestruct( stp->syms );
		sycount = vectorCount( stp->symlist );
		for ( y = 0; y < sycount; y++ )
		{
			vardef *vdp = (vardef *)vectorElementAt( stp->symlist, y );
			stringDestruct( vdp->name );
			free( vdp );
		}
		vectorDestruct( stp->symlist );
		free( stp );
	}
	if ( NULL != pp->errmsg )
	{
		stringDestruct( pp->errmsg );
	}
	vectorDestruct( pp->symstack );
	vectorDestruct( pp->cdata );
	vectorDestruct( pp->lclasses );
	vectorDestruct( pp->sourcecode );
	hashtableDestruct( pp->classes );
	free( pp );
}

void programSetIntrinsics( program *pp, vector *programs )
{
	programValidateMem( pp );
	vectorValidateMem( programs );
	ASSERT( NULL == pp->intrinsics );
	pp->intrinsics = programs;
}

jsclass *programDefineClass( program *pp, const char *name, const char *basename, void *privateclassdata )
{
	jsclass *jcp = jsclassConstruct( name, basename, privateclassdata );
	if ( NULL == jcp )
	{
		return NULL;
	}
	if (! hashtableAdd( pp->classes, jcp->name, jcp ) )
	{
		jsclassDestruct( jcp );
		return NULL;
	}
	if ( ! vectorAdd( pp->lclasses, &jcp ) )
	{
		hashtableRemove( pp->classes, jcp->name );
		jsclassDestruct( jcp );
		return NULL;		
	}
	pp->curclass = jcp;
	return jcp;
}

jsmethod *programDefineMethod( program *pp, const char *name, const bool isstatic, const enum jsaccess access )
{
	jsmethod *mp;

	programValidateMem( pp );
	ASSERT( pp->curclass != NULL );
	ASSERT( isstatic == 0 || isstatic == 1 );
	jsclassValidateMem( pp->curclass );

	mp = jsmethodConstruct( pp->curclass, name, isstatic, access );
	pp->curmethod = mp;
	return mp;
}

jsmethod *programDefineNativeMethod( program *pp, NATIVEMETHOD meth, const char *name, const bool isstatic, const enum jsaccess access, const bool hasretvalue )
{
	jsmethod *mp;

	programValidateMem( pp );
	ASSERT( pp->curclass != NULL );
	ASSERT( isstatic == 0 || isstatic == 1 );
	jsclassValidateMem( pp->curclass );

	mp = jsclassAddNativeMethod( pp->curclass, meth, name, isstatic, access, hasretvalue );
	pp->curmethod = mp;
	return mp;
}

vardef *programDefineNativeField( program *pp, NATIVEMETHOD fld, const char *name, const bool isstatic, const enum jsaccess access )
{
	vardef *vdp;

	programValidateMem( pp );
	ASSERT( pp->curclass != NULL );
	ASSERT( isstatic == 0 || isstatic == 1 );
	jsclassValidateMem( pp->curclass );

	vdp = jsclassAddNativeField( pp->curclass, fld, name, isstatic, TYPE_VOID, TYPE_VOID );
	return vdp;
}

vardef *programDefineField( program *pp, const char *name, const bool isstatic, const enum jstype typ, const enum jstype subtyp )
{
	vardef *vdp;

	programValidateMem( pp );
	ASSERT( pp->curclass != NULL );
	ASSERT( isstatic == 0 || isstatic == 1 );
	jsclassValidateMem( pp->curclass );

	vdp = jsclassAddField( pp->curclass, name, isstatic, typ, subtyp );
	return vdp;
}

uint16 encodeOp
(
	const enum OpCode op, 
	const int16 argcount, 
	const enum ArgSource mode,
	const enum ArgSource mode2
)
{
	uint16 opcode;
	ASSERT( argcount < 4 );

	opcode = (uint16)(op | argcount << 7 | mode << 9 | mode2 << 12);
	
	ASSERT( (opcode & (1<<0 | 1<<1 | 1<<2 | 1<<3 | 1<<4 | 1<<5 | 1<<6)) == op );
	ASSERT( (opcode & (1<<7|1<<8))>>7 == argcount );
	ASSERT( (opcode & (1<<9|1<<10|1<<11))>>9 == mode );
	ASSERT( (opcode & (1<<12|1<<13|1<<14))>>12 == mode2 );
	
	return opcode;
}

#ifdef DEBUG2
void decodeOp
(
	const uint16 opcode, 
	enum OpCode *op, 
	int16 *argcount,
	enum ArgSource *mode,
	enum ArgSource *mode2
)
{
	*op = (enum OpCode)(opcode & (1<<0 | 1<<1 | 1<<2 | 1<<3 | 1<<4 | 1<<5 | 1<<6));
	/*opcode >>= 7;*/
	*argcount = (int16)((opcode & (1<<0 | 1<<1)<<7)>>7);
	/*opcode >>= 2;*/
	*mode = (enum ArgSource)((opcode & (1<<0 | 1<<1 | 1<<2)<<9)>>9);
	/*opcode >>= 3;*/
	*mode2 = (enum ArgSource)((opcode & (1<<0 | 1<<1 | 1<<2)<<12)>>12);
}
#endif

static int _emit
(
	program *pp, 
	const enum OpCode op, 
	const int16 argcount, 
	const enum ArgSource mode,
	const enum ArgSource mode2
)
{
	int pc = pp->curmethod->pc++;
	uint16 encop;
	programValidateMem( pp );

	if ( NULL == pp->curclass )
	{
		return -1;
	}
	if ( NULL == pp->curmethod )
	{
		return -1;
	}
	jsclassValidateMem( pp->curclass );
	encop = encodeOp( op, argcount, mode, mode2 );
	int16arraySetElement( pp->curmethod->code, encop, pc );
	if ( OP_RET == op )
	{
		pp->curmethod->hasretval = true;
	}
	return pc;
}

int programEmit0( program *pp, const enum OpCode op )
{
	return _emit( pp, op, 0, (enum ArgSource)0, (enum ArgSource)0 );
}

int programEmit1( program *pp, const enum OpCode op, const enum ArgSource destmode )
{
	return _emit( pp, op, 1, destmode, (enum ArgSource)0 );
}

int programEmit2( program *pp, const enum OpCode op, const enum ArgSource mode, const enum ArgSource mode2 )
{
	return _emit( pp, op, 2, mode, mode2 );
}

bool programEmitArgAt( program *pp, const int pc, const int32 arg )
{
	programValidateMem( pp );

	if ( NULL == pp->curclass )
	{
		return false;
	}
	if ( NULL == pp->curmethod )
	{
		return false;
	}
	jsclassValidateMem( pp->curclass );
	return int16arraySet2Element( pp->curmethod->code, arg, pc );
}

bool programFixUpPc( program *pp, const int fixuppcat )
{
	return programEmitArgAt( pp, fixuppcat, pp->curmethod->pc );
}

int programEmitArg( program *pp, const int32 arg )
{
	int pc = pp->curmethod->pc;
	if ( ! programEmitArgAt( pp, pc, arg ) )
	{
		return -1;
	}
	pp->curmethod->pc += 2;
	return pc;
}

int blockDecode
( 
	const struct int16array *code, 
	const int pc, 
	enum OpCode *op, 
	int16 *argcount,
	enum ArgSource *mode,
	enum ArgSource *mode2
)
{
	decodeOp( int16arrayGetElement( code, pc ), op, argcount, mode, mode2 );
	return pc+1;
}

int blockFetchInt32( const struct int16array *code, const int pc, int32 *val )
{
	*val = int16arrayGetInt32( code, pc );
	return pc+2;
}

jsclass *programSearchClass( const vector *vp, const char *clsname )
{
	jsclass *jsp;
	int count = vectorCount( vp );
	int x;
	for ( x = 0; x < count; x++ )
	{
		program *pp = (program *)vectorElementAt(vp, x);
		ASSERT_MEM( pp, sizeof(program) );
		programValidateMem( pp );
		if ( NULL != (jsp = programFindClass( pp, clsname )) )
		{
			return jsp;
		}
	}
	return NULL;
}


jsclass *programFindClass( const program *pp, const char *clsname )
{
	string *key;
	jsclass *jsp;

	programValidateMem( pp );
	key = stringConstructChar( clsname );
	if ( NULL == key )
	{
		return NULL;
	}
	jsp = (jsclass *)hashtableGet( pp->classes, key );
	stringDestruct( key );
	if ( NULL == jsp && NULL != pp->intrinsics )
	{
		return programSearchClass(pp->intrinsics, clsname);
	}
	return jsp;
}

jsclass *programFindClassWithMethod( const program *pp, const char *methodsig, const bool isstatic )
{
	jsclass *jsp;
	jsmethod *jsm;
	int x;

	programValidateMem( pp );

	for ( x = 0; x < vectorCount(pp->lclasses); x++ )
	{
		jsp = (jsclass *)vectorElementAt( pp->lclasses, x );
		jsm = jsclassFindMethod( jsp, methodsig );
		if ( NULL != jsm )
		{
			return jsp;
		}
	}
	/*vectorBeginIter( pp->lclasses );
	//while ( (jsp = (jsclass *)vectorNext( pp->lclasses ) ) != NULL )
	//{
	//	jsmethod *jsm = jsclassFindMethod( jsp, methodsig );
	//	if ( NULL != jsm )
	//	{
	//		return jsp;
	//	}
	//}*/
	return NULL;
}

int programEmitConstInt( program *pp, const int32 val )
{
	variant var;

	programValidateMem( pp );
	memset( &var, 0, sizeof(struct variant) );
	if ( ! varAssignInt( &var, val ) )
	{
		return -1;
	}
	if ( ! vectorAdd( pp->cdata, &var ) )
	{
		return -1;
	}
	return pp->cdata->top - 1;
}

int programEmitConstFloat( program *pp, const float val )
{
	variant var;

	programValidateMem( pp );
	memset( &var, 0, sizeof(struct variant) );
	if ( ! varAssignFloat( &var, val ) )
	{
		return -1;
	}
	if ( ! vectorAdd( pp->cdata, &var ) )
	{
		return -1;
	}
	return pp->cdata->top - 1;
}

int programEmitConstChars( program *pp, const char * val )
{
	variant var;

	programValidateMem( pp );
	memset( &var, 0, sizeof(struct variant) );
	if (! varAssignChar( &var, val ) )
	{
		return -1;
	}
	if ( ! vectorAdd( pp->cdata, &var ) )
	{
		return -1;
	}
	return pp->cdata->top - 1;
}

int programEmitConstChar( program *pp, const char val )
{
	variant var;

	programValidateMem( pp );
	memset( &var, 0, sizeof(struct variant) );
	if (! varAssignCh( &var, val ) )
	{
		return -1;
	}
	if ( ! vectorAdd( pp->cdata, &var ) )
	{
		return -1;
	}
	return pp->cdata->top - 1;
}

#ifdef DEBUG

void programValidateMem( const program *pp )
{
	jsclass *jsp;
	string *str;
	int srclen = vectorCount( pp->sourcecode );
	int x, count;

	ASSERT_MEM( pp, sizeof( program ) );
	hashtableValidateMem( pp->classes );
	vectorValidateMem( pp->lclasses );
	vectorValidateMem( pp->sourcecode );
	vectorValidateMem( pp->cdata );

	count = vectorCount(pp->lclasses);
	for ( x = 0; x < count; x++ )
	{
		jsp = (jsclass *)vectorElementAt( pp->lclasses, x );
		jsclassValidateMem( jsp );
	}
	for ( x = 0; x < srclen; x++ )
	{
		str = (string *)vectorElementAt( pp->sourcecode, x );
		stringValidateMem( str );
	}
	count = vectorCount(pp->cdata);
	for ( x = 0; x < count; x++ )
	{
		varValidateContentMem( vectorDataElementAt( pp->cdata, x ) );
	}
	count = vectorCount( pp->symstack );
	for ( x = 0; x < count; x++ )
	{
		int y;
		int sycount;
		symtab *stp = (symtab *)vectorElementAt( pp->symstack, x );
		hashtableValidateMem( stp->syms );
		sycount = vectorCount( stp->symlist );
		for ( y = 0; y < sycount; y++ )
		{
			vardef *vdp = (vardef *)vectorElementAt( stp->symlist, y );
			ASSERT_MEM( vdp, sizeof(struct vardef) );
			stringValidateMem( vdp->name );
		}
		vectorValidateMem( stp->symlist );
		ASSERT_MEM( stp, sizeof(struct symtab) );
	}
	vectorValidateMem( pp->symstack );
}

void programCheckMem( const program *pp )
{
	jsclass *jsp;
	string *str;
	int srclen = vectorCount( pp->sourcecode );
	int x, count;

	programValidateMem( pp );

	DEBUG_NOTE_MEM_ALLOCATION( pp );
	hashtableCheckMem( pp->classes );
	vectorCheckMem( pp->lclasses );
	vectorCheckMem( pp->sourcecode );
	vectorCheckMem( pp->cdata );
	if ( NULL != pp->errmsg )
	{
		stringCheckMem( pp->errmsg );
	}
	count = vectorCount(pp->lclasses);
	for ( x = 0; x < count; x++ )
	{
		jsp = (jsclass *)vectorElementAt( pp->lclasses, x );
		jsclassCheckMem( jsp );
	}
	for ( x = 0; x < srclen; x++ )
	{
		str = (string *)vectorElementAt( pp->sourcecode, x );
		stringCheckMem( str );
	}
	count = vectorCount(pp->cdata);
	for ( x = 0; x < count; x++ )
	{
		varCheckMem( vectorDataElementAt( pp->cdata, x ) );
	}
	count = vectorCount( pp->symstack );
	for ( x = 0; x < count; x++ )
	{
		int y, sycount;
		symtab *stp = (symtab *)vectorElementAt( pp->symstack, x );
		hashtableCheckMem( stp->syms );
		sycount = vectorCount( stp->symlist );
		for ( y = 0; y < sycount; y++ )
		{
			vardef *vdp = (vardef *)vectorElementAt( stp->symlist, y );
			DEBUG_NOTE_MEM_ALLOCATION( vdp );
			stringCheckMem( vdp->name );
		}
		vectorCheckMem( stp->symlist );
		DEBUG_NOTE_MEM_ALLOCATION( stp );
	}
	vectorCheckMem( pp->symstack );
}

#endif
