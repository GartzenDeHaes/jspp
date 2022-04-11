#ifdef HAVE_CONFIG_H
#include <jspp/config.h>
#endif
#include <spl/debug.h>
#include <jspp/interp.h>
#include <jspp/native.h>

enum InterpErrors _interp( qinterp *qip );

#ifdef DEBUG2
static variant *stackPeek0( const qinterp *qip )
{
	ASSERT( vectorCount(qip->stack) > 0 );
	return (variant *)vectorDataPeek( qip->stack );
}
static variant *stackPeek1( const qinterp *qip )
{
	ASSERT( vectorCount(qip->stack) > 1 );
	return (variant *)vectorDataElementAt( qip->stack, vectorCount(qip->stack)-2 );
}
#else
#define stackPeek0(qip) ((variant *)vectorDataPeek( qip->stack ))
#define stackPeek1(qip) ((variant *)vectorDataElementAt( qip->stack, vectorCount(qip->stack)-2 ))
#endif

static variant *stackPeekN( const qinterp *qip, const int n )
{
	ASSERT( vectorCount(qip->stack) > n );
	return (variant *)vectorDataElementAt( qip->stack, (vectorCount(qip->stack)-1)-n );
}

#ifdef DEBUG2
static void stackPop( qinterp *qip )
{
	ASSERT( vectorCount(qip->stack) > 0 );
	varValidateContentMem( (variant *)vectorDataPeek( qip->stack ) );
	varClear( (variant *)vectorDataPop( qip->stack ) );
}
#else
#define stackPop(qip) varClear( (variant *)vectorDataPop( (qip)->stack ) )
#endif
bool stackPush( qinterp *qip, variant *var )
{
	if ( TYPE_INST > var->type )
	{
		if ( ! vectorAdd( qip->stack, var ) )
		{
			return false;
		}
		return true;
	}
	else
	{
		variant v;
		v.type = 0;

		if ( ! vectorAdd( qip->stack, &v ) )
		{
			return false;
		}
		return varAssign( (variant *)vectorDataPeek( qip->stack ), var );
	}
}

static jsinst *stackGetThis( const qinterp *qip )
{
	variant *var = vectorDataElementAt(qip->curframe->locals, 0);
	ASSERT ( NULL != var );
	varValidateMem( var );
	if ( TYPE_PTR == var->type )
	{
		var = var->data.vp;
	}
	ASSERT ( TYPE_INST == var->type );
	return var->data.ip;
}

static bool enterFrame( qinterp *qip, jsclass *jsp )
{
	stackframe *sfp;

	if ( NULL == (sfp = (stackframe *)vectorPop(qip->stkframecache)) )
	{
		sfp = malloc( sizeof(struct stackframe) );
		memset( sfp, 0, sizeof(struct stackframe) );
	}
	if ( NULL == sfp )
	{
		return false;
	}
#ifdef DEBUG
	if ( NULL != qip->curframe )
	{
		ASSERT( qip->curframe == vectorPeek( qip->frames ) );
	}
#endif
	if ( ! vectorAdd( qip->frames, &sfp ) )
	{
		free( sfp );
		return 0;
	}
	if ( NULL == sfp->locals )
	{
		sfp->locals = vectorConstruct( sizeof( variant ) );
		if ( NULL == sfp->locals )
		{
			vectorPop( qip->frames );
			free( sfp );
			return false;
		}
		sfp->blockframes = vectorConstruct( sizeof (struct blockframe) );
		if ( NULL == sfp->blockframes )
		{
			vectorDestruct( sfp->locals );
			vectorPop( qip->frames );
			free( sfp );
			return false;
		}
	}
#ifdef DEBUG
	else
	{
		vectorValidateMem( sfp->locals );
		vectorValidateMem( sfp->blockframes );
		ASSERT( 0 == vectorCount(sfp->locals) );
		ASSERT( 0 == vectorCount(sfp->blockframes) );
	}
#endif
	sfp->ctx = jsp;
	sfp->curmethod = NULL;
	sfp->stackcount = vectorCount( qip->stack );
	sfp->retval.type = TYPE_VOID;
	sfp->err = IERR_NONE;

	qip->curframe = sfp;

	return true;
}

static void leaveFrame( qinterp *qip, bool nopop )
{
	int x;
	int count;
	variant *vloc;

	ASSERT(qip->curframe->curmethod == NULL || ((nopop && !qip->curframe->curmethod->hasretval) || (!nopop)) );
	ASSERT( qip->curframe == vectorPeek( qip->frames ) );
	ASSERT_MEM( qip->curframe, sizeof(struct stackframe) );
	
	if ( nopop )
	{
		vectorClear( qip->curframe->locals );
	}
	else
	{
		count = vectorCount( qip->curframe->locals );
		for ( x = 0; x < count; x++ )
		{
			/*vardef *vdp = jsmethodGetArg( qip->curframe->curmethod, x );*/
			vloc = (variant *)vectorDataPop( qip->curframe->locals );
			VARCLEAR( vloc );
		}
	}
	ASSERT( 0 == vectorCount(qip->curframe->locals) );

	vectorClear( qip->curframe->blockframes );

	if ( NULL != qip->curframe->curmethod && ! nopop )
	{
		count = qip->curframe->stackcount - getArgumentCount( qip );
		while ( vectorCount(qip->stack) > count )
		{
			vloc = (variant *)vectorDataPop( qip->stack );
			VARCLEAR(vloc);
		}
	}
	/*
	 *  qip->curframe->curmethod can be null when a base class
	 *  doesn't have a constructor defined.
	 */
	if ( (NULL != qip->curframe->curmethod && qip->curframe->curmethod->hasretval) || IERR_EXCEPTION == qip->curframe->err )
	{
		if ( qip->curframe->err == IERR_NONE || qip->curframe->err == IERR_EXCEPTION )
		{
			ASSERT ( TYPE_VOID != qip->curframe->retval.type );
			stackPush( qip, &qip->curframe->retval );
			VARCLEAR( &qip->curframe->retval );
		}
	}
	vectorAdd( qip->stkframecache, &qip->curframe );

	vectorPop( qip->frames );
	qip->curframe = (stackframe *)vectorPeek( qip->frames );
}

static void setBreak( stackframe *sfp, const int breakpc )
{
	blockframe *bfp = (blockframe *)vectorDataPeek( sfp->blockframes );
	ASSERT( NULL != bfp );
	bfp->blockcount = vectorCount( sfp->blockframes );
	bfp->breakpc = breakpc;
}

static int getBreak( const stackframe *sfp )
{
	blockframe *bfp = (blockframe *)vectorDataPeek( sfp->blockframes );
	if ( NULL == bfp )
	{
		return -1;
	}
	return bfp->breakpc;
}

static int getBreakBlockCount( const stackframe *sfp )
{
	blockframe *bfp = (blockframe *)vectorDataPeek( sfp->blockframes );
	if ( NULL == bfp )
	{
		return 0;
	}
	return bfp->blockcount;
}

