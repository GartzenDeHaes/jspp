#ifdef HAVE_CONFIG_H
#include <jspp/config.h>
#endif
#include <spl/debug.h>
#include <jspp/jdes.h>
#include <jspp/native.h>

enum InterpErrors des_encrypt(qinterp *qip, const void *mp, variant *ret)
{
	variant *varpw = getArgumentN( qip, 0 );
	variant *vardata = getArgumentN( qip, 1 );
	string *pw;
	string *data;
	string *out;

	ASSERT( getArgumentCount( qip ) == 2 );
	ASSERT_MEM( mp, sizeof(struct jsmethod) );
	varValidateMem( varpw );
	varValidateMem( vardata );
	varValidateMem( ret );

	if ( TYPE_STRING == varpw->type )
	{
		pw = varpw->data.sp;
	}
	else
	{
		pw = varToString( varpw );
		if ( NULL == pw )
		{
			return IERR_OUT_OF_MEM;
		}
	}
	if ( TYPE_STRING == vardata->type )
	{
		data = vardata->data.sp;
	}
	else
	{
		data = varToString( vardata );
		if ( NULL == data )
		{
			if ( TYPE_STRING != varpw->type )
			{
				stringDestruct( pw );
			}
			return IERR_OUT_OF_MEM;
		}
	}
	if ( ! desEncryptString( pw, (byte *)data->cstr, data->len, &out ) )
	{
		if ( TYPE_STRING != varpw->type )
		{
			stringDestruct( pw );
		}
		if ( TYPE_STRING != vardata->type )
		{
			stringDestruct( data );
		}
		return IERR_OUT_OF_MEM;
	}
	ret->type = TYPE_STRING;
	ret->data.sp = out;

	if ( TYPE_STRING != varpw->type )
	{
		stringDestruct( pw );
	}
	if ( TYPE_STRING != vardata->type )
	{
		stringDestruct( data );
	}

	return IERR_NONE;
}

enum InterpErrors des_decrypt(qinterp *qip, const void *mp, variant *ret)
{
	variant *varpw = getArgumentN( qip, 0 );
	variant *vardata = getArgumentN( qip, 1 );
	string *pw;
	string *data;
	byte *out;

	ASSERT( getArgumentCount( qip ) == 2 );
	ASSERT_MEM( mp, sizeof(struct jsmethod) );
	varValidateMem( varpw );
	varValidateMem( vardata );
	varValidateMem( ret );

	if ( TYPE_STRING == varpw->type )
	{
		pw = varpw->data.sp;
	}
	else
	{
		pw = varToString( varpw );
		if ( NULL == pw )
		{
			return IERR_OUT_OF_MEM;
		}
	}
	if ( TYPE_STRING == vardata->type )
	{
		data = vardata->data.sp;
	}
	else
	{
		data = varToString( vardata );
		if ( NULL == data )
		{
			if ( TYPE_STRING != varpw->type )
			{
				stringDestruct( pw );
			}
			return IERR_OUT_OF_MEM;
		}
	}
	if ( ! desDecryptString( pw, data, &out ) )
	{
		if ( TYPE_STRING != varpw->type )
		{
			stringDestruct( pw );
		}
		if ( TYPE_STRING != vardata->type )
		{
			stringDestruct( data );
		}
		return IERR_OUT_OF_MEM;
	}
	ret->type = TYPE_STRING;
	ret->data.sp = stringConstructChar( (char *)out );
	free( out );

	if ( TYPE_STRING != varpw->type )
	{
		stringDestruct( pw );
	}
	if ( TYPE_STRING != vardata->type )
	{
		stringDestruct( data );
	}

	return IERR_NONE;
}


