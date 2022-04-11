#ifdef HAVE_CONFIG_H
#include <jspp/config.h>
#endif
#include <spl/debug.h>
#include <jspp/native.h>

enum InterpErrors callInstMethod( qinterp *qip, jsinst *inst, const string *mname, const bool nopop);
bool callInternalConstructors( qinterp *qip, jsclass *jsp, jsinst *inst );
enum InterpErrors callClassConstructors
( 
	qinterp *qip, 
	jsclass *jsp, 
	jsinst *inst, 
	int argcount,
	const bool isbase
);

variant *getArgumentN( qinterp *qip, const int argnum )
{
	qinterpValidateMem( qip );
	ASSERT( argnum >= 0 );
	ASSERT( argnum < vectorCount(qip->curframe->curmethod->args) );
	return (variant *)vectorDataElementAt(qip->curframe->locals, argnum );
}

#ifdef DEBUG2
int getArgumentCount( const qinterp *qip )
{
	return vectorCount(qip->curframe->curmethod->args);
}
#endif

variant *findField( qinterp *qip, const jsinst *inst, const char *name )
{
	jsclass *jsp;
	vardef *vdp;

	qinterpValidateMem( qip );
	jsinstValidateMem( inst );

	jsp = inst->myclass;
	vdp = jsclassFindField( jsp, name );
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
			return vectorDataElementAt( inst->fields, vdp->offset );
		}
	}
#ifdef _WIN32
	return NULL;
#endif
}

static string *buildInstanceSig( const string *name, int count )
{
	string *methsig;
	if ( NULL == (methsig = stringConstructDup( name )) )
	{
		return NULL;
	}
	if ( ! stringAppendChar( methsig, ":Iv" ) )
	{
		stringDestruct( methsig );
		return NULL;
	}
	if ( ! stringAppendChars( methsig, 'v', count+1 ) )
	{
		stringDestruct( methsig );
		return NULL;
	}
	return methsig;
}

enum InterpErrors createJSClassInstance
( 
	qinterp *qip, 
	string *classname, 
	jsinst **inst, 
	const int argc,
	...
)
{
	va_list marker;
	variant *varthis;
	jsclass *jsp;
	enum InterpErrors err;
	string *methsig;
	int x;

	varthis = varConstruct();
	if ( NULL == varthis )
	{
		return IERR_OUT_OF_MEM;
	}

	jsp = qinterpFindClass( qip, classname );
	if ( NULL == jsp )
	{
		varDestruct( varthis );
		return IERR_CLASS_NOT_FOUND;
	}
	*inst = jsinstConstruct( jsp );
	if ( NULL == *inst )
	{
		varDestruct( varthis );
		return IERR_OUT_OF_MEM;
	}
	jsinstValidateMem( *inst );
	if ( ! callInternalConstructors( qip, jsp, *inst ) )
	{
		varDestruct( varthis );
		return IERR_INTERNAL_ERROR2;
	}
	jsinstValidateMem( *inst );

	varthis->type = TYPE_INST;
	varthis->data.ip = *inst;
	stackPush( qip, varthis );

	va_start( marker, argc );
	for ( x = 0; x < argc; x++ )
	{
		variant *vparg = va_arg(marker, variant *);
		if ( ! stackPush( qip, vparg ) )
		{
			varDestruct( varthis );
			return IERR_OUT_OF_MEM;		
		}
	}
	va_end( marker );

	jsinstValidateMem( *inst );
	ASSERT( 2 == (*inst)->refcount );

	if ( NULL == (methsig = buildInstanceSig( jsp->name, argc )) )
	{
		return IERR_OUT_OF_MEM;
	}
	while ( NULL != methsig )
	{
		if ( (err = callInstMethod( qip, *inst, methsig, true )) != IERR_NONE )
		{
			if ( err == IERR_UNKNOWN_METHOD && argc > 0 )
			{
				stringDestruct( methsig );
				if ( NULL == (methsig = buildInstanceSig( jsp->name, 0 )) )
				{
					return IERR_OUT_OF_MEM;
				}
				if ( (err = callInstMethod( qip, *inst, methsig, true )) != IERR_NONE )
				{
					stringDestruct( methsig );
					varDestruct( varthis );
					return err;
				}
			}
			else if ( err == IERR_UNKNOWN_METHOD && argc == 0 )
			{
				/* ignore missing default constructor */
				err = IERR_NONE;
			}
			else
			{
				stringDestruct( methsig );
				varDestruct( varthis );
				return err;
			}
		}
		stringDestruct( methsig );
		if ( NULL == (jsp = jsp->baseclass) )
		{
			methsig = NULL;
		}
		else
		{
			methsig = buildInstanceSig(jsp->name, argc);
		}
	}
	varthis->type = TYPE_VOID;
	varDestruct( varthis );
	jsinstValidateMem( *inst );
	jsinstDeref( *inst );
	ASSERT( 1 == (*inst)->refcount );
	
	/* Native methods have their arguments still on the stack,
	// so this assertion isn't valid.
	//ASSERT( 0 == vectorCount( qip->stack ) ); */

	return IERR_NONE;
}

enum InterpErrors qinterpCall( qinterp *qip, jsinst *inst, const char *methodname, variant **varret, const int argc, ... )
{
	va_list marker;
	enum InterpErrors err;
	variant *varthis = varConstruct();
	string *strname;
	int x;

	qinterpValidateMem( qip );
	jsinstValidateMem( inst );
	jsinstRef( inst );
	ASSERT( 2 == inst->refcount );

	if ( NULL == varthis )
	{
		return IERR_OUT_OF_MEM;
	}
	strname = stringConstructChar( methodname );
	if ( NULL == strname )
	{
		varDestruct( varthis );
		return IERR_OUT_OF_MEM;
	}
	stringAppendChar( strname, ":Ivv" );
	if ( ! varAssignInst( varthis, inst ) )
	{
		varDestruct( varthis );
		stringDestruct( strname );
		return IERR_OUT_OF_MEM;
	}
	if ( ! vectorAdd( qip->stack, varthis ) )
	{
		varDestruct( varthis );
		stringDestruct( strname );
		return IERR_OUT_OF_MEM;
	}
	va_start( marker, argc );
	for ( x = 0; x < argc; x++ )
	{
		variant *vparg = va_arg(marker, variant *);
		varValidateMem( vparg );
		if ( ! vectorAdd( qip->stack, vparg ) )
		{
			varDestruct( varthis );
			stringDestruct( strname );
			return IERR_OUT_OF_MEM;
		}
		stringAppendChar( strname, "v" );
	}
	va_end( marker );

	err = callInstMethod( qip, inst, strname, false );
	stringDestruct( strname );

	ASSERT( 2 == inst->refcount );

	if ( 0 != vectorCount( qip->stack ) )
	{
		*varret = varConstructDup( (variant *)vectorDataPeek( qip->stack ) );
		varClear( (variant *)vectorDataPeek(qip->stack) );
		vectorPop( qip->stack );
	}
	while ( 0 != vectorCount( qip->stack ) )
	{
		varClear( (variant *)vectorDataPop(qip->stack) );
	}
	ASSERT( vectorCount(qip->stack) == 0 );
	if( 1 == inst->refcount )
	{
		jsinstRef( inst );
	}
	varDestruct( varthis );
	ASSERT( 1 == inst->refcount );

	jsinstValidateMem( inst );
	
	return err;
}