static int localCount( const stackframe *sfp )
{
	blockframe *bfp = (blockframe *)vectorDataPeek( sfp->blockframes );
	if ( NULL == bfp )
	{
		return 0;
	}
	return bfp->localcount;
}

static bool enterBlock( stackframe *sfp, const int localcount )
{
	int x;
	variant var;
	blockframe bf;

	ASSERT_MEM( sfp, sizeof( stackframe ) );
	var.type = TYPE_VOID;

	for ( x = 0; x < localcount; x++ )
	{
		if ( ! vectorAdd( sfp->locals, &var ) )
		{
			return false;
		}
	}
	bf.catchpc = -1;
	bf.localcount = localcount;
	bf.breakpc = getBreak( sfp );
	bf.blockcount = getBreakBlockCount( sfp );
	vectorAdd( sfp->blockframes, &bf );
	return true;
}

static void leaveBlock( stackframe *sfp )
{
	int x;
	int localcount = localCount( sfp );

	for ( x = 0; x < localcount; x++ )
	{
		varClear( (variant *)vectorDataPop( sfp->locals ) );
	}
	vectorPop( sfp->blockframes );
}

static void setTry( stackframe *sfp, const int catchpc )
{
	blockframe *bfp = (blockframe *)vectorDataPeek( sfp->blockframes );
	ASSERT ( NULL != bfp );
	bfp->catchpc = catchpc;
}

static int getCatch( stackframe *sfp )
{
	blockframe *bfp;
	
	while ( NULL !=  (bfp = (blockframe *)vectorDataPeek( sfp->blockframes )) )
	{
		if ( -1 != bfp->catchpc )
		{
			return bfp->catchpc;
		}
		leaveBlock( sfp );
	}
	return -1;
}

static int breakBlock( stackframe *sfp )
{
	int pc;
	int blockcount;
	blockframe *bfp = (blockframe *)vectorDataPeek( sfp->blockframes );
	if ( NULL == bfp )
	{
		return -1;
	}
	pc = bfp->breakpc;
	blockcount = bfp->blockcount;
	while ( blockcount > vectorCount( sfp->blockframes ) )
	{
		leaveBlock( sfp );
	}
	return pc;
}

enum InterpErrors callStaticMethod( qinterp *qip, const string *fqname )
{
	string *sig;
	string *clsname;
	jsclass *jsp;
	jsmethod *mp;
	int dotpos;
	int argcount;
	enum InterpErrors err;

#ifdef DEBUG
	int stackcount;
	int framecount = vectorCount( qip->frames );
#endif

	/*qinterpValidateMem( qip );*/

	dotpos = stringIndexOf(fqname, ".");
	clsname = stringLeft( fqname, dotpos );
	if ( NULL == clsname )
	{
		return IERR_OUT_OF_MEM;
	}
	jsp = (jsclass *)hashtableGet( qip->classes, clsname );
	stringDestruct( clsname );

	if ( NULL == jsp )
	{
		return IERR_MAIN_CLASS_NOT_FOUND;
	}
	if ( ! enterFrame( qip, jsp ) )
	{
		return IERR_OUT_OF_MEM;
	}
	sig = stringSubstringEnd( fqname, dotpos + 1 );
	mp = jsclassFindMethod( qip->curframe->ctx, sig->cstr );
	qip->curframe->curmethod = mp;
	stringDestruct( sig );

	if ( NULL == qip->curframe->curmethod )
	{
		leaveFrame( qip, false );
		return IERR_UNKNOWN_METHOD;
	}
	argcount = vectorCount(qip->curframe->curmethod->args);
	ASSERT( argcount <= vectorCount( qip->stack ) );

	/* enterBlock is called for zero locals, since the locals are */
	/* pushed on the stack a few lines down.*/
	if ( ! enterBlock( qip->curframe, 0 ) )
	{
		leaveFrame( qip, false );
		return IERR_OUT_OF_MEM;
	}
	if ( 0 != argcount )
	{
		/* push the method args ( 'this' should be index zero for non-static call )*/
		int x;
		variant *var;
		int start = vectorCount(qip->curframe->locals);

		for ( x = argcount-1; x >=0 ; x-- )
		{
			vardef *vdp = jsmethodGetArg( qip->curframe->curmethod, x );
			if ( NULL == vdp )
			{
				return IERR_INTERNAL_ERROR1;
			}
			var = (variant *)vectorDataPop( qip->stack );
			varValidateMem( var );
			if ( ! vdp->isptr )
			{
				VARDEREF( var );
			}
			/* outer block now owns the arg variant's data */
			vectorSetElementAt( qip->curframe->locals, var, start + x );
		}
	}
#ifdef DEBUG
	stackcount = vectorCount( qip->stack );
#endif
	/* exec byte code */
	
	if ( qip->curframe->curmethod->isnative )
	{
		varClear( &qip->curframe->retval );
		err = (*qip->curframe->curmethod->nativemethod)(qip, qip->curframe->curmethod, &qip->curframe->retval);
	}
	else
	{
		err = _interp( qip );
	}
	
	leaveBlock( qip->curframe );
	leaveFrame( qip, false );

#if defined(DEBUG) || defined(_DEBUG)
	if ( err == IERR_NONE || IERR_EXCEPTION == err )
	{
		ASSERT( (mp->hasretval || IERR_EXCEPTION == err) ? stackcount+1 == vectorCount(qip->stack) : stackcount == vectorCount(qip->stack) );
		ASSERT( framecount == vectorCount(qip->frames) );
	}
	else
	{
		ASSERT( vectorCount( qip->stack ) == 0 );
	}
#endif
	return err;
}

