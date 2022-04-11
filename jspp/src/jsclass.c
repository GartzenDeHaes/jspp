#ifdef HAVE_CONFIG_H
#include <jspp/config.h>
#endif
#include <stdlib.h>
#ifdef HAVE_STRING_H
#include <string.h>
#endif
#include <spl/debug.h>
#include <jspp/jsclass.h>
#include <jspp/opcodes.h>


string *buildMethodSig( const string *name, const bool isstatic )
{
	string *str = stringConstructChar( "" );
	if ( NULL == str )
	{
		return NULL;
	}
	stringAppend( str, name );
	stringAppendChar( str, (isstatic)? ":S" : ":I" );
	stringSetChar( str, name->len+2, 'v' );
	return str;
}

void methodSigAppendArg( string *sig )
{
	stringValidateMem( sig );
	stringAppendCh( sig, 'v' );
}

vardef *createVar( const char *name, const bool isstatic, const enum ArgSource srcmode, const bool isptr )
{
	vardef *vdp;
	ASSERT( isstatic == false || isstatic == true );
	ASSERT( isptr == false || isptr == true );

	vdp = (vardef *)malloc( sizeof ( vardef ) );
	if ( NULL == vdp )
	{
		return NULL;
	}
	vdp->name = stringConstructChar( name );
	if ( NULL == vdp->name )
	{
		free( vdp );
		return NULL;
	}
	vdp->srcmode = srcmode;
	vdp->isstatic = isstatic;
	vdp->isnative = false;
	vdp->isptr = isptr;
	vdp->isauto = false;
	vdp->nativefield = NULL;
	return vdp;
}

void deleteVar( vardef *vdp )
{
	stringDestruct( vdp->name );
	free( vdp );
}

jsclass *jsclassConstruct( const char *name, const char *basename, void *privateclassdata )
{
	jsclass *jsp = malloc( sizeof(struct jsclass) );
	if ( NULL == jsp )
	{
		return NULL;
	}
	jsp->name = stringConstructChar( name );
	if ( NULL == jsp->name )
	{
		free( jsp );
		return NULL;
	}
	jsp->basename = stringConstructChar( basename );
	if ( NULL == jsp->basename )
	{
		stringDestruct( jsp->name );
		free( jsp );
		return NULL;
	}
	jsp->methods = vectorConstruct( sizeof(jsmethod *) );
	if ( NULL == jsp->methods )
	{
		stringDestruct( jsp->basename );
		stringDestruct( jsp->name );
		free( jsp );
		return NULL;
	}
	jsp->fields = vectorConstruct( sizeof(vardef *) );
	if ( NULL == jsp->fields )
	{
		vectorDestruct( jsp->methods );
		stringDestruct( jsp->basename );
		stringDestruct( jsp->name );
		free( jsp );
		return NULL;
	}
	jsp->instancelocations = vectorConstruct( sizeof( struct location *) );
	if ( NULL == jsp->instancelocations )
	{
		vectorDestruct( jsp->fields );
		vectorDestruct( jsp->methods );
		stringDestruct( jsp->basename );
		stringDestruct( jsp->name );
		free( jsp );
		return NULL;
	}
	jsp->staticlocations = vectorConstruct( sizeof (struct location *) );
	if ( NULL == jsp->staticlocations )
	{
		vectorDestruct( jsp->instancelocations );
		vectorDestruct( jsp->fields );
		vectorDestruct( jsp->methods );
		stringDestruct( jsp->basename );
		stringDestruct( jsp->name );
		free( jsp );
		return NULL;
	}
	jsp->vtable = NULL;
	jsp->baseclass = NULL;
	jsp->privateclassdata = privateclassdata;
	return jsp;
}

