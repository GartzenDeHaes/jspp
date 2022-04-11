#ifdef HAVE_CONFIG_H
#include <jspp/config.h>
#endif
#include <ctype.h>
#ifdef HAVE_STRING_H
#include <string.h>
#endif
#include <spl/debug.h>
#include <jspp/jsocket.h>
#include <jspp/native.h>


#define SOCKET_MAJIC ((byte)0x2)

void socket_finializer( void *vadp )
{
	csocket *sock = (csocket *)vadp;
	socketValidateMem( sock );
	socketDestruct( sock );
}

void socket_checkmem( void *vadp )
{
	csocket *sock = (csocket *)vadp;
	DEBUG_NOTE_MEM( sock );
	socketCheckMem( sock );
}

enum InterpErrors socket_socket(qinterp *qip, const void *mp, variant *ret)
{
	variant *varthis = getArgumentN( qip, 0 );
	variant *varport = getArgumentN( qip, 1 );
	jsinst *inst = varthis->data.ip;
	csocket *sock;
	int port;

	ASSERT( getArgumentCount( qip ) == 2 );
	ASSERT_MEM( mp, sizeof(struct jsmethod) );
	varValidateMem( varthis );
	ASSERT( TYPE_INST == varthis->type );
	varValidateMem( varport );
	varValidateMem( ret );

	if ( ! varGetInt( varport, &port ) )
	{
		return IERR_TYPE_MISMATCH;
	}
	inst->nativedata = malloc( sizeof(struct nativedataHolder) );
	if ( NULL == inst->nativedata )
	{
		return IERR_OUT_OF_MEM;
	}
	inst->nativedata->data = sock = socketConstruct( port );
	if ( NULL == sock )
	{
		return IERR_OUT_OF_MEM;
	}
	inst->nativedata->finializer = socket_finializer;
	inst->nativedata->memcheck = socket_checkmem;
	inst->magic = SOCKET_MAJIC;
	return IERR_NONE;
}

enum InterpErrors socket_accept(qinterp *qip, const void *mp, variant *ret)
{
	variant *varthis = getArgumentN( qip, 0 );
	variant *varaddr = getArgumentN( qip, 1 );
	jsinst *inst = varthis->data.ip;
	jsinst *instret;
	csocket *clnt;
	enum InterpErrors err;
	string *classname;
	variant *vararg;

	ASSERT( getArgumentCount( qip ) == 2 );
	ASSERT_MEM( mp, sizeof(struct jsmethod) );
	varValidateMem( varthis );
	ASSERT( TYPE_INST == varthis->type );
	ASSERT( SOCKET_MAJIC == varthis->data.ip->magic );
	varValidateMem( varaddr );
	varValidateMem( ret );

	if ( TYPE_STRING != varaddr->type )
	{
		return IERR_TYPE_MISMATCH;
	}
	classname = stringConstructChar( "Socket" );
	if ( NULL == classname )
	{
		return IERR_OUT_OF_MEM;
	}
	vararg = varConstruct();
	if ( NULL == vararg )
	{
		stringDestruct( classname );
		return IERR_OUT_OF_MEM;
	}
	clnt = socketAccept( (csocket *)inst->nativedata->data );
	if ( NULL == clnt )
	{
		stringDestruct( classname );
		varDestruct( vararg );
		return IERR_OUT_OF_MEM;
	}
	varAssignInt( vararg, 0 );
	err = createJSClassInstance( qip, classname, &instret, 1, vararg );
	if ( IERR_NONE == err )
	{
		jsinstValidateMem( instret );
		socketDestruct( (csocket *)instret->nativedata->data );
		instret->nativedata->data = clnt;
		varAssignInst( ret, instret );
		ASSERT( 1 == instret->refcount );
	}
	stringDestruct( classname );
	varDestruct( vararg );


	return err;
}

enum InterpErrors socket_connect(qinterp *qip, const void *mp, variant *ret)
{
	variant *varthis = getArgumentN( qip, 0 );
	variant *varaddr = getArgumentN( qip, 1 );
	jsinst *inst = varthis->data.ip;

	ASSERT( getArgumentCount( qip ) == 2 );
	ASSERT_MEM( mp, sizeof(struct jsmethod) );
	varValidateMem( varthis );
	ASSERT( TYPE_INST == varthis->type );
	ASSERT( SOCKET_MAJIC == varthis->data.ip->magic );
	varValidateMem( varaddr );
	varValidateMem( ret );

	if ( TYPE_STRING != varaddr->type )
	{
		return IERR_TYPE_MISMATCH;
	}
	varAssignBool( ret, socketConnect( (csocket *)inst->nativedata->data, varaddr->data.sp->cstr ) );

	return IERR_NONE;
}

enum InterpErrors socket_bind(qinterp *qip, const void *mp, variant *ret)
{
	variant *varthis = getArgumentN( qip, 0 );
	jsinst *inst = varthis->data.ip;
	csocket *sock = inst->nativedata->data;