enum InterpErrors callInstMethod
( 
	qinterp *qip, 
	jsinst *inst, 
	const string *mname,
	const bool nopop
)
{
	int argcount;
	enum InterpErrors err;
	jsmethod *mp;
#ifdef DEBUG
	int stackcount;
	int framecount = vectorCount( qip->frames );
#endif

	jsinstValidateMem( inst );
	stringValidateMem( mname );

	if ( ! enterFrame( qip, inst->myclass ) )
	{
		vectorPop( qip->stack );
		return IERR_OUT_OF_MEM;
	}
	ASSERT( inst->myclass == qip->curframe->ctx );
	mp = jsclassFindMethod( qip->curframe->ctx, mname->cstr );
	qip->curframe->curmethod = mp;
	if ( NULL == qip->curframe->curmethod )
	{
		if ( ! nopop )
		{
			vectorPop( qip->stack );
		}
		leaveFrame( qip, nopop );
		return IERR_UNKNOWN_METHOD;
	}
	argcount = vectorCount(qip->curframe->curmethod->args);
	ASSERT( argcount-1 <= vectorCount( qip->stack ) );

	/* enterBlock is called for zero locals, since the locals are*/
	/* pushed off the stack a few lines down.*/
	if ( ! enterBlock( qip->curframe, 0 ) )
	{
		leaveFrame( qip, nopop );
		return IERR_OUT_OF_MEM;
	}
	ASSERT( qip->curframe->curmethod->isstatic || (!qip->curframe->curmethod->isstatic && argcount > 0) );

	if ( argcount > 0 )
	{
		/* push the method args ( 'this' should be index zero for non-static call )*/
		int stkpos = 0;
		int x;
		variant *var;
		int start = vectorCount(qip->curframe->locals);

		for ( x = argcount-1; x >= 0; x-- )
		{
			vardef *vdp = jsmethodGetArg( qip->curframe->curmethod, x );
			if ( NULL == vdp )
			{
				return IERR_INTERNAL_ERROR1;
			}
			if ( nopop )
			{
				++stkpos;
				var = (variant *)vectorDataElementAt( qip->stack, vectorCount(qip->stack) - stkpos );
			}
			else
			{
				var = (variant *)vectorDataPop( qip->stack );
			}
			varValidateMem( var );
			if ( ! vdp->isptr )
			{
				VARDEREF( var );
			}
			/* outer block now owns the arg variant's data*/
			vectorSetElementAt(qip->curframe->locals, var, start + x );
		}
	}
#ifdef DEBUG
	stackcount = vectorCount( qip->stack );
#endif
	/* exec byte code */
	
	if ( qip->curframe->curmethod->isnative )
	{
		varClear( &qip->curframe->retval );
		err = (*qip->curframe->curmethod->nativemethod)(qip, qip->curframe->curmethod, &qip->curframe->retval);
	}
	else
	{
		err = _interp( qip );
	}
	leaveBlock( qip->curframe );
	leaveFrame( qip, nopop );

#ifdef DEBUG
	if ( nopop )
	{
		ASSERT( ((!mp->hasretval) && stackcount == vectorCount(qip->stack)) || (err == IERR_EXCEPTION && stackcount+1 == vectorCount(qip->stack)) );
	}
	else
	{
		ASSERT( (mp->hasretval || IERR_EXCEPTION == err) ? stackcount+1 == vectorCount(qip->stack) : stackcount == vectorCount(qip->stack) );
	}
	ASSERT( framecount == vectorCount(qip->frames) );
#endif
	return err;
}

enum InterpErrors callBaseMethod( qinterp *qip, jsinst *inst, const string *mname )
{
	int argcount;
	enum InterpErrors err;
	jsmethod *mp;
#ifdef DEBUG
	int stackcount;
	int framecount = vectorCount( qip->frames );
#endif

	jsinstValidateMem( inst );
	stringValidateMem( mname );

	if ( NULL == inst->myclass->baseclass )
	{
		return callInstMethod( qip, inst, mname, false );
	}
	jsclassValidateMem( inst->myclass->baseclass );
	if ( ! enterFrame( qip, inst->myclass->baseclass ) )
	{
		vectorPop( qip->stack );
		return IERR_OUT_OF_MEM;
	}
	ASSERT( inst->myclass->baseclass == qip->curframe->ctx );
	mp = jsclassFindMethod( qip->curframe->ctx, mname->cstr );
	qip->curframe->curmethod = mp;
	if ( NULL == qip->curframe->curmethod )
	{
		vectorPop( qip->stack );
		leaveFrame( qip, false );
		return IERR_UNKNOWN_METHOD;
	}
	argcount = vectorCount(qip->curframe->curmethod->args);
	ASSERT( argcount-1 <= vectorCount( qip->stack ) );

	/* enterBlock is called for zero locals, since the locals are */
	/* pushed of the stack a few lines down. */
	if ( ! enterBlock( qip->curframe, 0 ) )
	{
		leaveFrame( qip, false );
		return IERR_OUT_OF_MEM;
	}
	ASSERT( qip->curframe->curmethod->isstatic || (!qip->curframe->curmethod->isstatic && argcount > 0) );

	if ( argcount > 0 )
	{
		/* push the method args ( 'this' should be index zero for non-static call )*/
		int x;
		variant *var;
		int start = vectorCount(qip->curframe->locals);

		for ( x = argcount-1; x >= 0; x-- )
		{
			vardef *vdp = jsmethodGetArg( qip->curframe->curmethod, x );
			if ( NULL == vdp )
			{
				return IERR_INTERNAL_ERROR1;
			}
			var = (variant *)vectorDataPop( qip->stack );
			varValidateMem( var );
			if ( ! vdp->isptr )
			{
				VARDEREF( var );
			}
			/* outer block now owns the arg variant's data */
			vectorSetElementAt(qip->curframe->locals, var, start + x );
		}
	}
#ifdef DEBUG
	stackcount = vectorCount( qip->stack );
#endif

	/* exec byte code */
	
	if ( qip->curframe->curmethod->isnative )
	{
		varClear( &qip->curframe->retval );
		err = (*qip->curframe->curmethod->nativemethod)(qip, qip->curframe->curmethod, &qip->curframe->retval);
	}
	else
	{
		err = _interp( qip );
	}
	
	leaveBlock( qip->curframe );
	leaveFrame( qip, false );

	ASSERT( (mp->hasretval || IERR_EXCEPTION == err) ? stackcount+1 == vectorCount(qip->stack) : stackcount == vectorCount(qip->stack) );
	ASSERT( framecount == vectorCount(qip->frames) );

	return err;
}

bool callInternalConstructors( qinterp *qip, jsclass *jsp, jsinst *inst )
{
	variant *var;
	int argcount;
	enum InterpErrors err;
	bool boostRefCount = true;

	if ( NULL != jsp->baseclass )
	{
		if ( ! callInternalConstructors( qip, jsp->baseclass, inst ) )
		{
			return false;
		}
		boostRefCount = false;
		ASSERT( 1 == inst->refcount );
	}

	/*
	 * setup internal constructor call.
	 */
	varAssignInst( &qip->tempdata1, inst );
	stackPush( qip, &qip->tempdata1 );
	varClear( &qip->tempdata1 );
	if ( boostRefCount )
	{
		jsinstRef( inst );
	}
	ASSERT( 2 == inst->refcount );
	
	jsinstValidateMem( inst );

	if ( ! enterFrame( qip, jsp ) )
	{
		vectorPop( qip->stack );
		return IERR_OUT_OF_MEM;
	}
	qip->curframe->curmethod = jsclassFindMethod( qip->curframe->ctx, "@@constructor:Ivv" );
	if ( NULL == qip->curframe->curmethod )
	{
		vectorPop( qip->stack );
		leaveFrame( qip, false );
		return false;
	}
	argcount = vectorCount(qip->curframe->curmethod->args);
	ASSERT( argcount-1 <= vectorCount( qip->stack ) );
	ASSERT( 1 == argcount );

	/* enterBlock is called for zero locals, since the locals are */
	/* pushed of the stack a few lines down. */
	if ( ! enterBlock( qip->curframe, 0 ) )
	{
		vectorPop( qip->stack );
		leaveFrame( qip, false );
		return false;
	}
	ASSERT( qip->curframe->curmethod->isstatic || (!qip->curframe->curmethod->isstatic && argcount > 0) );

	var = (variant *)vectorDataPop( qip->stack );
	varValidateMem( var );
	VARDEREF( var );

	/* outer block now owns the arg variant's data */
	vectorAdd(qip->curframe->locals, var );

	/* exec byte code */
	
	if ( qip->curframe->curmethod->isnative )
	{
		err = (*qip->curframe->curmethod->nativemethod)(qip, qip->curframe->curmethod, &qip->tempdata1);
	}
	else
	{
		err = _interp( qip );
	}
	leaveBlock( qip->curframe );
	leaveFrame( qip, false );
	
	if ( IERR_NONE != err )
	{
		return false;
	}
	ASSERT( 1 == inst->refcount );
	return true;
}

