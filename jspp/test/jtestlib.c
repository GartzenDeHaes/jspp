#ifdef HAVE_CONFIG_H
#include <jspp/config.h>
#endif
#include <spl/debug.h>
#include <jspp/jsocket.h>
#include <jspp/program.h>
#include <jspp/parse.h>
#include <jspp/jhttp_lib.h>

#if defined(DEBUG) || defined(_DEBUG)

void desCompTest(  )
{
	qinterp *qip;
	program *pp;
	enum InterpErrors err;
	variant *retval = NULL;
	bool prepareOk;
	char *src;

	logWriteCheck( NULL, "des compile/run test ... " );

	qip = qinterpConstruct();
	src = 
		"class test{\n" \
		"	public static main()\n{ var data = \"the time has come\"; var enc = DES::encrypt(\"xyzzy\", data); var dec = DES::decrypt(\"xyzzy\", enc); return dec == data; }\n" \
		"}\n";

	pp = parseChar( src, qinterpGetPrograms(qip) );
	/*programDump( pp, "c:\\temp\\descomp.txt" );*/

	UNIT_ASSERT( "desCompTest parse", pp->errornum == 0 );
	prepareOk = qinterpPrepare( qip, pp );
	UNIT_ASSERT( "desCompTest prepare", prepareOk );

	err = qinterpRun( qip, "test", "main", 0, &retval );
	UNIT_ASSERT( "desCompTest interp error", err == IERR_NONE );
	UNIT_ASSERT( "desCompTest return value", TYPE_BOOL == retval->type && retval->data.i == true );
	varDestruct( retval );

	DEBUG_CLEAR_MEM_CHECK_POINTS();
	qinterpCheckMem( qip );
	DEBUG_DUMP_MEM_LEAKS();
	UNIT_ASSERT_MEM_NOTED( "des comp/run test" );

	qinterpDestruct( qip );

	DEBUG_CLEAR_MEM_CHECK_POINTS();
	DEBUG_DUMP_MEM_LEAKS();
	UNIT_ASSERT_MEM_NOTED( "des comp/run test" );

	logWriteOkFail( NULL );
}

void httpReqTestJspp(  )
{
	httprequest *req;
	string *html = NULL;
	http_retcode ret;

	logWriteCheck( NULL, "httprequest test ... " );

	req = httprequestConstruct( "http://198.105.130.222", NULL, 0, UA_GENERIC );
	httprequestValidateMem( req );
	ret = httprequestGet( req, &html );	
	httprequestValidateMem( req );
	UNIT_ASSERT( "http ret code", ret == 0 );
	UNIT_ASSERT( "dor.wa.gov load", NULL != html && stringStartsWith(html, "<head xmlns=") );
	
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	if ( NULL != html )
	{
		stringCheckMem( html );
	}
	httprequestCheckMem( req );
	UNIT_ASSERT_MEM_NOTED( "httpreq test" );

	if ( NULL != html )
	{
		stringDestruct( html );
	}
	httprequestValidateMem( req );
	httprequestDestruct( req );

	socketShutdownService();

	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "httpreq test" );
	logWriteOkFail( NULL );
}

void httpReqCompTest(  )
{
	qinterp *qip;
	program *pp;
	enum InterpErrors err;
	variant *retval = NULL;
	bool prepareOk;
	char *src;

	logWriteCheck( NULL, "httprequest compile/run test ... " );

	qip = qinterpConstruct();
	src = 
		"class test{\n" \
		"	public static main()\n{var req = new HttpRequest(\"http://198.105.130.222\"); return req.get(); }\n" \
		"}\n";

	pp = parseChar( src, qinterpGetPrograms(qip) );
	/*programDump( pp, "c:\\temp\\httpReqCompTest.txt" );*/

	UNIT_ASSERT( "httpReqCompTest parse", pp->errornum == 0 );
	prepareOk = qinterpPrepare( qip, pp );
	UNIT_ASSERT( "httpReqCompTest prepare", prepareOk );

	err = qinterpRun( qip, "test", "main", 0, &retval );
	UNIT_ASSERT( "httpReqCompTest interp error", err == IERR_NONE );
	UNIT_ASSERT( "httpReqCompTest return value", TYPE_STRING == retval->type );
	UNIT_ASSERT( "httpReqCompTest result", stringStartsWith(retval->data.sp, "<html xmlns=") );
	varDestruct( retval );

	qinterpDestruct( qip );
	logWriteOkFail( NULL );
}

#endif

