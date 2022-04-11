#ifdef HAVE_CONFIG_H
#include <jspp/config.h>
#endif
#include <spl/debug.h>
#include <jspp/program.h>
#include <jspp/jsocket.h>
#include <jspp/jdes.h>
#include <jspp/jhttp_lib.h>

bool libPrepare( program *pp )
{
	jsclass *jsp;
	jsmethod *mp;

	/*
	 *  socket
	 */
	jsp = programDefineClass( pp, "Socket", "", NULL );
	if ( NULL == jsp )
	{
		programDestruct( pp );
		return false;
	}
	mp = programDefineMethod( pp, "@@constructor", true, ACC_PRIVATE );
	mp = programDefineMethod( pp, "@@constructor", false, ACC_PRIVATE );
	jsmethodAddArg( mp, "this", false );

	mp = programDefineNativeMethod( pp, socket_socket, "Socket", false, ACC_PUBLIC, false );
	jsmethodAddArg( mp, "this", false );
	jsmethodAddArg( mp, "port", false );

	mp = programDefineNativeMethod( pp, socket_accept, "accept", false, ACC_PUBLIC, true );
	jsmethodAddArg( mp, "this", false );
	mp = programDefineNativeMethod( pp, socket_connect, "connect", false, ACC_PUBLIC, true );
	jsmethodAddArg( mp, "this", false );
	jsmethodAddArg( mp, "addr", false );
	mp = programDefineNativeMethod( pp, socket_bind, "bind", false, ACC_PUBLIC, true );
	jsmethodAddArg( mp, "this", false );
	mp = programDefineNativeMethod( pp, socket_listen, "listen", false, ACC_PUBLIC, true );
	jsmethodAddArg( mp, "this", false );
	jsmethodAddArg( mp, "buffers", false );
	mp = programDefineNativeMethod( pp, socket_settimeout, "setTimeout", false, ACC_PUBLIC, true );
	jsmethodAddArg( mp, "this", false );
	jsmethodAddArg( mp, "recvto", false );
	jsmethodAddArg( mp, "sendto", false );
	mp = programDefineNativeMethod( pp, socket_setlingeron, "setLingerOn", false, ACC_PUBLIC, true );
	jsmethodAddArg( mp, "this", false );
	mp = programDefineNativeMethod( pp, socket_setnodelay, "setNoDelay", false, ACC_PUBLIC, true );
	jsmethodAddArg( mp, "this", false );
	mp = programDefineNativeMethod( pp, socket_send, "send", false, ACC_PUBLIC, true );
	jsmethodAddArg( mp, "this", false );
	jsmethodAddArg( mp, "str", false );
	jsmethodAddArg( mp, "len", false );
	mp = programDefineNativeMethod( pp, socket_recv, "recv", false, ACC_PUBLIC, true );
	jsmethodAddArg( mp, "this", false );
	mp = programDefineNativeMethod( pp, socket_flush, "flush", false, ACC_PUBLIC, false );
	jsmethodAddArg( mp, "this", false );
	mp = programDefineNativeMethod( pp, socket_error, "error", false, ACC_PUBLIC, true );
	jsmethodAddArg( mp, "this", false );
	mp = programDefineNativeMethod( pp, socket_setblocking, "setBlocking", false, ACC_PUBLIC, true );
	jsmethodAddArg( mp, "this", false );
	jsmethodAddArg( mp, "onoff", false );
	mp = programDefineNativeMethod( pp, socket_close, "close", false, ACC_PUBLIC, false );
	jsmethodAddArg( mp, "this", false );

	/*
	 *  DES
	 */
	jsp = programDefineClass( pp, "DES", "", NULL );
	if ( NULL == jsp )
	{
		programDestruct( pp );
		return false;
	}
	mp = programDefineMethod( pp, "@@constructor", true, ACC_PRIVATE );

	mp = programDefineNativeMethod( pp, des_encrypt, "encrypt", true, ACC_PUBLIC, true );
	jsmethodAddArg( mp, "pw", false );
	jsmethodAddArg( mp, "data", false );
	mp = programDefineNativeMethod( pp, des_decrypt, "decrypt", true, ACC_PUBLIC, true );
	jsmethodAddArg( mp, "pw", false );
	jsmethodAddArg( mp, "data", false );

	/*
	 *  httprequest
	 */
	jsp = programDefineClass( pp, "HttpRequest", "", NULL );
	if ( NULL == jsp )
	{
		programDestruct( pp );
		return false;
	}
	mp = programDefineMethod( pp, "@@constructor", true, ACC_PRIVATE );
	mp = programDefineMethod( pp, "@@constructor", false, ACC_PRIVATE );
	jsmethodAddArg( mp, "this", false );

	mp = programDefineNativeMethod( pp, httprequest_httprequest, "HttpRequest", false, ACC_PUBLIC, false );
	jsmethodAddArg( mp, "this", false );
	jsmethodAddArg( mp, "url", false );

	mp = programDefineNativeMethod( pp, httprequest_get, "get", false, ACC_PUBLIC, true );
	jsmethodAddArg( mp, "this", false );

	return true;
}