	ASSERT( getArgumentCount( qip ) == 1 );
	ASSERT_MEM( mp, sizeof(struct jsmethod) );
	varValidateMem( varthis );
	ASSERT( TYPE_INST == varthis->type );
	ASSERT( SOCKET_MAJIC == varthis->data.ip->magic );
	varValidateMem( ret );
	socketValidateMem( sock );

	varAssignBool( ret, socketBind( sock ) );

	return IERR_NONE;
}

enum InterpErrors socket_listen(qinterp *qip, const void *mp, variant *ret)
{
	variant *varthis = getArgumentN( qip, 0 );
	variant *varbufs = getArgumentN( qip, 1 );
	jsinst *inst = varthis->data.ip;
	csocket *sock = inst->nativedata->data;
	int numbufs;

	ASSERT( getArgumentCount( qip ) == 2 );
	ASSERT_MEM( mp, sizeof(struct jsmethod) );
	varValidateMem( varthis );
	ASSERT( TYPE_INST == varthis->type );
	ASSERT( SOCKET_MAJIC == varthis->data.ip->magic );
	varValidateMem( varbufs );
	varValidateMem( ret );
	socketValidateMem( sock );

	if ( ! varGetInt( varbufs, &numbufs ) )
	{
		return IERR_TYPE_MISMATCH;
	}
	varAssignBool( ret, socketListen( sock, numbufs ) );

	return IERR_NONE;
}

enum InterpErrors socket_settimeout(qinterp *qip, const void *mp, variant *ret)
{
	variant *varthis = getArgumentN( qip, 0 );
	variant *varrecv = getArgumentN( qip, 1 );
	variant *varsend = getArgumentN( qip, 2 );
	jsinst *inst = varthis->data.ip;
	csocket *sock = inst->nativedata->data;
	int rto;
	int sto;

	ASSERT( getArgumentCount( qip ) == 3 );
	ASSERT_MEM( mp, sizeof(struct jsmethod) );
	varValidateMem( varthis );
	ASSERT( TYPE_INST == varthis->type );
	ASSERT( SOCKET_MAJIC == varthis->data.ip->magic );
	varValidateMem( varrecv );
	varValidateMem( varsend );
	varValidateMem( ret );
	socketValidateMem( sock );

	if ( ! varGetInt( varrecv, &rto ) )
	{
		return IERR_TYPE_MISMATCH;
	}
	if ( ! varGetInt( varsend, &sto ) )
	{
		return IERR_TYPE_MISMATCH;
	}
	varAssignBool( ret, socketSetSendTimeout( sock, sto ) || socketSetRecvTimeout( sock, rto ) );

	return IERR_NONE;
}

enum InterpErrors socket_setlingeron(qinterp *qip, const void *mp, variant *ret)
{
	variant *varthis = getArgumentN( qip, 0 );
	jsinst *inst = varthis->data.ip;
	csocket *sock = inst->nativedata->data;

	ASSERT( getArgumentCount( qip ) == 1 );
	ASSERT_MEM( mp, sizeof(struct jsmethod) );
	varValidateMem( varthis );
	ASSERT( TYPE_INST == varthis->type );
	ASSERT( SOCKET_MAJIC == varthis->data.ip->magic );
	varValidateMem( ret );
	socketValidateMem( sock );

	varAssignBool( ret, socketSetLingerOn( sock ) );

	return IERR_NONE;
}

enum InterpErrors socket_setnodelay(qinterp *qip, const void *mp, variant *ret)
{
	variant *varthis = getArgumentN( qip, 0 );
	jsinst *inst = varthis->data.ip;
	csocket *sock = inst->nativedata->data;

	ASSERT( getArgumentCount( qip ) == 1 );
	ASSERT_MEM( mp, sizeof(struct jsmethod) );
	varValidateMem( varthis );
	ASSERT( TYPE_INST == varthis->type );
	ASSERT( SOCKET_MAJIC == varthis->data.ip->magic );
	varValidateMem( ret );
	socketValidateMem( sock );

	varAssignBool( ret, socketSetNoDelay( sock ) );

	return IERR_NONE;
}

enum InterpErrors socket_send(qinterp *qip, const void *mp, variant *ret)
{
	variant *varthis = getArgumentN( qip, 0 );
	variant *vardata = getArgumentN( qip, 1 );
	variant *varlen = getArgumentN( qip, 2 );
	jsinst *inst = varthis->data.ip;
	int len;

	ASSERT( getArgumentCount( qip ) == 3 );
	ASSERT_MEM( mp, sizeof(struct jsmethod) );
	varValidateMem( varthis );
	ASSERT( TYPE_INST == varthis->type );
	ASSERT( SOCKET_MAJIC == varthis->data.ip->magic );
	varValidateMem( vardata );
	varValidateMem( varlen );
	varValidateMem( ret );
	socketValidateMem( (csocket *)inst->nativedata->data );