void jsclassDestruct( jsclass *jsp )
{
	int x, count;
	jsmethod *mp;
	vardef *vdp;
	jsclassValidateMem( jsp );

	stringDestruct( jsp->name );
	stringDestruct( jsp->basename );

	count = vectorCount(jsp->methods);
	for ( x = 0; x < count; x++ )
	{
		mp = (jsmethod *)vectorElementAt( jsp->methods, x );
		jsmethodDestruct( mp );
	}
	vectorDestruct( jsp->methods );

	count = vectorCount(jsp->fields);
	for ( x = 0; x < count; x++ )
	{
		vdp = (vardef *)vectorElementAt( jsp->fields, x );
		deleteVar( vdp );
	}
	vectorDestruct( jsp->fields );

	count = vectorCount( jsp->instancelocations );
	for ( x = 0; x < count; x++ )
	{
		variant *lp = (variant *)vectorElementAt( jsp->instancelocations, x );
		varDestruct( lp );
	}
	vectorDestruct( jsp->instancelocations );

	count = vectorCount( jsp->staticlocations );
	for ( x = 0; x < count; x++ )
	{
		variant *lp = (variant *)vectorElementAt( jsp->staticlocations, x );
		varDestruct( lp );
	}
	vectorDestruct( jsp->staticlocations );
	if ( NULL != jsp->vtable )
	{
		hashtableDestruct( jsp->vtable );
	}
	free( jsp );
}

static void jsclassAddMethod( jsclass *jsp, jsmethod *mp )
{
	jsclassValidateMem( jsp );
	jsmethodValidateMem( mp );
	vectorAdd( jsp->methods, &mp );
}

vardef *jsclassAddField
( 
	jsclass *jsp, 
	const char *name, 
	const bool isstatic, 
	const enum jstype typ, 
	const enum jstype subtyp 
)
{
	vardef *vdp;
	variant *lp;

	jsclassValidateMem( jsp );
	ASSERT( isstatic == false || isstatic == true );
	vdp = createVar( name, isstatic, (isstatic) ? MODE_CLASS : MODE_INST, false );
	if ( NULL == vdp )
	{
		return NULL;
	}
	lp = varConstruct();
	if ( NULL == lp )
	{
		free(vdp);
		return NULL;
	}
	vdp->isstatic = isstatic;
	if ( isstatic )
	{
		vdp->offset = vectorCount( jsp->staticlocations );
		vectorAdd( jsp->staticlocations, &lp );
	}
	else
	{
		vdp->offset = vectorCount( jsp->instancelocations );
		vectorAdd( jsp->instancelocations, &lp );
	}
	vectorAdd( jsp->fields, &vdp );
	return vdp;
}

vardef *jsclassAddNativeField
( 
	jsclass *jsp, 
	NATIVEMETHOD fld,
	const char *name, 
	const bool isstatic, 
	const enum jstype typ, 
	const enum jstype subtyp 
)
{
	vardef *vdp = jsclassAddField( jsp, name, isstatic, typ, subtyp );
	if ( NULL == vdp )
	{
		return NULL;
	}
	vdp->nativefield = fld;
	vdp->isnative = true;
	return vdp;
}

bool jsclassInitVtab( jsclass *jsp )
{
	int x, count;

	jsclassValidateMem( jsp );
	if ( NULL != jsp->baseclass && NULL == jsp->baseclass->vtable )
	{
		if ( ! jsclassInitVtab( jsp->baseclass ) )
		{
			return false;
		}
	}
	ASSERT( jsp->vtable == NULL );
	if ( NULL == jsp->baseclass )
	{
		jsp->vtable = hashtableConstruct();
	}
	else
	{
		jsp->vtable = hashtableConstructDupStrKey( jsp->baseclass->vtable );
	}
	if ( NULL == jsp->vtable )
	{
		return false;
	}
	count = vectorCount(jsp->methods);
	for ( x = 0; x < count; x++ )
	{
		jsmethod *mp = (jsmethod *)vectorElementAt( jsp->methods, x );
		if ( ! hashtableAdd( jsp->vtable, mp->sig, mp ) )
		{
			return false;
		}
	}
	/*
	 *  Init static fields
	 */
	count = vectorCount(jsp->fields);
	for ( x = 0; x < count; x++ )
	{
		vardef *vdp = (vardef *)vectorElementAt( jsp->fields, x );
		if ( vdp->isstatic )
		{
			variant *var = varConstruct();
			if ( NULL == var )
			{
				return false;
			}
			vectorAdd( jsp->staticlocations, &var );
		}
	}
	return true;
}	

variant *jsclassGetStaticField( const jsclass *jsp, const int offset )
{
	ASSERT( offset < vectorCount( jsp->staticlocations ) );
	return (variant *)vectorElementAt( jsp->staticlocations, offset );
}