enum InterpErrors callConstructors
( 
	qinterp *qip, 
	jsclass *jsp,
	jsinst *inst, 
	int argcount
)
{
	enum InterpErrors err;
	string *sig;
	int instcount = inst->refcount;

	ASSERT( argcount >= 0 );
	ASSERT( 1 <= instcount );

	jsclassValidateMem( jsp );
	if ( NULL != jsp->baseclass )
	{
		if ( (err = callConstructors( qip, jsp->baseclass, inst, argcount )) != IERR_NONE )
		{
			return err;
		}
		ASSERT( instcount == inst->refcount );
	}
	sig = stringConstructDup( jsp->name );
	if ( NULL == sig )
	{
		return IERR_OUT_OF_MEM;
	}
	if ( ! stringAppendChar( sig, ":Ivv" ) )
	{
		return IERR_OUT_OF_MEM;
	}
	if ( ! stringAppendChars( sig, 'v', argcount ) )
	{
		return IERR_OUT_OF_MEM;
	}

	/*
	 *  make the call
	 */
	jsinstRef( inst );
	ASSERT( 2 <= inst->refcount );
	err = callInstMethod( qip, inst, sig, true );
	stringDestruct( sig );

	jsinstDeref( inst );
	ASSERT( inst->refcount == instcount );

	if ( ! (err == IERR_NONE || (argcount == 0 && err == IERR_UNKNOWN_METHOD)) )
	{
		if ( jsp != inst->myclass && argcount != 0 )
		{
			/* try calling the default base constructor */
			return callConstructors( qip, jsp, inst, 0 );
		}
		ASSERT( instcount == inst->refcount );
		return err;
	}
	if ( jsp == inst->myclass )
	{
		/*
		*  clear the arguments, leave this
		*/
		while ( argcount-- > 0 )
		{
			stackPop( qip );
		}
	}
	ASSERT( instcount == inst->refcount );
	return IERR_NONE;
}

static variant *qinterpFindField( qinterp *qip, variant *pthis, const variant *fname )
{
	jsclass *jsp;
	vardef *vdp;

	if ( TYPE_PTR == pthis->type )
	{
		pthis = pthis->data.vp;
	}
	ASSERT( TYPE_INST == pthis->type );
	ASSERT( TYPE_STRING == fname->type );

	jsp = pthis->data.ip->myclass;
	vdp = jsclassFindField( jsp, fname->data.sp->cstr );
	if ( NULL == vdp )
	{
		return NULL;
	}
	if ( vdp->isnative )
	{
		if ( (*vdp->nativefield)( qip, vdp, &qip->tempdata1) != IERR_NONE )
		{
			return NULL;
		}
		return &qip->tempdata1;
	}
	else
	{
		if ( vdp->isstatic )
		{
			return (variant *)vectorElementAt( jsp->staticlocations, vdp->offset );
		}
		else
		{
			return vectorDataElementAt( pthis->data.ip->fields, vdp->offset );
		}
	}
#ifdef _WIN32
	return NULL;
#endif
}

static enum InterpErrors qinterpCreateJSClassInstance
( 
	qinterp *qip, 
	const string *classname, 
	jsinst **inst
)
{
	jsclass *jsp;

	jsp = qinterpFindClass( qip, classname );
	if ( NULL == jsp )
	{
		return IERR_CLASS_NOT_FOUND;
	}
	*inst = jsinstConstruct( jsp );
	if ( NULL == *inst )
	{
		return IERR_OUT_OF_MEM;
	}
	if ( ! callInternalConstructors( qip, jsp, *inst ) )
	{
		return IERR_INTERNAL_ERROR2;
	}

	ASSERT( 1 == (*inst)->refcount );
	/*if ( (err = callClassConstructors( qip, jsp, *inst, argc, false )) != IERR_NONE )
	//{
	//	return err;
	//}
	//ASSERT( 1 == (*inst)->refcount );*/

	return IERR_NONE;
}

enum InterpErrors _interp( qinterp *qip )
{
	struct int16array *code;
	int *pc;
	int codelen;
	enum OpCode op;
	enum ArgSource mode;
	enum ArgSource mode2;
	int16 argcount;
	int32 arg1, arg2;
	variant *lval1;
	variant *lval2;
	jsclass *jsp;

	int stackpos = vectorCount( qip->stack ) + qip->curframe->curmethod->hasretval;
	int framepos = vectorCount( qip->frames );

	ASSERT( IERR_NONE == qip->curframe->err );

	code = qip->curframe->curmethod->code;
	codelen = jsmethodCodeLen( qip->curframe->curmethod );/*int16arrayCapacity( code );*/
	pc = &qip->curframe->pc;
	*pc = 0;

	qinterpValidateMem( qip );
	int16arrayValidateMem( code );
	ASSERT_PTR( pc );

