#ifdef HAVE_CONFIG_H
#include <jspp/config.h>
#endif
#include <stdio.h>
#include <spl/debug.h>
#include <jspp/jhttp_lib.h>
#include <jspp/native.h>

void httprequest_finializer( void *vadp )
{
	httprequest *req = (httprequest *)vadp;
	httprequestDestruct( req );
}

void httprequest_memcheck( void *vadp )
{
	DEBUG_NOTE_MEM( vadp );
}

enum InterpErrors httprequest_httprequest(qinterp *qip, const void *mp, variant *ret)
{
	variant *varthis = getArgumentN( qip, 0 );
	variant *varurl = getArgumentN( qip, 1 );
	jsinst *inst = varthis->data.ip;
	httprequest *req;

	ASSERT( getArgumentCount( qip ) == 2 );
	ASSERT_MEM( mp, sizeof(struct jsmethod) );
	varValidateMem( varthis );
	ASSERT( TYPE_INST == varthis->type );
	varValidateMem( varurl );
	varValidateMem( ret );

	if ( TYPE_STRING != varurl->type )
	{
		return IERR_TYPE_MISMATCH;
	}
	inst->nativedata = malloc( sizeof(struct nativedataHolder) );
	if ( NULL == inst->nativedata )
	{
		return IERR_OUT_OF_MEM;
	}
	inst->nativedata->data = req = httprequestConstruct( varurl->data.sp->cstr, "", 0, UA_GENERIC );
	if ( NULL == req )
	{
		return IERR_OUT_OF_MEM;
	}
	inst->nativedata->finializer = httprequest_finializer;
	inst->nativedata->memcheck = httprequest_memcheck;
	inst->magic = HTTPREQ_MAJIC;
	return IERR_NONE;
}

enum InterpErrors httprequest_get(qinterp *qip, const void *mp, variant *ret)
{
	variant *varthis = getArgumentN( qip, 0 );
	jsinst *inst = varthis->data.ip;
	httprequest *req;
	http_retcode httpret;
	string *data = NULL;

	ASSERT( getArgumentCount( qip ) == 1 );
	ASSERT_MEM( mp, sizeof(struct jsmethod) );
	varValidateMem( varthis );
	ASSERT( TYPE_INST == varthis->type );
	varValidateMem( ret );
	ASSERT( inst->magic == HTTPREQ_MAJIC );

	req = (httprequest *)inst->nativedata->data;
	httprequestValidateMem( req );

	if ( (httpret = httprequestGet( req, &data )) == 0 )
	{
		varAssignString( ret, data );
	}
	else
	{
		char buf[100];
		sprintf( buf, "HTTP ERROR: %d", httpret );
		varAssignChar( ret, buf );
	}
	return IERR_NONE;
}