vardef *jsclassFindField( const jsclass *jsp, const char *name )
{
	vardef *vdp;
	int x, count;

	/* should be a hashtable lookup */
	count = vectorCount(jsp->fields);
	for ( x = 0; x < count; x++ )
	{
		vdp = (vardef *)vectorElementAt( jsp->fields, x );
		ASSERT_MEM( vdp, sizeof(struct vardef) );
		if ( strcmp(vdp->name->cstr, name) == 0 )
		{
			return vdp;
		}
	}
	return NULL;
}

jsmethod *jsclassFindMethod( const jsclass *jsp, const char *sig )
{
	jsclassValidateMem( jsp );
	if ( NULL == jsp->vtable )
	{
		int x, count;
		jsmethod *mp;

		count = vectorCount(jsp->methods);
		for ( x = 0; x < count; x++ )
		{
			mp = (jsmethod *)vectorElementAt( jsp->methods, x );
			if ( strcmp(sig, mp->sig->cstr) == 0 )
			{
				return mp;
			}
		}
	}
	else
	{
		return (jsmethod *)hashtableGetChar( jsp->vtable, sig, (int)strlen(sig) );
	}
	return NULL;
}

jsmethod *jsclassAddNativeMethod( jsclass *jsp, NATIVEMETHOD meth, const char *name, const bool isstatic, const enum jsaccess access, const bool returnsvalue )
{
	jsmethod *mp = jsmethodConstruct( jsp, name, isstatic, access );
	if ( NULL == mp )
	{
		return NULL;
	}
	mp->nativemethod = meth;
	mp->isnative = true;
	mp->hasretval = returnsvalue;
	return mp;
}

jsmethod *jsmethodConstruct( jsclass *jsp, const char *name, const bool isstatic, const enum jsaccess acc )
{
	jsmethod *mp;
	jsclassValidateMem( jsp );
	ASSERT( isstatic == 0 || isstatic == 1 );
	
	mp = (jsmethod *)malloc( sizeof( jsmethod ) );
	if ( NULL == mp )
	{
		return NULL;
	}
	mp->name = stringConstructChar( name );
	if ( NULL == mp->name )
	{
		free( mp );
		return NULL;
	}
	mp->args = vectorConstruct( sizeof( vardef *) );
	if ( NULL == mp->args )
	{
		stringDestruct( mp->name );
		free( mp );
		return NULL;
	}
	mp->code = int16arrayConstruct( 100 );
	if ( NULL == mp->code )
	{
		vectorDestruct( mp->args );
		stringDestruct( mp->name );
		free( mp );
		return NULL;
	}
	mp->pc = 0;
	mp->isstatic = isstatic;
	mp->isnative = false;
	mp->hasretval = false;
	mp->access = acc;
	mp->myclass = jsp;

	mp->sig = buildMethodSig( mp->name, mp->isstatic );
	if ( NULL == mp->sig )
	{
		int16arrayDestruct( mp->code );
		vectorDestruct( mp->args );
		stringDestruct( mp->name );
		free( mp );
		return NULL;
	}

	jsclassAddMethod ( jsp, mp );
	return mp;
}

void jsmethodDestruct( jsmethod *mp )
{
	vardef *arg;
	int x, count;

	jsmethodValidateMem( mp );

	count = vectorCount(mp->args);
	for ( x = 0; x < count; x++ )
	{
		arg = (vardef *)vectorElementAt( mp->args, x );
		deleteVar( arg );
	}
	vectorDestruct( mp->args );
	stringDestruct( mp->name );
	stringDestruct( mp->sig );
	int16arrayDestruct( mp->code );
	free( mp );
}

int jsmethodAddArg( jsmethod *mp, const char *name, const bool isptr )
{
	vardef *vdp;
	jsmethodValidateMem( mp );

	vdp = createVar( name, 0, MODE_BLOCK, isptr );
	if ( ! vectorAdd( mp->args, &vdp ) )
	{
		deleteVar( vdp );
		return -1;
	}
	vdp->offset = vectorCount(mp->args) - 1;

	methodSigAppendArg( mp->sig );
	return vdp->offset;
}