	if ( ! varGetInt( varlen, &len ) )
	{
		return IERR_TYPE_MISMATCH;
	}
	if ( TYPE_STRING == vardata->type )
	{
		if ( len > vardata->data.sp->len )
		{
			return IERR_INDEX_OUT_OF_BOUNDS;
		}
		varAssignBool( ret, socketSend( (csocket *)inst->nativedata->data, (byte *)vardata->data.sp->cstr, len ) );
	}
	else
	{
		string *str = varToString( vardata );
		if ( NULL == str )
		{
			return IERR_OUT_OF_MEM;
		}
		if ( len > str->len )
		{
			stringDestruct( str );
			return IERR_INDEX_OUT_OF_BOUNDS;
		}
		varAssignBool( ret, socketSend( (csocket *)inst->nativedata->data, (byte *)str->cstr, len ) );
		stringDestruct( str );
	}
	return IERR_NONE;
}

enum InterpErrors socket_recv(qinterp *qip, const void *mp, variant *ret)
{
	variant *varthis = getArgumentN( qip, 0 );
	jsinst *inst = varthis->data.ip;
	csocket *sock = inst->nativedata->data;
	byte buf[256];
	int received;

	ASSERT( getArgumentCount( qip ) == 1 );
	ASSERT_MEM( mp, sizeof(struct jsmethod) );
	varValidateMem( varthis );
	ASSERT( TYPE_INST == varthis->type );
	ASSERT( SOCKET_MAJIC == varthis->data.ip->magic );
	varValidateMem( ret );
	socketValidateMem( sock );

	received = socketRecv( sock, buf, 255 );
	if ( received >= 0 )
	{
		buf[received] = '\0';
	}
	else
	{
		buf[0] = '\0';
#if _WIN32
		sock->m_errorStatus = WSAGetLastError();
#else
		sock->m_errorStatus = errno;
#endif
	}
	ASSERT( sizeof(char) == sizeof(byte) );
	if ( ! varAssignChar( ret, (char *)buf ) )
	{
		return IERR_OUT_OF_MEM;
	}
	return IERR_NONE;
}

enum InterpErrors socket_flush(qinterp *qip, const void *mp, variant *ret)
{
	variant *varthis = getArgumentN( qip, 0 );
	jsinst *inst = varthis->data.ip;
	csocket *sock = inst->nativedata->data;

	ASSERT( getArgumentCount( qip ) == 1 );
	ASSERT_MEM( mp, sizeof(struct jsmethod) );
	varValidateMem( varthis );
	ASSERT( TYPE_INST == varthis->type );
	ASSERT( SOCKET_MAJIC == varthis->data.ip->magic );
	varValidateMem( ret );
	socketValidateMem( sock );

	socketFlush( sock );

	return IERR_NONE;
}

enum InterpErrors socket_error(qinterp *qip, const void *mp, variant *ret)
{
	variant *varthis = getArgumentN( qip, 0 );
	jsinst *inst = varthis->data.ip;
	csocket *sock = inst->nativedata->data;

	ASSERT( getArgumentCount( qip ) == 1 );
	ASSERT_MEM( mp, sizeof(struct jsmethod) );
	varValidateMem( varthis );
	ASSERT( TYPE_INST == varthis->type );
	ASSERT( SOCKET_MAJIC == varthis->data.ip->magic );
	varValidateMem( ret );
	socketValidateMem( sock );

	varAssignInt( ret, sock->m_errorStatus );

	return IERR_NONE;
}

enum InterpErrors socket_setblocking(qinterp *qip, const void *mp, variant *ret)
{
	variant *varthis = getArgumentN( qip, 0 );
	variant *vardata = getArgumentN( qip, 1 );
	jsinst *inst = varthis->data.ip;
	int onoff;

	ASSERT( getArgumentCount( qip ) == 2 );
	ASSERT_MEM( mp, sizeof(struct jsmethod) );
	varValidateMem( varthis );
	ASSERT( TYPE_INST == varthis->type );
	ASSERT( SOCKET_MAJIC == varthis->data.ip->magic );
	varValidateMem( vardata );
	varValidateMem( ret );
	socketValidateMem( (csocket *)inst->nativedata->data );

	if ( ! varGetInt( vardata, &onoff ) )
	{
		return IERR_TYPE_MISMATCH;
	}
	if ( onoff )
	{
		varAssignBool( ret, socketSetBlocking( (csocket *)inst->nativedata->data ) );
	}
	else
	{
		varAssignBool( ret, socketSetNonBlocking( (csocket *)inst->nativedata->data ) );
	}
	return IERR_NONE;
}

enum InterpErrors socket_close(qinterp *qip, const void *mp, variant *ret)
{
	variant *varthis = getArgumentN( qip, 0 );
	jsinst *inst = varthis->data.ip;
	csocket *sock = inst->nativedata->data;

	ASSERT( getArgumentCount( qip ) == 1 );
	ASSERT_MEM( mp, sizeof(struct jsmethod) );
	varValidateMem( varthis );
	ASSERT( TYPE_INST == varthis->type );
	ASSERT( SOCKET_MAJIC == varthis->data.ip->magic );
	varValidateMem( ret );
	socketValidateMem( sock );

	socketClose( sock );

	return IERR_NONE;
}