	while ( *pc < codelen )
	{
		if ( IERR_NONE != qip->curframe->err )
		{
			break;
		}
		decodeOp( int16arrayGetElement( code, *pc ), &op, &argcount, &mode, &mode2 );
		(*pc)++;
		ASSERT( argcount < 3 );

		if ( argcount > 0 )
		{
			arg1 = int16arrayGetInt32( code, *pc );
			*pc += 2;

			if ( argcount > 1 )
			{
				arg2 = int16arrayGetInt32( code, *pc );
				*pc += 2;
			}
		}
		switch ( op )
		{
		case OP_NOP:
			break;

		case OP_NULL:
			ASSERT( argcount == 0 );
			if ( ! stackPush( qip, &qip->null ) )
			{
				qip->curframe->err = IERR_OUT_OF_MEM;
			}
			break;

		case OP_NEW:
			{
				jsinst *inst;

				ASSERT( argcount == 1 );
				/*ASSERT( MODE_IM == mode2 );*/

				/* class name */
				lval1 = qinterpResolve( qip, mode, arg1, &qip->tempdata1 );
				ASSERT( TYPE_STRING == lval1->type );

				qip->curframe->err = qinterpCreateJSClassInstance( qip, lval1->data.sp, &inst );
				if ( IERR_NONE != qip->curframe->err )
				{
					if ( IERR_EXCEPTION == qip->curframe->err )
					{
						*pc = getCatch( qip->curframe );
						if ( -1 == *pc )
						{
							VARDEREF(stackPeek0(qip));
							varAssign( &qip->curframe->retval, stackPeek0(qip) );
							leaveBlock( qip->curframe );
							qip->curframe->err = IERR_EXCEPTION;
						}
						else
						{
							qip->curframe->err = IERR_NONE;
						}
						break;
					}
					else
					{
						/*return err;*/
						break;
					}
				}
				/*
				* leave "this" on the stack
				*/
				varAssignInst( &qip->tempdata1, inst );
				if ( ! stackPush( qip, &qip->tempdata1 ) )
				{
					qip->curframe->err = IERR_OUT_OF_MEM;
					break;
				}
				ASSERT( stackPeek0( qip )->type == TYPE_INST );
				ASSERT( stackPeek0( qip )->data.ip == inst );
				varClear( &qip->tempdata1 );
				jsinstDeref( inst );
				ASSERT( 1 == inst->refcount );
			}
			break;

		case OP_RSHIFT:
			lval1 = stackPeek0(qip);
			lval2 = stackPeek1(qip);
			if ( ! varAssignOpRightShift( &qip->tempdata1, lval2, lval1 ) )
			{
				qip->curframe->err = IERR_TYPE_MISMATCH;
				break;
			}
			stackPop( qip );
			stackPop( qip );
			stackPush( qip, &qip->tempdata1 );
			varClear( &qip->tempdata1 );
			break;

		case OP_LSHIFT:
			lval1 = stackPeek0(qip);
			lval2 = stackPeek1(qip);
			if ( ! varAssignOpRightShift( &qip->tempdata1, lval2, lval1 ) )
			{
				qip->curframe->err = IERR_TYPE_MISMATCH;
				break;
			}
			stackPop( qip );
			stackPop( qip );
			stackPush( qip, &qip->tempdata1 );
			varClear( &qip->tempdata1 );
			break;

		case OP_MULT:
			lval1 = stackPeek0(qip);
			lval2 = stackPeek1(qip);
			if ( ! varAssignOpMul( &qip->tempdata1, lval2, lval1 ) )
			{
				qip->curframe->err = IERR_TYPE_MISMATCH;
				break;
			}
			stackPop( qip );
			stackPop( qip );
			stackPush( qip, &qip->tempdata1 );
			varClear( &qip->tempdata1 );
			break;

		case OP_DIV:
			ASSERT( argcount == 0 );
			lval1 = stackPeek0(qip);
			lval2 = stackPeek1(qip);
			if ( ! varAssignOpDiv( &qip->tempdata1, lval2, lval1 ) )
			{
				qip->curframe->err = IERR_TYPE_MISMATCH;
				break;
			}
			stackPop( qip );
			stackPop( qip );
			stackPush( qip, &qip->tempdata1 );
			varClear( &qip->tempdata1 );
			break;

		case OP_ADD:
			ASSERT( argcount == 0 );
			lval1 = stackPeek0(qip);
			lval2 = stackPeek1(qip);
			if ( ! varAssignOpPlus( &qip->tempdata1, lval2, lval1 ) )
			{
				qip->curframe->err = IERR_TYPE_MISMATCH;
				break;
			}
			stackPop( qip );
			stackPop( qip );
			stackPush( qip, &qip->tempdata1 );
			varClear( &qip->tempdata1 );
			break;

		case OP_SUB:
			ASSERT( argcount == 0 );
			lval1 = stackPeek0(qip);
			lval2 = stackPeek1(qip);
			if ( ! varAssignOpSub( &qip->tempdata1, lval2, lval1 ) )
			{
				qip->curframe->err = IERR_TYPE_MISMATCH;
				break;
			}
			stackPop( qip );
			stackPop( qip );
			stackPush( qip, &qip->tempdata1 );
			varClear( &qip->tempdata1 );
			break;

		case OP_NEGATE:
			ASSERT( argcount == 0 );
			lval1 = stackPeek0(qip);
			if ( ! varNegate( lval1 ) )
			{
				qip->curframe->err = IERR_TYPE_MISMATCH;
				break;
			}
			break;

		case OP_VRET:
			ASSERT( argcount == 0 );
			qip->curframe->err = IERR_RETURN;
			break;

		case OP_RET:
			ASSERT( argcount == 0 );
			VARDEREF(stackPeek0(qip));
			varAssign( &qip->curframe->retval, stackPeek0(qip) );
			stackPop( qip );
			leaveBlock( qip->curframe );
			qip->curframe->err = IERR_RETURN;
			break;

		case OP_TRY:
			ASSERT( argcount == 1 );
			lval1 = qinterpResolve( qip, mode, arg1, &qip->tempdata1 );
			ASSERT( TYPE_INT == lval1->type );
			setTry( qip->curframe, lval1->data.i );
			break;

		case OP_THROW:
			ASSERT( argcount == 0 );

			*pc = getCatch( qip->curframe );
			if ( -1 == *pc )
			{
				VARDEREF(stackPeek0(qip));
				varAssign( &qip->curframe->retval, stackPeek0(qip) );
				leaveBlock( qip->curframe );
				qip->curframe->err = IERR_EXCEPTION;
				break;
			}
			else
			{
				qip->curframe->err = IERR_NONE;
			}
			break;

		case OP_ENTER:
			ASSERT( 1 == argcount );
			ASSERT( MODE_IM == mode );
			/*lval1 = qinterpResolve( qip, mode, arg1, &qip->tempdata1 );
			//ASSERT( TYPE_INT == lval1->type );
			//if ( ! enterBlock( qip->curframe, lval1->data.i ) )*/
			if ( ! enterBlock( qip->curframe, arg1 ) )
			{
				qip->curframe->err = IERR_OUT_OF_MEM;
				break;
			}
			break;
		
		case OP_LEAVE:
			ASSERT( 0 == argcount );
			leaveBlock( qip->curframe );
			break;

		case OP_SETBREAK:
			ASSERT( 1 == argcount );
			lval1 = qinterpResolve( qip, mode, arg1, &qip->tempdata1 );
			ASSERT( TYPE_INT == lval1->type );
			setBreak( qip->curframe, lval1->data.i );
			break;

		case OP_BREAK:
			ASSERT( 0 == argcount );
			*pc = breakBlock( qip->curframe );
			if ( -1 == *pc )
			{
				qip->curframe->err = IERR_BAD_BREAK;
				break;
			}
			break;

		case OP_CONTINUE:
			ASSERT( 0 == argcount );
			arg1 = breakBlock( qip->curframe );
			if ( -1 == arg1 )
			{
				qip->curframe->err = IERR_BAD_CONTINUE;
				break;
			}
			/* we cheat here, since the continue JMP should be the previous
			 * instruction from the break target */
			*pc = arg1-3;
			break;

		case OP_PUSH:
			ASSERT( argcount == 1 );
			lval1 = qinterpResolve( qip, mode, arg1, &qip->tempdata1 );
			if ( ! stackPush( qip, lval1 ) )
			{
				qip->curframe->err = IERR_OUT_OF_MEM;
				break;
			}
			break;

		case OP_DUP:
			ASSERT( argcount == 0 );
			VARCLEAR( &qip->tempdata1 );
			if ( ! stackPush( qip, &qip->tempdata1 ) )
			{
				qip->curframe->err = IERR_OUT_OF_MEM;
				break;
			}
			if ( ! varAssign( stackPeek0( qip ), stackPeek1( qip ) ) )
			{
				qip->curframe->err = IERR_OUT_OF_MEM;
				break;
			}
			break;

		case OP_ASSIGN:
			ASSERT( argcount == 0 );
			lval1 = stackPeek0( qip );	/* value */
			lval2 = stackPeek1( qip );	/* lvalue */
			ASSERT( TYPE_PTR == lval2->type );
			if( TYPE_PTR == lval1->type )
			{
				varValidateMem( lval1 );
				lval1 = lval1->data.vp;
				varValidateMem( lval1 );
			}
			if ( ! varAssign( lval2->data.vp, lval1 ) )
			{
				qip->curframe->err = IERR_OUT_OF_MEM;
				break;
			}
			stackPop( qip );
			stackPop( qip );
			break;

		case OP_THIS:
			ASSERT( argcount == 0 );
			if ( ! varAssignInst( &qip->tempdata1, stackGetThis( qip ) ) )
			{
				qip->curframe->err = IERR_CALL_REQUIRES_THIS;
				break;
			}
			if ( ! stackPush( qip, &qip->tempdata1 ) )
			{
				qip->curframe->err = IERR_OUT_OF_MEM;
				break;
			}
			VARCLEAR( &qip->tempdata1 );
			break;

		case OP_LVALUE:
			ASSERT( argcount == 1 );
			ASSERT( mode != MODE_IM );
			lval1 = qinterpResolve( qip, mode, arg1, NULL );
			ASSERT( TYPE_PTR != lval1->type );
			VARCLEAR( &qip->tempdata1 );
			stackPush( qip, &qip->tempdata1 );
			varRef( stackPeek0( qip ), lval1 );
			break;

		case OP_RVALUE:
			ASSERT( argcount == 0 );
			lval1 = stackPeek0( qip );
			VARDEREF( lval1 );
			break;

		case OP_SDEREF:
			ASSERT( argcount == 2 );
			ASSERT( mode != MODE_IM );
			lval1 = qinterpResolve( qip, mode, arg1, NULL );	/* class name */
			lval2 = qinterpResolve( qip, mode2, arg2, &qip->tempdata1 );	/* field offset */
			varGetInt( lval2, &arg2 );
			ASSERT( TYPE_STRING == lval1->type );
			jsp = qinterpFindClass( qip, lval1->data.sp );
			if ( NULL == jsp )
			{
				qip->curframe->err = IERR_CLASS_NOT_FOUND;
				break;
			}
			VARCLEAR( &qip->tempdata1 );
			stackPush( qip, &qip->tempdata1 );
			varRef( stackPeek0( qip ), jsclassGetStaticField( jsp, arg2 ) );
			break;

		case OP_DEREF:
			ASSERT( argcount == 1 );
			ASSERT( mode != MODE_IM );
			lval1 = qinterpResolve( qip, mode, arg1, NULL );	/* field name */
			ASSERT( TYPE_STRING == lval1->type );
			lval2 = stackPeek0( qip );				/* this */
			ASSERT( TYPE_INST == lval2->type || (TYPE_PTR == lval2->type && TYPE_INST == lval2->data.vp->type) );
			lval1 = qinterpFindField( qip, lval2, lval1 );
			if ( NULL == lval1 )
			{
				qip->curframe->err = IERR_FIELD_NOT_FOUND;
				break;
			}
			stackPop( qip );	/* pop this */
			VARCLEAR( &qip->tempdata1 );
			stackPush( qip, &qip->tempdata1 );
			ASSERT( TYPE_PTR != lval1->type );
			varRef( stackPeek0( qip ), lval1 );
			break;

		case OP_POP:
			ASSERT( argcount == 0 );
			stackPop( qip );
			break;

		case OP_ROT:
			ASSERT( argcount == 0 );
			lval1 = stackPeek0(qip);
			lval2 = stackPeek1(qip);
			varAssign( &qip->tempdata1, lval1 );
			varAssign( lval1, lval2 );
			varAssign( lval2, &qip->tempdata1 );
			VARCLEAR( &qip->tempdata1 );
			break;

		case OP_ROT3:
			ASSERT( argcount == 0 );
			lval1 = stackPeek0(qip);
			lval2 = stackPeekN(qip, 2);
			varAssign( &qip->tempdata1, lval1 );
			varAssign( lval1, lval2 );
			varAssign( lval2, &qip->tempdata1 );
			VARCLEAR( &qip->tempdata1 );
			break;

		case OP_BASECALL:
		case OP_CALL:
		case OP_CONSTRUCT:
			ASSERT( argcount == 2 );
			
			/* method sig in the const table */
			/*lval1 = qinterpResolve( qip, mode, arg1, &qip->tempdata1 );*/
			lval1 = (variant *)vectorDataElementAt( qip->curframe->ctx->constdata, arg1 );
			varValidateMem( lval1 );
			ASSERT( TYPE_STRING == lval1->type );
			{
				jsinst *jinst;
				variant *vthis = stackPeekN( qip, arg2 );
				if ( TYPE_PTR == vthis->type )
				{
					vthis = vthis->data.vp;
				}
				/*ASSERT( TYPE_INST == vthis->type );*/
				if ( TYPE_INST != vthis->type )
				{
					/* intrinsic type, attempt to call object:: whatever*/
					string *fqname = stringConstructChar("Object.");
					if ( NULL == fqname )
					{
						return IERR_OUT_OF_MEM;
					}
					if ( ! stringAppend( fqname, lval1->data.sp ) )
					{
						stringDestruct( fqname );
						return IERR_OUT_OF_MEM;
					}
					stringReplaceChar( fqname, 'I', 'S' );
					/*stackPush( qip, vthis );*/
					qip->curframe->err = callStaticMethod( qip, fqname );
					stringDestruct( fqname );
				}
				else
				{
					jinst = vthis->data.ip;
					if ( OP_CALL == op )
					{
						qip->curframe->err = callInstMethod( qip, jinst, lval1->data.sp, false );
					}
					else if ( OP_CONSTRUCT == op )
					{
						/*ASSERT( 1 == jinst->refcount );*/
						qip->curframe->err = callConstructors( qip, jinst->myclass, jinst, arg2 );
						if ( IERR_NONE == qip->curframe->err )
						{
							ASSERT( stackPeek0( qip )->type == TYPE_INST );
						}
					}
					else
					{
						qip->curframe->err = callBaseMethod( qip, jinst, lval1->data.sp );
					}
				}
				if ( qip->curframe->err != IERR_NONE )
				{
					if ( IERR_EXCEPTION == qip->curframe->err )
					{
						*pc = getCatch( qip->curframe );
						if ( -1 != *pc )
						{
							qip->curframe->err = IERR_NONE;
							break;
						}
						varAssign( &qip->curframe->retval, stackPeek0(qip) );
						stackPop( qip );
					}
					break;
				}
			}
			break;

		case OP_SCALL:
			ASSERT( argcount == 1 );
			lval1 = (variant *)vectorDataElementAt( qip->curframe->ctx->constdata, arg1 );
			varValidateMem( lval1 );
			ASSERT( TYPE_STRING == lval1->type );
			qip->curframe->err = callStaticMethod( qip, lval1->data.sp );
			if ( qip->curframe->err != IERR_NONE )
			{
				if ( IERR_EXCEPTION == qip->curframe->err )
				{
					*pc = getCatch( qip->curframe );
					if ( -1 != *pc )
					{
						qip->curframe->err = IERR_NONE;
						break;
					}
					varAssign( &qip->curframe->retval, stackPeek0(qip) );
					stackPop( qip );
				}
				break;
			}
			break;

		case OP_JMPZ:
			ASSERT( argcount == 1 );

			lval1 = stackPeek0( qip );

			if ( ! VARBOOL(lval1) )
			{
				qip->curframe->err = IERR_TYPE_MISMATCH;
				break;
			}
			if ( lval1->data.i == 0 )
			{
				lval1 = qinterpResolve( qip, mode, arg1, &qip->tempdata1 );
				ASSERT( TYPE_INT == lval1->type );
				*pc = lval1->data.i;
			}
			stackPop( qip );
			break;

		case OP_JMPNZ:
			ASSERT( argcount == 1 );

			lval1 = stackPeek0( qip );
			if ( ! VARBOOL(lval1) )
			{
				qip->curframe->err = IERR_TYPE_MISMATCH;
				break;
			}
			if ( lval1->data.i != 0 )
			{
				lval1 = qinterpResolve( qip, mode, arg1, &qip->tempdata1 );
				ASSERT( TYPE_INT == lval1->type );
				*pc = lval1->data.i;
			}
			stackPop( qip );
			break;

		case OP_JMP:
			ASSERT( argcount == 1 );
			lval1 = qinterpResolve( qip, mode, arg1, &qip->tempdata1 );
			ASSERT( TYPE_INT == lval1->type );
			*pc = lval1->data.i;
			break;

		case OP_LT:
			ASSERT( argcount == 0 );
			lval1 = stackPeek0(qip);
			lval2 = stackPeek1(qip);
			if ( ! varAssignOpLT( &qip->tempdata1, lval2, lval1 ) )
			{
				qip->curframe->err = IERR_TYPE_MISMATCH;
				break;
			}
			stackPop( qip );
			stackPop( qip );
			stackPush( qip, &qip->tempdata1 );
			VARCLEAR( &qip->tempdata1 );
			break;

		case OP_LTEQ:
			ASSERT( argcount == 0 );
			lval1 = stackPeek0(qip);
			lval2 = stackPeek1(qip);
			if ( ! varAssignOpLE( &qip->tempdata1, lval2, lval1 ) )
			{
				qip->curframe->err = IERR_TYPE_MISMATCH;
				break;
			}
			stackPop( qip );
			stackPop( qip );
			stackPush( qip, &qip->tempdata1 );
			VARCLEAR( &qip->tempdata1 );
			break;

		case OP_EQ:
			ASSERT( argcount == 0 );
			lval1 = stackPeek0(qip);
			lval2 = stackPeek1(qip);
			if (! varAssignOpEQ( &qip->tempdata1, lval2, lval1 ) )
			{
				qip->curframe->err = IERR_TYPE_MISMATCH;
				break;
			}
			stackPop( qip );
			stackPop( qip );
			stackPush( qip, &qip->tempdata1 );
			VARCLEAR( &qip->tempdata1 );
			break;

		case OP_GT:
			ASSERT( argcount == 0 );
			lval1 = stackPeek0(qip);
			lval2 = stackPeek1(qip);
			if ( ! varAssignOpGT( &qip->tempdata1, lval2, lval1 ) )
			{
				qip->curframe->err = IERR_TYPE_MISMATCH;
				break;
			}
			stackPop( qip );
			stackPop( qip );
			stackPush( qip, &qip->tempdata1 );
			VARCLEAR( &qip->tempdata1 );
			break;

		case OP_GTEQ:
			ASSERT( argcount == 0 );
			lval1 = stackPeek0(qip);
			lval2 = stackPeek1(qip);
			if ( ! varAssignOpGE( &qip->tempdata1, lval2, lval1 ) )
			{
				qip->curframe->err = IERR_TYPE_MISMATCH;
				break;
			}
			stackPop( qip );
			stackPop( qip );
			stackPush( qip, &qip->tempdata1 );
			VARCLEAR( &qip->tempdata1 );
			break;

		case OP_NEQ:
			ASSERT( argcount == 0 );
			lval1 = stackPeek0(qip);
			lval2 = stackPeek1(qip);
			if ( ! varAssignOpNEQ( &qip->tempdata1, lval2, lval1 ) )
			{
				qip->curframe->err = IERR_TYPE_MISMATCH;
				break;
			}
			stackPop( qip );
			stackPop( qip );
			stackPush( qip, &qip->tempdata1 );
			VARCLEAR( &qip->tempdata1 );
			break;

		case OP_MOD:
			ASSERT( argcount == 0 );
			lval1 = stackPeek0(qip);
			lval2 = stackPeek1(qip);
			if ( ! varAssignOpMod( &qip->tempdata1, lval2, lval1 ) )
			{
				qip->curframe->err = IERR_TYPE_MISMATCH;
				break;
			}
			stackPop( qip );
			stackPop( qip );
			stackPush( qip, &qip->tempdata1 );
			VARCLEAR( &qip->tempdata1 );
			break;

		case OP_OR:
			ASSERT( argcount == 0 );
			lval1 = stackPeek0(qip);
			lval2 = stackPeek1(qip);
			if ( TYPE_PTR == lval1->type )
			{
				lval1 = lval1->data.vp;
				varValidateMem( lval1 );
			}
			if ( TYPE_PTR == lval2->type )
			{
				lval2 = lval2->data.vp;
				varValidateMem( lval2 );
			}
			ASSERT( TYPE_PTR != lval1->type );
			ASSERT( TYPE_PTR != lval2->type );
			if ( varIsNumeric( lval1 ) )
			{
				varGetInt( lval1, &arg1 );
			}
			else
			{
				/* if it isn't bool or a number of some sort,
				// then it's a non-null inst or string or
				// something*/
				arg1 = 1;
			}
			if ( varIsNumeric( lval2 ) )
			{
				varGetInt( lval2, &arg2 );
			}
			else
			{
				arg2 = 1;
			}
			varAssignBool( &qip->tempdata1, arg1 || arg2 );
			stackPop( qip );
			stackPop( qip );
			stackPush( qip, &qip->tempdata1 );
			VARCLEAR( &qip->tempdata1 );
			break;

		case OP_AND:
			ASSERT( argcount == 0 );
			lval1 = stackPeek0(qip);
			lval2 = stackPeek1(qip);
			if ( TYPE_PTR == lval1->type )
			{
				lval1 = lval1->data.vp;
				varValidateMem( lval1 );
			}
			if ( TYPE_PTR == lval2->type )
			{
				lval2 = lval2->data.vp;
				varValidateMem( lval2 );
			}
			ASSERT( TYPE_PTR != lval1->type );
			ASSERT( TYPE_PTR != lval2->type );
			if ( varIsNumeric( lval1 ) )
			{
				varGetInt( lval1, &arg1 );
			}
			else
			{
				/* if it isn't bool or a number of some sort,
				// then it's a non-null inst or string or
				// something*/
				arg1 = 1;
			}
			if ( varIsNumeric( lval2 ) )
			{
				varGetInt( lval2, &arg2 );
			}
			else
			{
				arg2 = 1;
			}
			varAssignBool( &qip->tempdata1, arg1 && arg2 );
			stackPop( qip );
			stackPop( qip );
			stackPush( qip, &qip->tempdata1 );
			VARCLEAR( &qip->tempdata1 );
			break;

		case OP_BINOR:
			ASSERT( argcount == 0 );
			lval1 = stackPeek0(qip);
			lval2 = stackPeek1(qip);
			if ( ! varAssignOpBinOr( &qip->tempdata1, lval2, lval1 ) )
			{
				qip->curframe->err = IERR_TYPE_MISMATCH;
				break;
			}
			stackPop( qip );
			stackPop( qip );
			stackPush( qip, &qip->tempdata1 );
			VARCLEAR( &qip->tempdata1 );
			break;

		case OP_BINAND:
			ASSERT( argcount == 0 );
			lval1 = stackPeek0(qip);
			lval2 = stackPeek1(qip);
			if ( ! varAssignOpBinAnd( &qip->tempdata1, lval2, lval1 ) )
			{
				qip->curframe->err = IERR_TYPE_MISMATCH;
				break;
			}
			stackPop( qip );
			stackPop( qip );
			stackPush( qip, &qip->tempdata1 );
			VARCLEAR( &qip->tempdata1 );
			break;

		case OP_XOR:
			ASSERT( argcount == 0 );
			lval1 = stackPeek0(qip);
			lval2 = stackPeek1(qip);
			if ( ! varAssignOpBinXOr( &qip->tempdata1, lval2, lval1 ) )
			{
				qip->curframe->err = IERR_TYPE_MISMATCH;
				break;
			}
			stackPop( qip );
			stackPop( qip );
			stackPush( qip, &qip->tempdata1 );
			VARCLEAR( &qip->tempdata1 );
			break;

		case OP_COMPL:
			ASSERT( argcount == 0 );
			lval1 = stackPeek0(qip);
			if ( ! varComp( lval1 ) )
			{
				qip->curframe->err = IERR_TYPE_MISMATCH;
				break;
			}
			break;

		case OP_NOT:
			ASSERT( argcount == 0 );
			lval1 = stackPeek0(qip);
			if ( ! varLogNot( lval1 ) )
			{
				qip->curframe->err = IERR_TYPE_MISMATCH;
				break;
			}
			break;

		case OP_PINC:
			ASSERT( argcount == 0 );
			lval1 = stackPeek0(qip);
			ASSERT( TYPE_PTR == lval1->type );
			lval2 = lval1->data.vp;
			VARDEREF( lval1 );
			ASSERT( lval2->type == lval1->type );
			ASSERT( TYPE_PTR != lval1->type );
			if ( TYPE_INT == lval2->type || TYPE_CHAR == lval2->type )
			{
				lval2->data.i++;
			}
			else
			{
				qip->curframe->err = IERR_TYPE_MISMATCH;
				break;
			}
			break;

		case OP_PDEC:
			ASSERT( argcount == 0 );
			lval1 = stackPeek0(qip);
			ASSERT( TYPE_PTR == lval1->type );
			lval2 = lval1->data.vp;
			VARDEREF( lval1 );
			ASSERT( lval2->type == lval1->type );
			ASSERT( TYPE_PTR != lval1->type );
			if ( TYPE_INT == lval2->type || TYPE_CHAR == lval2->type )
			{
				lval2->data.i--;
			}
			else
			{
				qip->curframe->err = IERR_TYPE_MISMATCH;
				break;
			}
			break;

		case OP_INC:
			ASSERT( argcount == 0 );
			lval1 = stackPeek0(qip);
			ASSERT( TYPE_PTR == lval1->type );
			lval2 = lval1->data.vp;
			if ( TYPE_INT == lval2->type || TYPE_CHAR == lval2->type )
			{
				lval2->data.i++;
			}
			else
			{
				qip->curframe->err = IERR_TYPE_MISMATCH;
				break;
			}
			VARDEREF( lval1 );
			break;

		case OP_DEC:
			ASSERT( argcount == 0 );
			lval1 = stackPeek0(qip);
			ASSERT( TYPE_PTR == lval1->type );
			lval2 = lval1->data.vp;
			if ( TYPE_INT == lval2->type || TYPE_CHAR == lval2->type )
			{
				lval2->data.i--;
			}
			else
			{
				qip->curframe->err = IERR_TYPE_MISMATCH;
				break;
			}
			VARDEREF( lval1 );
			break;

		case OP_LINE:
			ASSERT( argcount == 1 );
			ASSERT( MODE_IM == mode );
			qip->linenum = arg1;
			break;

		default:
			qip->curframe->err = IERR_UNKNOWN_OPCODE;
			break;
		}
	}
	if ( qip->curframe->err == IERR_RETURN )
	{
		qip->curframe->err = IERR_NONE;
	}
	while ( vectorCount(qip->stack) > stackpos )
	{
		stackPop( qip );
	}
	ASSERT ( vectorCount(qip->frames) == framepos );
	return qip->curframe->err;
}