vardef *jsmethodGetArg( const jsmethod *mp, const int offset )
{
	jsmethodValidateMem( mp );
	ASSERT( offset < vectorCount( mp->args ) );
	return (vardef *)vectorElementAt( mp->args, offset );
}

int jsmethodCodeLen( const jsmethod *mp )
{
	jsmethodValidateMem( mp );
	return mp->pc;
}

bool loadBaseFields( const jsclass *jsp, jsinst *inst )
{
	int x, count;
	variant var;
	var.type = TYPE_VOID;
	var.data.i = 0;

	if ( NULL != jsp->baseclass )
	{
		if ( ! loadBaseFields( jsp->baseclass, inst ) )
		{
			return false;
		}
	}
	count = vectorCount(jsp->fields);
	for ( x = 0; x < count; x++ )
	{
		/* should use myclass->instancelocations */
		vardef *vdp = (vardef *)vectorElementAt( jsp->fields, x );
		ASSERT_MEM( vdp, sizeof( vardef ) );
		if ( ! vdp->isstatic )
		{
			if ( ! vectorAdd( inst->fields, &var ) )
			{
				return false;
			}
		}
		/*ASSERT( vectorCount( inst->fields ) == vdp->offset + 1 );*/
	}
	return true;
}

jsinst *jsinstConstruct( jsclass *myclass )
{
	jsinst *inst;

	jsclassValidateMem( myclass );
	inst = (jsinst *)malloc( sizeof( jsinst ) );
	if ( NULL == inst )
	{
		return NULL;
	}
	inst->fields = vectorConstruct( sizeof(struct variant) );
	if ( NULL == inst->fields )
	{
		free( inst );
		return NULL;
	}
	if ( ! loadBaseFields( myclass, inst ) )
	{
		vectorDestruct( inst->fields );
		free( inst );
		return NULL;
	}
	inst->refcount = 0;
	inst->myclass = myclass;
	inst->nativedata = NULL;
	return inst;
}

void jsinstDestruct( jsinst *inst )
{
	int x, count;
	variant *var;

	count = vectorCount( inst->fields );
	for ( x = 0; x < count; x++ )
	{
		var = vectorDataElementAt( inst->fields, x );
		varClear( var );
	}
	if ( NULL != inst->nativedata )
	{
		if ( NULL != inst->nativedata->finializer )
		{
			(*inst->nativedata->finializer)(inst->nativedata->data);
		}
		free( inst->nativedata );
	}
	vectorDestruct( inst->fields );
	free( inst );
}

void jsinstRef( jsinst *inst )
{
	inst->refcount++;
}

void jsinstDeref( jsinst *inst )
{
	if ( --inst->refcount <=0 )
	{
		jsinstDestruct( inst );
	}
}

#ifdef DEBUG

void jsmethodValidateMem( const jsmethod *mp )
{
	vardef *vdp;
	int x, count;

	ASSERT_MEM( mp, sizeof( jsmethod ) );
	stringValidateMem( mp->name );
	stringValidateMem( mp->sig );
	vectorValidateMem( mp->args );

	count = vectorCount(mp->args);
	for ( x = 0; x < count; x++ )
	{
		vdp = (vardef *)vectorElementAt( mp->args, x );
		ASSERT_MEM( vdp, sizeof( vardef ) );
		stringValidateMem( vdp->name );
	}
	int16arrayCheckMem( mp->code );
}

static void jsmethodCheckMem( const jsmethod *mp )
{
	vardef *vdp;
	int x, count;

	DEBUG_NOTE_MEM_ALLOCATION( mp );
	stringCheckMem( mp->name );
	stringCheckMem( mp->sig );
	vectorCheckMem( mp->args );

	count = vectorCount(mp->args);
	for ( x = 0; x < count; x++ )
	{
		vdp = (vardef *)vectorElementAt( mp->args, x );
		DEBUG_NOTE_MEM_ALLOCATION( vdp );
		stringCheckMem( vdp->name );
	}
	int16arrayCheckMem( mp->code );
}

void jsclassValidateMem( const jsclass *jsp )
{
	int x, count;
	jsmethod *mp;
	vardef *vdp;

	ASSERT_MEM( jsp, sizeof( jsclass ) );
	stringValidateMem( jsp->name );
	stringValidateMem( jsp->basename );
	vectorValidateMem( jsp->methods );

	count = vectorCount(jsp->methods);
	for ( x = 0; x < count; x++ )
	{
		mp = (jsmethod *)vectorElementAt( jsp->methods, x );
		jsmethodValidateMem( mp );
	}

	vectorValidateMem( jsp->fields );
	count = vectorCount(jsp->fields);
	for ( x = 0; x < count; x++ )
	{
		vdp = (vardef *)vectorElementAt( jsp->fields, x );
		ASSERT_MEM( vdp, sizeof( vardef ) );
		stringValidateMem( vdp->name );
	}
	vectorValidateMem( jsp->staticlocations );
	count = vectorCount( jsp->staticlocations );
	for ( x = 0; x < count; x++ )
	{
		ASSERT_MEM( (variant *)vectorElementAt( jsp->staticlocations, x ), sizeof( variant ) );
	}
	vectorValidateMem( jsp->instancelocations );
	count = vectorCount( jsp->instancelocations );
	for ( x = 0; x < count; x++ )
	{
		ASSERT_MEM( (variant *)vectorElementAt( jsp->instancelocations, x ), sizeof( variant ) );
	}
	if ( NULL != jsp->vtable )
	{
		hashtableValidateMem( jsp->vtable );
	}
}

void jsclassCheckMem( const jsclass *jsp )
{
	int x, count;
	jsmethod *mp;
	vardef *vdp;

	jsclassValidateMem( jsp );

	DEBUG_NOTE_MEM_ALLOCATION( jsp );
	if ( NULL != jsp->baseclass )
	{
		jsclassCheckMem( jsp->baseclass );
	}
	stringCheckMem( jsp->name );
	stringCheckMem( jsp->basename );
	vectorCheckMem( jsp->methods );

	count = vectorCount( jsp->methods );
	for( x = 0; x < count; x++ )
	{
		mp = (jsmethod *)vectorElementAt( jsp->methods, x );
		jsmethodCheckMem( mp );
	}

	vectorCheckMem( jsp->fields );
	count = vectorCount( jsp->fields );
	for ( x = 0; x < count; x++ )
	{
		vdp = (vardef *)vectorElementAt( jsp->fields, x );
		DEBUG_NOTE_MEM_ALLOCATION( vdp );
		stringCheckMem( vdp->name );
	}
	vectorCheckMem( jsp->staticlocations );
	count = vectorCount( jsp->staticlocations );
	for ( x = 0; x < count; x++ )
	{
		varCheckMem( (struct variant *)vectorElementAt( jsp->staticlocations, x ) );
	}
	vectorCheckMem( jsp->instancelocations );
	count = vectorCount( jsp->instancelocations );
	for ( x = 0; x < count; x++ )
	{
		varCheckMem( (struct variant *)vectorElementAt( jsp->instancelocations, x ) );
	}
	if ( NULL != jsp->vtable )
	{
		hashtableCheckMem( jsp->vtable );
	}
}

void jsinstValidateMem( const jsinst *inst )
{
	int x, count;

	ASSERT_MEM(inst, sizeof(struct jsinst) );
	vectorValidateMem( inst->fields );

	count = vectorCount( inst->fields );
	for ( x = 0; x < count; x++ )
	{
		varValidateMem( (variant *)vectorDataElementAt( inst->fields, x ) );
	}
	if ( inst->nativedata != NULL )
	{
		ASSERT_MEM( inst->nativedata, sizeof(struct nativedataHolder) );
	}
}

void jsinstCheckMem( const jsinst *inst )
{
	int x, count;

	DEBUG_NOTE_MEM_ALLOCATION( inst );
	vectorCheckMem( inst->fields );

	count = vectorCount( inst->fields );
	for ( x = 0; x < count; x++ )
	{
		varCheckMem( (variant *)vectorDataElementAt( inst->fields, x ) );
	}
	if ( inst->nativedata != NULL )
	{
		DEBUG_NOTE_MEM_ALLOCATION( inst->nativedata );
		(*inst->nativedata->memcheck)(inst->nativedata->data);
	}
}

#endif
