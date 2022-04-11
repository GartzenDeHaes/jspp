#include <string.h>
#include <spl/debug.h>
#include <jspp/parse.h>
#include <jspp/interp.h>

#ifdef DEBUG


static void compTest00(  )
{
	qinterp *qip;
	program *pp;
	enum InterpErrors err;
	variant *retval = NULL;
	bool prepareOk;
	char *src;

	logWriteCheck( NULL, "compile/run test 00 ... " );

	qip = qinterpConstruct();
	src = 
		"class test{\n" \
		"	public static main()\n{ var a = 1; while( a++ < 3 ) {} return a; }\n" \
		"	public static main2()\n{ var a = 1; while( ++a < 3 ) {} return a; }\n" \
		"}\n";

	pp = parseChar( src, qinterpGetPrograms(qip) );
	/*programDump( pp, "c:\\temp\\testcomp00.txt" );*/

	UNIT_ASSERT( "compTest00 parse", pp->errornum == 0 );
	prepareOk = qinterpPrepare( qip, pp );
	UNIT_ASSERT( "compTest00 prepare", prepareOk );

	err = qinterpRun( qip, "test", "main", 0, &retval );
	UNIT_ASSERT( "compTest00 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest00 return value", retval->data.i == 4 );
	varDestruct( retval );

	err = qinterpRun( qip, "test", "main2", 0, &retval );
	UNIT_ASSERT( "compTest00 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest00 return value", retval->data.i == 3 );
	varDestruct( retval );

	qinterpDestruct( qip );
	logWriteOkFail(NULL);
}

static void compTest0(  )
{
	qinterp *qip;
	program *pp;
	enum InterpErrors err;
	variant *retval = NULL;
	bool prepareOk;
	char *src;

	logWriteCheck( NULL, "compile/run test 0 ... " );

	qip = qinterpConstruct();
	src = 
		"class test{\n" \
		"	private static var y = \"data\";\n" \
		"	public callme2(a, b) {return a + b;}\n" \
		"	public static main()\n{ var a = new test(); return a.callme2(test::y, test::y.length()); }\n" \
		"}\n";

	pp = parseChar( src, qinterpGetPrograms(qip) );
	/*programDump( pp, "c:\\temp\\testcomp0.txt" );*/

	UNIT_ASSERT( "compTest0 parse", pp->errornum == 0 );
	prepareOk = qinterpPrepare( qip, pp );
	UNIT_ASSERT( "compTest0 prepare", prepareOk );

	err = qinterpRun( qip, "test", "main", 0, &retval );
	UNIT_ASSERT( "compTest0 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest0 return value", TYPE_STRING == retval->type && strcmp(retval->data.sp->cstr, "data4") == 0 );
	varDestruct( retval );

	qinterpDestruct( qip );
	logWriteOkFail(NULL);
}

static void compTest1(  )
{
	qinterp *qip;
	program *pp;
	enum InterpErrors err;
	variant *retval = NULL;
	bool prepareOk;

	logWriteCheck( NULL, "compile/run test 1 ... " );

	qip = qinterpConstruct();
	pp = parseChar( "class test {public static main(){return 1;}}", NULL );

	UNIT_ASSERT( "compTest1 parse", pp->errornum == 0 );

	prepareOk = qinterpPrepare( qip, pp );
	UNIT_ASSERT( "compTest1 prepare", prepareOk );
	err = qinterpRun( qip, "test", "main", 0, &retval );
	UNIT_ASSERT( "compTest1 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest1 return value not NULL", NULL != retval );
	UNIT_ASSERT( "compTest1 return value", retval->data.i == 1 );

	if ( NULL != retval )
	{
		varDestruct( retval );
	}
	qinterpDestruct( qip );
	logWriteOkFail(NULL);
}

static void compTest2(  )
{
	qinterp *qip;
	program *pp;
	enum InterpErrors err;
	variant *retval = NULL;
	bool prepareOk;

	logWriteCheck( NULL, "compile/run test 2 ... " );

	qip = qinterpConstruct();
	pp = parseChar( "class test {public static main(){return 1+1;}}", NULL );

	UNIT_ASSERT( "compTest2 parse", pp->errornum == 0 );

	prepareOk = qinterpPrepare( qip, pp );
	UNIT_ASSERT( "compTest2 prepare", prepareOk );
	err = qinterpRun( qip, "test", "main", 0, &retval );
	UNIT_ASSERT( "compTest2 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest2 return value not NULL", NULL != retval );
	UNIT_ASSERT( "compTest2 return value", retval->data.i == 2 );

	if ( NULL != retval )
	{
		varDestruct( retval );
	}
	qinterpDestruct( qip );
	logWriteOkFail(NULL);
}

static void compTest3(  )
{
	qinterp *qip;
	program *pp;
	enum InterpErrors err;
	variant *retval = NULL;
	bool prepareOk;

	logWriteCheck( NULL, "compile/run test 3 ... " );

	qip = qinterpConstruct();
	pp = parseChar( "class test {public static main(){return 3-1;}}", NULL );

	UNIT_ASSERT( "compTest3 parse", pp->errornum == 0 );

	prepareOk = qinterpPrepare( qip, pp );
	UNIT_ASSERT( "compTest3 prepare", prepareOk );
	err = qinterpRun( qip, "test", "main", 0, &retval );
	UNIT_ASSERT( "compTest3 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest3 return value not NULL", NULL != retval );
	UNIT_ASSERT( "compTest3 return value", retval->data.i == 2 );

	if ( NULL != retval )
	{
		varDestruct( retval );
	}
	qinterpDestruct( qip );
	logWriteOkFail(NULL);
}

static void compTest4(  )
{
	qinterp *qip;
	program *pp;
	enum InterpErrors err;
	variant *retval = NULL;
	bool prepareOk;

	logWriteCheck( NULL, "compile/run test 4 ... " );

	qip = qinterpConstruct();
	pp = parseChar( "class test {public static main(){return 3*2;}}", NULL );

	UNIT_ASSERT( "compTest4 parse", pp->errornum == 0 );

	prepareOk = qinterpPrepare( qip, pp );
	UNIT_ASSERT( "compTest4 prepare", prepareOk );
	err = qinterpRun( qip, "test", "main", 0, &retval );
	UNIT_ASSERT( "compTest4 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest4 return value not NULL", NULL != retval );
	UNIT_ASSERT( "compTest4 return value", retval->data.i == 6 );

	if ( NULL != retval )
	{
		varDestruct( retval );
	}
	qinterpDestruct( qip );
	logWriteOkFail(NULL);
}

static void compTest5(  )
{
	qinterp *qip;
	program *pp;
	enum InterpErrors err;
	variant *retval = NULL;
	bool prepareOk;

	logWriteCheck( NULL, "compile/run test 5 ... " );

	qip = qinterpConstruct();
	pp = parseChar( 
		"class test { " \
		"	private static const var x = 2; " \
		"\n" \
		"	public static main()" \
		"	{" \
		"		return x*2;" \
		"	}\n " \
		"	public static main2() {var x; var y = x = 1; return y;}\n" \
		"}" , NULL );

	UNIT_ASSERT( "compTest5 parse", pp->errornum == 0 );

	prepareOk = qinterpPrepare( qip, pp );
	UNIT_ASSERT( "compTest5 prepare", prepareOk );

	err = qinterpRun( qip, "test", "main", 0, &retval );
	UNIT_ASSERT( "compTest5 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest5 return value not NULL", NULL != retval );
	UNIT_ASSERT( "compTest5 return value", retval->data.i == 4 );
	varDestruct( retval );

	/*err = qinterpRun( qip, "test", "main2", &retval );
	UNIT_ASSERT( "compTest5 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest5 return value not NULL", NULL != retval );
	UNIT_ASSERT( "compTest5 return value", retval->data.i == 1 )
	varDestruct( retval );
	*/
	qinterpDestruct( qip );
	logWriteOkFail(NULL);
}

static void compTest6(  )
{
	qinterp *qip;
	program *pp;
	enum InterpErrors err;
	variant *retval = NULL;
	bool prepareOk;

	logWriteCheck( NULL, "compile/run test 6 ... " );

	qip = qinterpConstruct();
	pp = parseChar( 
		"class test { " \
		"	private static const var x = 2; " \
		"	private static const var y = 3; " \
		"\n" \
		"	public static main()" \
		"	{" \
		"		return x*y;" \
		"	} " \
		"}" , NULL );

	UNIT_ASSERT( "compTest6 parse", pp->errornum == 0 );

	prepareOk = qinterpPrepare( qip, pp );
	UNIT_ASSERT( "compTest6 prepare", prepareOk );
	err = qinterpRun( qip, "test", "main", 0, &retval );
	UNIT_ASSERT( "compTest6 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest6 return value not NULL", NULL != retval );
	UNIT_ASSERT( "compTest6 return value", retval->data.i == 6 );

	if ( NULL != retval )
	{
		varDestruct( retval );
	}
	qinterpDestruct( qip );
	logWriteOkFail(NULL);
}

static void compTest7(  )
{
	qinterp *qip;
	program *pp;
	enum InterpErrors err;
	variant *retval = NULL;
	bool prepareOk;
	char *src;

	logWriteCheck( NULL, "compile/run test 7 ... " );

	qip = qinterpConstruct();
	src = 
		"class test { " \
		"	private static var x; " \
		"	private static const var y = 3; " \
		"\n" \
		"	public static main()" \
		"	{" \
		"		x = y * 2; " \
		"		return x;" \
		"	} " \
		"	public static gety()" \
		"	{" \
		"		return y; " \
		"	}" \
		"}";

	pp = parseChar( src, NULL );

	UNIT_ASSERT( "compTest7 parse", pp->errornum == 0 );

	prepareOk = qinterpPrepare( qip, pp );
	UNIT_ASSERT( "compTest7 prepare", prepareOk );
	err = qinterpRun( qip, "test", "main", 0, &retval );
	UNIT_ASSERT( "compTest7 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest7 return value not NULL", NULL != retval );
	UNIT_ASSERT( "compTest7 return value", retval->data.i == 6 );
	if ( NULL != retval )
	{
		varDestruct( retval );
	}

	err = qinterpRun( qip, "test", "gety", 0, &retval );
	UNIT_ASSERT( "compTest7 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest7 return value not NULL", NULL != retval );
	UNIT_ASSERT( "compTest7 return value", retval->data.i == 3 );

	if ( NULL != retval )
	{
		varDestruct( retval );
	}
	qinterpDestruct( qip );
	logWriteOkFail(NULL);
}

static void compTest7_1a(  )
{
	qinterp *qip;
	program *pp;
	enum InterpErrors err;
	variant *retval = NULL;
	bool prepareOk;
	char *src;

	logWriteCheck( NULL, "compile/run test 7_1a ... " );

	qip = qinterpConstruct();
	src = 
		"class test {\n" \
		"	private static var x; " \
		"\n" \
		"	public test() {}\n" \
		"	public static main()" \
		"	{" \
		"		var t = new test();\n" \
		"		return t;" \
		"	} " \
		"}";

	pp = parseChar( src, NULL );

	UNIT_ASSERT( "compTest71 parse", pp->errornum == 0 );

	prepareOk = qinterpPrepare( qip, pp );
	UNIT_ASSERT( "compTest71 prepare", prepareOk );
	err = qinterpRun( qip, "test", "main", 0, &retval );
	UNIT_ASSERT( "compTest71 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest71 return value not NULL", NULL != retval );
	UNIT_ASSERT( "compTest71 return value", strcmp(retval->data.ip->myclass->name->cstr, "test") == 0 );
	varDestruct( retval );

	qinterpDestruct( qip );
	logWriteOkFail(NULL);
}

static void compTest7_1b(  )
{
	qinterp *qip;
	program *pp;
	enum InterpErrors err;
	variant *retval = NULL;
	bool prepareOk;
	char *src;

	logWriteCheck( NULL, "compile/run test 7_1b ... " );

	qip = qinterpConstruct();
	src = 
		"class test { " \
		"	private static var x; " \
		"	private static const var y = 3; " \
		"\n" \
		"	public test() {}\n" \
		"	public gety()" \
		"	{" \
		"		return y; " \
		"	}" \
		"	public static main()" \
		"	{" \
		"		var t = new test();\n" \
		"		x = t.gety() * 2; " \
		"		return x;" \
		"	} " \
		"}";

	pp = parseChar( src, NULL );

	UNIT_ASSERT( "compTest71 parse", pp->errornum == 0 );

	prepareOk = qinterpPrepare( qip, pp );
	UNIT_ASSERT( "compTest71 prepare", prepareOk );
	err = qinterpRun( qip, "test", "main", 0, &retval );
	UNIT_ASSERT( "compTest71 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest71 return value not NULL", NULL != retval );
	UNIT_ASSERT( "compTest71 return value", retval->data.i == 6 );
	varDestruct( retval );

	qinterpDestruct( qip );
	logWriteOkFail(NULL);
}

static void compTest7_1c(  )
{
	qinterp *qip;
	program *pp;
	enum InterpErrors err;
	variant *retval = NULL;
	bool prepareOk;
	char *src;

	logWriteCheck( NULL, "compile/run test 7_1c ... " );

	qip = qinterpConstruct();
	src = 
		"class test {\n" \
		"	private static var x = 1; " \
		"\n" \
		"	public test() {}\n" \
		"	public static main()" \
		"	{" \
		"		var t = new test();\n" \
		"		return t.x;" \
		"	} " \
		"}";

	pp = parseChar( src, NULL );

	UNIT_ASSERT( "compTest71 parse", pp->errornum == 0 );

	prepareOk = qinterpPrepare( qip, pp );
	UNIT_ASSERT( "compTest71c prepare", prepareOk );
	err = qinterpRun( qip, "test", "main", 0, &retval );
	UNIT_ASSERT( "compTest71c interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest71c return value not NULL", NULL != retval );
	UNIT_ASSERT( "compTest71c return value", retval->data.i == 1 );
	varDestruct( retval );

	qinterpDestruct( qip );
	logWriteOkFail(NULL);
}

static void compTest8(  )
{
	qinterp *qip;
	program *pp;
	enum InterpErrors err;
	variant *retval = NULL;
	bool prepareOk;
	char *src;

	logWriteCheck( NULL, "compile/run test 8 ... " );

	qip = qinterpConstruct();
	src = 
		"class factorial { " \
		"\n" \
		"	public static n(i)\n" \
		"	{\n" \
		"		if ( i <= 1 ) \n" \
		"		{\n" \
		"			return 1; \n" \
		"		}\n" \
		"		return i * factorial::n(i-1); \n" \
		"	}\n" \
		"	public static main()\n" \
		"	{\n" \
		"		return factorial::n(1);\n" \
		"	} \n" \
		"}\n";

	pp = parseChar( src, NULL );

	UNIT_ASSERT( "compTest8 parse", pp->errornum == 0 );

	prepareOk = qinterpPrepare( qip, pp );
	UNIT_ASSERT( "compTest8 prepare", prepareOk );
	err = qinterpRun( qip, "factorial", "main", 0, &retval );
	UNIT_ASSERT( "compTest8 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest8 return value not NULL", NULL != retval );
	UNIT_ASSERT( "compTest8 return value", retval->data.i == 1 );
	if ( NULL != retval )
	{
		varDestruct( retval );
	}
	qinterpDestruct( qip );
	logWriteOkFail(NULL);
}

static void compTest9(  )
{
	qinterp *qip;
	program *pp;
	enum InterpErrors err;
	variant *retval = NULL;
	bool prepareOk;
	char *src;

	logWriteCheck( NULL, "compile/run test 9 ... " );

	qip = qinterpConstruct();
	src = 
		"class factorial { " \
		"\n" \
		"	public static n(i)\n" \
		"	{\n" \
		"		if ( i <= 1 ) \n" \
		"		{\n" \
		"			return 1; \n" \
		"		}\n" \
		"		return i * factorial::n(i-1); \n" \
		"	}\n" \
		"	public static main()\n" \
		"	{\n" \
		"		return factorial::n(2);\n" \
		"	} \n" \
		"}\n";

	pp = parseChar( src, NULL );

	UNIT_ASSERT( "compTest9 parse", pp->errornum == 0 );

	prepareOk = qinterpPrepare( qip, pp );
	UNIT_ASSERT( "compTest9 prepare", prepareOk );
	err = qinterpRun( qip, "factorial", "main", 0, &retval );
	UNIT_ASSERT( "compTest9 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest9 return value not NULL", NULL != retval );
	UNIT_ASSERT( "compTest9 return value", retval->data.i == 2 );
	if ( NULL != retval )
	{
		varDestruct( retval );
	}
	qinterpDestruct( qip );
	logWriteOkFail(NULL);
}

static void compTest10(  )
{
	qinterp *qip;
	program *pp;
	enum InterpErrors err;
	variant *retval = NULL;
	bool prepareOk;
	char *src;

	logWriteCheck( NULL, "compile/run test 10 ... " );

	qip = qinterpConstruct();
	src = 
		"class factorial { " \
		"\n" \
		"	public static n(i)\n" \
		"	{\n" \
		"		if ( i <= 1 ) \n" \
		"		{\n" \
		"			return 1; \n" \
		"		}\n" \
		"		return i * factorial::n(i-1); \n" \
		"	}\n" \
		"	public static main()\n" \
		"	{\n" \
		"		return factorial::n(3);\n" \
		"	} \n" \
		"}\n";

	pp = parseChar( src, NULL );

	UNIT_ASSERT( "compTest910 parse", pp->errornum == 0 );

	prepareOk = qinterpPrepare( qip, pp );
	UNIT_ASSERT( "compTest10 prepare", prepareOk );
	err = qinterpRun( qip, "factorial", "main", 0, &retval );
	UNIT_ASSERT( "compTest10 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest10 return value not NULL", NULL != retval );
	UNIT_ASSERT( "compTest10 return value", retval->data.i == 6 );
	if ( NULL != retval )
	{
		varDestruct( retval );
	}
	qinterpDestruct( qip );
	logWriteOkFail(NULL);
}


static void compTest11(  )
{
	qinterp *qip;
	program *pp;
	enum InterpErrors err;
	variant *retval = NULL;
	bool prepareOk;
	char *src;

	logWriteCheck( NULL, "compile/run test 11 ... " );

	qip = qinterpConstruct();
	src = 
		"class factorial { " \
		"\n" \
		"	public factorial(){}" \
		"\n" \
		"	public n(i)\n" \
		"	{\n" \
		"		if ( i <= 1 ) \n" \
		"		{\n" \
		"			return 1; \n" \
		"		}\n" \
		"		return i * n(i-1); \n" \
		"	}\n" \
		"}\n" \
		"class main {\n" \
		"	public static main1()\n" \
		"	{\n" \
		"		var fac = new factorial();" \
		"		return fac.n(1);\n" \
		"	} \n" \
		"	public static main2()\n" \
		"	{\n" \
		"		var fac = new factorial();" \
		"		return fac.n(2);\n" \
		"	} \n" \
		"	public static main3()\n" \
		"	{\n" \
		"		var fac = new factorial();" \
		"		return fac.n(3);\n" \
		"	} \n" \
		"}\n";

	pp = parseChar( src, NULL );

	UNIT_ASSERT( "compTest11 parse", pp->errornum == 0 );

	prepareOk = qinterpPrepare( qip, pp );
	UNIT_ASSERT( "compTest11 prepare", prepareOk );
	err = qinterpRun( qip, "main", "main1", 0, &retval );
	UNIT_ASSERT( "compTest11 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest11 return value not NULL", NULL != retval );
	UNIT_ASSERT( "compTest11 return value", retval->data.i == 1 );
	if ( NULL != retval )
	{
		varDestruct( retval );
	}
	err = qinterpRun( qip, "main", "main2", 0, &retval );
	UNIT_ASSERT( "compTest11 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest11 return value not NULL", NULL != retval );
	UNIT_ASSERT( "compTest11 return value", retval->data.i == 2 );
	if ( NULL != retval )
	{
		varDestruct( retval );
	}
	err = qinterpRun( qip, "main", "main3", 0, &retval );
	UNIT_ASSERT( "compTest11 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest11 return value not NULL", NULL != retval );
	UNIT_ASSERT( "compTest11 return value", retval->data.i == 6 );
	if ( NULL != retval )
	{
		varDestruct( retval );
	}
	qinterpDestruct( qip );
	logWriteOkFail(NULL);
}

static void compTest12(  )
{
	qinterp *qip;
	program *pp;
	enum InterpErrors err;
	variant *retval = NULL;
	bool prepareOk;
	char *src;

	logWriteCheck( NULL, "compile/run test 12 ... " );

	qip = qinterpConstruct();
	src = 
		"class factorial { " \
		"\n" \
		"	public var result;\n" \
		"\n" \
		"	public factorial(i) {result = n(i);}" \
		"\n" \
		"	public n(i)\n" \
		"	{\n" \
		"		if ( i <= 1 ) \n" \
		"		{\n" \
		"			return 1; \n" \
		"		}\n" \
		"		return i * n(i-1); \n" \
		"	}\n" \
		"}\n" \
		"class main {\n" \
		"	public static main3()\n" \
		"	{\n" \
		"		var fac = new factorial(3);" \
		"		return fac.result;\n" \
		"	} \n" \
		"}\n";

	pp = parseChar( src, NULL );

	UNIT_ASSERT( "compTest12 parse", pp->errornum == 0 );

	prepareOk = qinterpPrepare( qip, pp );
	UNIT_ASSERT( "compTest12 prepare", prepareOk );
	err = qinterpRun( qip, "main", "main3", 0, &retval );
	UNIT_ASSERT( "compTest12 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest12 return value not NULL", NULL != retval );
	UNIT_ASSERT( "compTest12 return value", retval->data.i == 6 );
	if ( NULL != retval )
	{
		varDestruct( retval );
	}
	qinterpDestruct( qip );
	logWriteOkFail(NULL);
}

static void compTest13(  )
{
	qinterp *qip;
	program *pp;
	enum InterpErrors err;
	variant *retval = NULL;
	bool prepareOk;
	char *src;

	logWriteCheck( NULL, "compile/run test 13 ... " );

	qip = qinterpConstruct();
	src = 
		"class maths { \n" \
		"	public static plus() {return 99 + 1;}\n" \
		"	public static minus() {return 99-1;}\n" \
		"	public static times() {return 50*2;}\n" \
		"	public static div() {return 4/2;}\n" \
		"	public static mod() {return 9%3;}\n" \
		"	public static or() {return 2|1;}\n" \
		"	public static and() {return 2&1;}\n" \
		"	public static xor() {return 2^1;}\n" \
		"	public static not() {return !1;}\n" \
		"	public static inc() {var x = 1; return x++;}\n" \
		"	public static dec() {var x = 1; return x--;}\n" \
		"	public static pinc() {var x = 1; return ++x;}\n" \
		"	public static pdec() {var x = 1; return --x;}\n" \
		"	public static inc2() {var x = 1; x++; return x;}\n" \
		"	public static dec2() {var x = 1; x--; return x;}\n" \
		"	public static pinc2() {var x = 1; ++x; return x;}\n" \
		"	public static pdec2() {var x = 1; --x; return x;}\n" \
		"	public static lt1() {return 1<2;}\n" \
		"	public static lt2() {return 1<1;}\n" \
		"	public static gt1() {return 1>2;}\n" \
		"	public static gt2() {return 2>1;}\n" \
		"	public static lteq1() {return 1<=2;}\n" \
		"	public static lteq2() {return 1<=1;}\n" \
		"	public static lteq3() {return 1<=0;}\n" \
		"	public static gteq1() {return 1>=2;}\n" \
		"	public static gteq2() {return 1>=1;}\n" \
		"	public static gteq3() {return 1>=0;}\n" \
		"	public static eq1() {return 1==1;}\n" \
		"	public static eq2() {return 1==0;}\n" \
		"	public static neq1() {return 1!=1;}\n" \
		"	public static neq2() {return 1!=0;}\n" \
		"	public static pluseq() {var x = 1; x+=2; return x;}\n" \
		"	public static minuseq() {var x = 1; x-=2; return x;}\n" \
		"	public static timeseq() {var x = 1; x*=2; return x;}\n" \
		"	public static diveq() {var x = 8; x/=2; return x;}\n" \
		"	public static modeq() {var x = 1; x%=2; return x;}\n" \
		"	public static oreq() {var x = 1; x|=2; return x;}\n" \
		"	public static andeq() {var x = 1; x&=2; return x;}\n" \
		"	public static xoreq() {var x = 1; x^=2; return x;}\n" \
		"	public static fcat() {var x = 99.0; return x + \"eos\";}\n" \
		"}\n";

	pp = parseChar( src, NULL );

	UNIT_ASSERT( "compTest13 parse", pp->errornum == 0 );
	prepareOk = qinterpPrepare( qip, pp );
	UNIT_ASSERT( "compTest13 prepare", prepareOk );

	err = qinterpRun( qip, "maths", "plus", 0, &retval );
	UNIT_ASSERT( "compTest13 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest13 return value", retval->data.i == 100 );
	varDestruct( retval );

	err = qinterpRun( qip, "maths", "minus", 0, &retval );
	UNIT_ASSERT( "compTest13 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest13 return value", retval->data.i == 98 );
	varDestruct( retval );

	err = qinterpRun( qip, "maths", "times", 0, &retval );
	UNIT_ASSERT( "compTest13 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest13 return value", retval->data.i == 100 );
	varDestruct( retval );

	err = qinterpRun( qip, "maths", "div", 0, &retval );
	UNIT_ASSERT( "compTest13 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest13 return value", retval->data.i == 2 );
	varDestruct( retval );

	err = qinterpRun( qip, "maths", "mod", 0, &retval );
	UNIT_ASSERT( "compTest13 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest13 return value", retval->data.i == 0 );
	varDestruct( retval );

	err = qinterpRun( qip, "maths", "or", 0, &retval );
	UNIT_ASSERT( "compTest13 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest13 return value", retval->data.i == 3 );
	varDestruct( retval );

	err = qinterpRun( qip, "maths", "and", 0, &retval );
	UNIT_ASSERT( "compTest13 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest13 return value", retval->data.i == 0 );
	varDestruct( retval );

	err = qinterpRun( qip, "maths", "xor", 0, &retval );
	UNIT_ASSERT( "compTest13 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest13 return value", retval->data.i == 3 );
	varDestruct( retval );

	err = qinterpRun( qip, "maths", "not", 0, &retval );
	UNIT_ASSERT( "compTest13 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest13 return value", retval->data.i == 0 );
	varDestruct( retval );

	err = qinterpRun( qip, "maths", "inc", 0, &retval );
	UNIT_ASSERT( "compTest13 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest13 return value", retval->data.i == 1 );
	varDestruct( retval );

	err = qinterpRun( qip, "maths", "dec", 0, &retval );
	UNIT_ASSERT( "compTest13 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest13 return value", retval->data.i == 1 );
	varDestruct( retval );

	err = qinterpRun( qip, "maths", "pinc", 0, &retval );
	UNIT_ASSERT( "compTest13 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest13 return value", retval->data.i == 2 );
	varDestruct( retval );

	err = qinterpRun( qip, "maths", "pdec", 0, &retval );
	UNIT_ASSERT( "compTest13 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest13 return value", retval->data.i == 0 );
	varDestruct( retval );

	err = qinterpRun( qip, "maths", "inc2", 0, &retval );
	UNIT_ASSERT( "compTest13 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest13 return value", retval->data.i == 2 );
	varDestruct( retval );

	err = qinterpRun( qip, "maths", "dec2", 0, &retval );
	UNIT_ASSERT( "compTest13 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest13 return value", retval->data.i == 0 );
	varDestruct( retval );

	err = qinterpRun( qip, "maths", "pinc2", 0, &retval );
	UNIT_ASSERT( "compTest13 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest13 return value", retval->data.i == 2 );
	varDestruct( retval );

	err = qinterpRun( qip, "maths", "pdec2", 0, &retval );
	UNIT_ASSERT( "compTest13 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest13 return value", retval->data.i == 0 );
	varDestruct( retval );

	err = qinterpRun( qip, "maths", "lt1", 0, &retval );
	UNIT_ASSERT( "compTest13 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest13 ret bool", TYPE_BOOL == retval->type );
	UNIT_ASSERT( "compTest13 return value", retval->data.i == 1 );
	varDestruct( retval );

	err = qinterpRun( qip, "maths", "lt2", 0, &retval );
	UNIT_ASSERT( "compTest13 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest13 ret bool", TYPE_BOOL == retval->type );
	UNIT_ASSERT( "compTest13 return value", retval->data.i == 0 );
	varDestruct( retval );

	err = qinterpRun( qip, "maths", "gt1", 0, &retval );
	UNIT_ASSERT( "compTest13 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest13 ret bool", TYPE_BOOL == retval->type );
	UNIT_ASSERT( "compTest13 return value", retval->data.i == 0 );
	varDestruct( retval );

	err = qinterpRun( qip, "maths", "gt2", 0, &retval );
	UNIT_ASSERT( "compTest13 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest13 ret bool", TYPE_BOOL == retval->type );
	UNIT_ASSERT( "compTest13 return value", retval->data.i == 1 );
	varDestruct( retval );

	err = qinterpRun( qip, "maths", "lteq1", 0, &retval );
	UNIT_ASSERT( "compTest13 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest13 ret bool", TYPE_BOOL == retval->type );
	UNIT_ASSERT( "compTest13 return value", retval->data.i == 1 );
	varDestruct( retval );

	err = qinterpRun( qip, "maths", "lteq2", 0, &retval );
	UNIT_ASSERT( "compTest13 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest13 ret bool", TYPE_BOOL == retval->type );
	UNIT_ASSERT( "compTest13 return value", retval->data.i == 1 );
	varDestruct( retval );

	err = qinterpRun( qip, "maths", "lteq3", 0, &retval );
	UNIT_ASSERT( "compTest13 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest13 ret bool", TYPE_BOOL == retval->type );
	UNIT_ASSERT( "compTest13 return value", retval->data.i == 0 );
	varDestruct( retval );

	err = qinterpRun( qip, "maths", "gteq1", 0, &retval );
	UNIT_ASSERT( "compTest13 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest13 ret bool", TYPE_BOOL == retval->type );
	UNIT_ASSERT( "compTest13 return value", retval->data.i == 0 );
	varDestruct( retval );

	err = qinterpRun( qip, "maths", "gteq2", 0, &retval );
	UNIT_ASSERT( "compTest13 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest13 ret bool", TYPE_BOOL == retval->type );
	UNIT_ASSERT( "compTest13 return value", retval->data.i == 1 );
	varDestruct( retval );

	err = qinterpRun( qip, "maths", "gteq3", 0, &retval );
	UNIT_ASSERT( "compTest13 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest13 ret bool", TYPE_BOOL == retval->type );
	UNIT_ASSERT( "compTest13 return value", retval->data.i == 1 );
	varDestruct( retval );

	err = qinterpRun( qip, "maths", "eq1", 0, &retval );
	UNIT_ASSERT( "compTest13 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest13 ret bool", TYPE_BOOL == retval->type );
	UNIT_ASSERT( "compTest13 return value", retval->data.i == 1 );
	varDestruct( retval );

	err = qinterpRun( qip, "maths", "eq2", 0, &retval );
	UNIT_ASSERT( "compTest13 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest13 ret bool", TYPE_BOOL == retval->type );
	UNIT_ASSERT( "compTest13 return value", retval->data.i == 0 );
	varDestruct( retval );

	err = qinterpRun( qip, "maths", "neq1", 0, &retval );
	UNIT_ASSERT( "compTest13 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest13 ret bool", TYPE_BOOL == retval->type );
	UNIT_ASSERT( "compTest13 return value", retval->data.i == 0 );
	varDestruct( retval );

	err = qinterpRun( qip, "maths", "neq2", 0, &retval );
	UNIT_ASSERT( "compTest13 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest13 ret bool", TYPE_BOOL == retval->type );
	UNIT_ASSERT( "compTest13 return value", retval->data.i == 1 );
	varDestruct( retval );

	err = qinterpRun( qip, "maths", "pluseq", 0, &retval );
	UNIT_ASSERT( "compTest13 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest13 return value", retval->data.i == 3 );
	varDestruct( retval );

	err = qinterpRun( qip, "maths", "minuseq", 0, &retval );
	UNIT_ASSERT( "compTest13 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest13 return value", retval->data.i == -1 );
	varDestruct( retval );

	err = qinterpRun( qip, "maths", "timeseq", 0, &retval );
	UNIT_ASSERT( "compTest13 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest13 return value", retval->data.i == 2 );
	varDestruct( retval );

	err = qinterpRun( qip, "maths", "diveq", 0, &retval );
	UNIT_ASSERT( "compTest13 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest13 return value", retval->data.i == 4 );
	varDestruct( retval );

	err = qinterpRun( qip, "maths", "modeq", 0, &retval );
	UNIT_ASSERT( "compTest13 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest13 return value", retval->data.i == 1 );
	varDestruct( retval );

	err = qinterpRun( qip, "maths", "oreq", 0, &retval );
	UNIT_ASSERT( "compTest13 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest13 return value", retval->data.i == 3 );
	varDestruct( retval );

	err = qinterpRun( qip, "maths", "andeq", 0, &retval );
	UNIT_ASSERT( "compTest13 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest13 return value", retval->data.i == 0 );
	varDestruct( retval );

	err = qinterpRun( qip, "maths", "xoreq", 0, &retval );
	UNIT_ASSERT( "compTest13 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest13 return value", retval->data.i == 3 );
	varDestruct( retval );

	err = qinterpRun( qip, "maths", "fcat", 0, &retval );
	UNIT_ASSERT( "compTest13 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest13 return value", TYPE_STRING == retval->type && stringEndsWith( retval->data.sp, "eos") );
	varDestruct( retval );

	qinterpDestruct( qip );
	logWriteOkFail(NULL);
}

static void compTest14_1(  )
{
	qinterp *qip;
	program *pp;
	enum InterpErrors err;
	variant *retval = NULL;
	bool prepareOk;
	char *src;

	logWriteCheck( NULL, "compile/run test 14_1 ... " );

	qip = qinterpConstruct();
	src = 
		"class maths { \n" \
		"	public maths() {}\n" \
		"	public runit2() {var x = 0; x++; return x;}\n" \
		"	public static main() {var m = new maths();}\n" \
		"	public static main2() {var m = new maths(); return m.runit2();}\n" \
		"}\n";

	pp = parseChar( src, NULL );

	UNIT_ASSERT( "compTest14_1 parse", pp->errornum == 0 );
	prepareOk = qinterpPrepare( qip, pp );
	UNIT_ASSERT( "compTest14_1 prepare", prepareOk );

	err = qinterpRun( qip, "maths", "main", 0, &retval );
	UNIT_ASSERT( "compTest14_1 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest14_1", NULL == retval );

	err = qinterpRun( qip, "maths", "main2", 0, &retval );
	UNIT_ASSERT( "compTest14_1 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest14_1", retval->data.i == 1 );
	varDestruct( retval );

	qinterpDestruct( qip );
	logWriteOkFail(NULL);
}

static void compTest14(  )
{
	qinterp *qip;
	program *pp;
	enum InterpErrors err;
	variant *retval = NULL;
	bool prepareOk;
	char *src;

	logWriteCheck( NULL, "compile/run test 14 ... " );

	qip = qinterpConstruct();
	src = 
		"class maths { \n" \
		"	public maths() {}\n" \
		"	public plus() {return 99 + 1;}\n" \
		"	public minus() {return 99-1;}\n" \
		"	public times() {return 50*2;}\n" \
		"	public div() {return 4/2;}\n" \
		"	public mod() {return 9%3;}\n" \
		"	public or() {return 2|1;}\n" \
		"	public and() {return 2&1;}\n" \
		"	public xor() {return 2^1;}\n" \
		"	public not() {return !1;}\n" \
		"	public inc() {var x = 1; return x++;}\n" \
		"	public dec() {var x = 1; return x--;}\n" \
		"	public pinc() {var x = 1; return ++x;}\n" \
		"	public pdec() {var x = 1; return --x;}\n" \
		"	public inc2() {var x = 1; x++; return x;}\n" \
		"	public dec2() {var x = 1; x--; return x;}\n" \
		"	public pinc2() {var x = 1; ++x; return x;}\n" \
		"	public pdec2() {var x = 1; --x; return x;}\n" \
		"	public lt1() {return 1<2;}\n" \
		"	public lt2() {return 1<1;}\n" \
		"	public gt1() {return 1>2;}\n" \
		"	public gt2() {return 2>1;}\n" \
		"	public lteq1() {return 1<=2;}\n" \
		"	public lteq2() {return 1<=1;}\n" \
		"	public lteq3() {return 1<=0;}\n" \
		"	public gteq1() {return 1>=2;}\n" \
		"	public gteq2() {return 1>=1;}\n" \
		"	public gteq3() {return 1>=0;}\n" \
		"	public eq1() {return 1==1;}\n" \
		"	public eq2() {return 1==0;}\n" \
		"	public neq1() {return 1!=1;}\n" \
		"	public neq2() {return 1!=0;}\n" \
		"	public pluseq() {var x = 1; x+=2; return x;}\n" \
		"	public minuseq() {var x = 1; x-=2; return x;}\n" \
		"	public timeseq() {var x = 1; x*=2; return x;}\n" \
		"	public diveq() {var x = 8; x/=2; return x;}\n" \
		"	public modeq() {var x = 1; x%=2; return x;}\n" \
		"	public oreq() {var x = 1; x|=2; return x;}\n" \
		"	public andeq() {var x = 1; x&=2; return x;}\n" \
		"	public xoreq() {var x = 1; x^=2; return x;}\n" \
		"\n" \
		"	public runit() {" \
		"		if ( plus() != 100 ) { return false; }\n" \
		"		if ( minus() != 98 ) { return false; }\n" \
		"		if ( times() != 100 ) { return false; }\n" \
		"		if ( div() != 2 ) { return false; }\n" \
		"		if ( mod() != 0 ) { return false; }\n" \
		"		if ( or() != 3 ) { return false; }\n" \
		"		if ( and() != 0 ) { return false; }\n" \
		"		if ( xor() != 3 ) { return false; }\n" \
		"		if ( not() != 0 ) { return false; }\n" \
		"		if ( minus() != 98 ) { return false; }\n" \
		"		if ( inc() != 1 ) { return false; }\n" \
		"		if ( dec() != 1 ) { return false; }\n" \
		"		if ( pinc() != 2 ) { return false; }\n" \
		"		if ( pdec() != 0 ) { return false; }\n" \
		"		if ( inc2() != 2 ) { return false; }\n" \
		"		if ( dec2() != 0 ) { return false; }\n" \
		"		if ( pinc2() != 2 ) { return false; }\n" \
		"		if ( pdec2() != 0 ) { return false; }\n" \
		"		if ( lt1() != 1 ) { return false; }\n" \
		"		if ( lt2() != 0 ) { return false; }\n" \
		"		if ( gt1() != 0 ) { return false; }\n" \
		"		if ( gt2() != 1 ) { return false; }\n" \
		"		return true;\n" \
		"	}\n" \
		"	public static main() {var m = new maths(); return m.runit();}\n" \
		"}\n";

	pp = parseChar( src, NULL );

	UNIT_ASSERT( "compTest14 parse", pp->errornum == 0 );
	prepareOk = qinterpPrepare( qip, pp );
	UNIT_ASSERT( "compTest14 prepare", prepareOk );

	err = qinterpRun( qip, "maths", "main", 0, &retval );
	UNIT_ASSERT( "compTest14 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest14 return value", retval->data.i == 1 );
	varDestruct( retval );

	qinterpDestruct( qip );
	logWriteOkFail(NULL);
}

static void compTest15(  )
{
	qinterp *qip;
	program *pp;
	enum InterpErrors err;
	variant *retval = NULL;
	bool prepareOk;
	char *src;

	logWriteCheck( NULL, "compile/run test 15 ... " );

	qip = qinterpConstruct();
	src = 
		"class field { public var data; public field(i) {data = i;} }\n" \
		"" \
		"class fields { \n" \
		"	public var f1 = new field(1);\n" \
		"	public fields() {}\n" \
		"\n" \
		"	public static main() {var f = new fields(); return f.f1.data;}\n" \
		"}\n";

	pp = parseChar( src, NULL );

	UNIT_ASSERT( "compTest15 parse", pp->errornum == 0 );
	prepareOk = qinterpPrepare( qip, pp );
	UNIT_ASSERT( "compTest15 prepare", prepareOk );

	err = qinterpRun( qip, "fields", "main", 0, &retval );
	UNIT_ASSERT( "compTest15 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest15 return value", retval->data.i == 1 );
	varDestruct( retval );

	qinterpDestruct( qip );
	logWriteOkFail(NULL);
}

static void compTest16(  )
{
	qinterp *qip;
	program *pp;
	enum InterpErrors err;
	variant *retval = NULL;
	bool prepareOk;
	char *src;

	logWriteCheck( NULL, "compile/run test 16 ... " );

	qip = qinterpConstruct();
	src = 
		"class test { \n" \
		"	public static main() {var f = 10; while( f > 0 ) {f--;}  return f; }\n" \
		"}\n";

	pp = parseChar( src, NULL );

	UNIT_ASSERT( "compTest16 parse", pp->errornum == 0 );
	prepareOk = qinterpPrepare( qip, pp );
	UNIT_ASSERT( "compTest16 prepare", prepareOk );

	err = qinterpRun( qip, "test", "main", 0, &retval );
	UNIT_ASSERT( "compTest16 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest16 return value", retval->data.i == 0 );
	varDestruct( retval );

	qinterpDestruct( qip );
	logWriteOkFail(NULL);
}

static void compTest17(  )
{
	qinterp *qip;
	program *pp;
	enum InterpErrors err;
	variant *retval = NULL;
	bool prepareOk;
	char *src;

	logWriteCheck( NULL, "compile/run test 17 ... " );

	qip = qinterpConstruct();
	src = 
		"class test { \n" \
		"	public static main() { var q; for(var x = 0; x < 10; x++ ) {q=x;}  return q; }\n" \
		"}\n";

	pp = parseChar( src, NULL );

	UNIT_ASSERT( "compTest17 parse", pp->errornum == 0 );
	prepareOk = qinterpPrepare( qip, pp );
	UNIT_ASSERT( "compTest17 prepare", prepareOk );

	err = qinterpRun( qip, "test", "main", 0, &retval );
	UNIT_ASSERT( "compTest17 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest17 return value", retval->data.i == 9 );
	varDestruct( retval );

	qinterpDestruct( qip );
	logWriteOkFail(NULL);
}

static void compTest18(  )
{
	qinterp *qip;
	program *pp;
	enum InterpErrors err;
	variant *retval = NULL;
	bool prepareOk;
	char *src;

	logWriteCheck( NULL, "compile/run test 18 ... " );

	qip = qinterpConstruct();
	src = 
		"class test { \n" \
		"	public static main() { var q; for(var x = 0; x < 10; x++ ) {if ( x == 5 ) {break;} q=x;}  return q; }\n" \
		"}\n";

	pp = parseChar( src, NULL );

	UNIT_ASSERT( "compTest18 parse", pp->errornum == 0 );
	prepareOk = qinterpPrepare( qip, pp );
	UNIT_ASSERT( "compTest18 prepare", prepareOk );

	err = qinterpRun( qip, "test", "main", 0, &retval );
	UNIT_ASSERT( "compTest18 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest18 return value", retval->data.i == 4 );
	varDestruct( retval );

	qinterpDestruct( qip );
	logWriteOkFail(NULL);
}

static void compTest19(  )
{
	qinterp *qip;
	program *pp;
	enum InterpErrors err;
	variant *retval = NULL;
	bool prepareOk;
	char *src;

	logWriteCheck( NULL, "compile/run test 19 ... " );

	qip = qinterpConstruct();
	src = 
		"class test { \n" \
		"	public static main() { var q; for(var x = 0; x < 10; x++ ) {if ( x >= 5 ) {continue;} q=x;}  return q; }\n" \
		"}\n";

	pp = parseChar( src, NULL );

	UNIT_ASSERT( "compTest19 parse", pp->errornum == 0 );
	prepareOk = qinterpPrepare( qip, pp );
	UNIT_ASSERT( "compTest19 prepare", prepareOk );

	err = qinterpRun( qip, "test", "main", 0, &retval );
	UNIT_ASSERT( "compTest19 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest19 return value", retval->data.i == 4 );
	varDestruct( retval );

	qinterpDestruct( qip );
	logWriteOkFail(NULL);
}

static void compTest20(  )
{
	qinterp *qip;
	program *pp;
	enum InterpErrors err;
	variant *retval = NULL;
	bool prepareOk;
	char *src;

	logWriteCheck( NULL, "compile/run test 20 ... " );

	qip = qinterpConstruct();
	src = 
		"class test { \n" \
		"	public static main2() { var q = 1; switch(q) {case 2: return 1; case 1: return 2; case 3: break; default: return 3;}  return -1; }\n" \
		"	public static main1() { var q = 2; switch(q) {case 2: return 1; case 1: return 2; case 3: break; default: return 3;}  return -1; }\n" \
		"	public static mainm1() { var q = 2; switch(3) {case 2: return 1; case 1: return 2; case 3: break; default: return 3;}  return -1; }\n" \
		"	public static main3() { var q = 20; switch(q) {case 2: return 1; case 1: return 2; case 3: break; default: return 3;}  return -1; }\n" \
		"}\n";

	pp = parseChar( src, NULL );

	UNIT_ASSERT( "compTest20 parse", pp->errornum == 0 );
	prepareOk = qinterpPrepare( qip, pp );
	UNIT_ASSERT( "compTest20 prepare", prepareOk );

	err = qinterpRun( qip, "test", "main2", 0, &retval );
	UNIT_ASSERT( "compTest20 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest20 return value", retval->data.i == 2 );
	varDestruct( retval );

	err = qinterpRun( qip, "test", "main1", 0, &retval );
	UNIT_ASSERT( "compTest20 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest20 return value", retval->data.i == 1 );
	varDestruct( retval );

	err = qinterpRun( qip, "test", "mainm1", 0, &retval );
	UNIT_ASSERT( "compTest20 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest20 return value", retval->data.i == -1 );
	varDestruct( retval );

	err = qinterpRun( qip, "test", "main3", 0, &retval );
	UNIT_ASSERT( "compTest20 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest20 return value", retval->data.i == 3 );
	varDestruct( retval );

	qinterpDestruct( qip );
	logWriteOkFail(NULL);
}

static void compTest20_1(  )
{
	qinterp *qip;
	program *pp;
	enum InterpErrors err;
	variant *retval = NULL;
	bool prepareOk;
	char *src;

	logWriteCheck( NULL, "compile/run test 20_1 ... " );

	qip = qinterpConstruct();
	src = 
		"class test { \n" \
		"	private static val() { return 88; }\n" \
		"	public static main() { return test::val(); }\n" \
		"}\n";

	pp = parseChar( src, NULL );

	UNIT_ASSERT( "compTest20_1 parse", pp->errornum == 0 );
	prepareOk = qinterpPrepare( qip, pp );
	UNIT_ASSERT( "compTest20 prepare", prepareOk );

	err = qinterpRun( qip, "test", "main", 0, &retval );
	UNIT_ASSERT( "compTest20_1 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest20_1 return value", retval->data.i == 88 );
	varDestruct( retval );

	qinterpDestruct( qip );
	logWriteOkFail(NULL);
}

static void compTest21(  )
{
	qinterp *qip;
	program *pp;
	enum InterpErrors err;
	variant *retval = NULL;
	bool prepareOk;
	char *src;

	logWriteCheck( NULL, "compile/run test 21 ... " );

	qip = qinterpConstruct();
	src = 
		"class test { \n" \
		"	private static thrower() { throw 88; }\n" \
		"	public static main1() { try {} catch(ex){} }\n" \
		"	public static main2() { try {throw 1;} catch(ex){return ex;} return 0; }\n" \
		"	public static main3() { try {return 1;} catch(ex){return ex;} return 0; }\n" \
		"	public static mainF() { try {throw 1;} catch(ex){return ex;} finally {return 9;} return 0; }\n" \
		"	public static mainUnwind() { try { test::thrower(); } catch (ex) { return ex; } return 0; }\n" \
		"}\n";

	pp = parseChar( src, NULL );

	UNIT_ASSERT( "compTest21 parse", pp->errornum == 0 );
	prepareOk = qinterpPrepare( qip, pp );
	UNIT_ASSERT( "compTest21 prepare", prepareOk );

	err = qinterpRun( qip, "test", "main1", 0, &retval );
	UNIT_ASSERT( "compTest21 interp error", err == IERR_NONE );

	err = qinterpRun( qip, "test", "main2", 0, &retval );
	UNIT_ASSERT( "compTest21 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest21 return value", retval->data.i == 1 );
	varDestruct( retval );

	err = qinterpRun( qip, "test", "main3", 0, &retval );
	UNIT_ASSERT( "compTest21 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest21 return value", retval->data.i == 1 );
	varDestruct( retval );

	err = qinterpRun( qip, "test", "mainF", 0, &retval );
	UNIT_ASSERT( "compTest21 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest21 return value", retval->data.i == 9 );
	varDestruct( retval );
	
	err = qinterpRun( qip, "test", "mainUnwind", 0, &retval );
	UNIT_ASSERT( "compTest21 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest21 return value", retval->data.i == 88 );
	varDestruct( retval );
	
	qinterpDestruct( qip );
	logWriteOkFail(NULL);
}

static void compTest21_1(  )
{
	qinterp *qip;
	program *pp;
	enum InterpErrors err;
	variant *retval = NULL;
	bool prepareOk;
	char *src;

	logWriteCheck( NULL, "compile/run test 21_1 ... " );

	qip = qinterpConstruct();
	src = 
		"class test { \n" \
		"	public static main() { new test(); }\n" \
		"}\n";

	pp = parseChar( src, NULL );

	DEBUG_CLEAR_MEM_CHECK_POINTS();
	programCheckMem( pp );
	qinterpCheckMem( qip );
	UNIT_ASSERT_MEM_NOTED( "testcompile 21_1 1" );

	UNIT_ASSERT( "compTest21_1 parse", pp->errornum == 0 );
	prepareOk = qinterpPrepare( qip, pp );
	UNIT_ASSERT( "compTest21_1 prepare", prepareOk );

	DEBUG_CLEAR_MEM_CHECK_POINTS();
	qinterpCheckMem( qip );
	UNIT_ASSERT_MEM_NOTED( "testcompile 21_1 2" );

	err = qinterpRun( qip, "test", "main", 0, &retval );
	UNIT_ASSERT( "compTest21_1 interp error", err == IERR_NONE );
	/*UNIT_ASSERT( "compTest21_1 return value", retval->data.i == 88 );*/
	/*varDestruct( retval );*/
	
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	qinterpCheckMem( qip );
	UNIT_ASSERT_MEM_NOTED( "testcompile 21_1 3" );

	qinterpDestruct( qip );

	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "testcompile 21_1 4" );
	logWriteOkFail(NULL);
}

static void compTest22(  )
{
	qinterp *qip;
	program *pp;
	enum InterpErrors err;
	variant *retval = NULL;
	bool prepareOk;
	char *src;

	logWriteCheck( NULL, "compile/run test 22 ... " );

	qip = qinterpConstruct();
	src = 
		"class test { \n" \
		"	private thrower() { throw 88; }\n" \
		"	public test() { thrower(); } \n" \
		"	public static main() { try { new test(); } catch(ex){return ex} return 0; }\n" \
		"}\n";

	pp = parseChar( src, NULL );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	qinterpCheckMem( qip );
	programCheckMem( pp );
	UNIT_ASSERT_MEM_NOTED( "testcompile 22 1" );

	UNIT_ASSERT( "compTest22 parse", pp->errornum == 0 );
	prepareOk = qinterpPrepare( qip, pp );
	UNIT_ASSERT( "compTest22 prepare", prepareOk );

	err = qinterpRun( qip, "test", "main", 0, &retval );
	UNIT_ASSERT( "compTest22 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest22 return value", retval->data.i == 88 );
	varDestruct( retval );
	
	qinterpDestruct( qip );
	logWriteOkFail(NULL);
}

static void compTest23(  )
{
	qinterp *qip;
	program *pp;
	enum InterpErrors err;
	variant *retval = NULL;
	bool prepareOk;
	char *src;

	logWriteCheck( NULL, "compile/run test 23 ... " );

	qip = qinterpConstruct();
	src = 
		"class base {\n" \
		"	public base() {}\n" \
		"	public method() {return 1;}\n" \
		"}\n" \
		"class test : base { \n" \
		"	public test() { } \n" \
		"	public method() { return 2; }\n" \
		"	public call() { return ::method(); }\n" \
		"	public static main() { var t = new test(); return t.call(); }\n" \
		"}\n";

	pp = parseChar( src, NULL );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	qinterpCheckMem( qip );
	programCheckMem( pp );
	UNIT_ASSERT_MEM_NOTED( "testcompile 23 1" );

	UNIT_ASSERT( "compTest23 parse", pp->errornum == 0 );
	prepareOk = qinterpPrepare( qip, pp );
	UNIT_ASSERT( "compTest23 prepare", prepareOk );

	err = qinterpRun( qip, "test", "main", 0, &retval );
	UNIT_ASSERT( "compTest23 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest23 return value", retval->data.i == 1 );
	varDestruct( retval );
	
	qinterpDestruct( qip );
	logWriteOkFail(NULL);
}

static void compTest24(  )
{
	qinterp *qip;
	program *pp;
	enum InterpErrors err;
	variant *retval = NULL;
	bool prepareOk;
	char *src;

	logWriteCheck( NULL, "compile/run test 24 ... " );

	qip = qinterpConstruct();
	src = 
		"class base {\n" \
		"	public base() {}\n" \
		"	public method() {return 99;}\n" \
		"}\n" \
		"class test : base { \n" \
		"	public test() { } \n" \
		"	public method() { return 2; }\n" \
		"	public call() { return ::method(); }\n" \
		"	public call2() { return method(); }\n" \
		"}\n" \
		"class main {\n" \
		"	public static main1() { var t = new test(); return t.method(); }\n" \
		"	public static main2() { var t = new test(); return t.call(); }\n" \
		"	public static main3() { var t = new test(); return t.call2(); }\n" \
		"}\n";

	pp = parseChar( src, NULL );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	qinterpCheckMem( qip );
	programCheckMem( pp );
	UNIT_ASSERT_MEM_NOTED( "testcompile 24" );

	UNIT_ASSERT( "compTest24 parse", pp->errornum == 0 );
	prepareOk = qinterpPrepare( qip, pp );
	UNIT_ASSERT( "compTest23 prepare", prepareOk );

	err = qinterpRun( qip, "main", "main1", 0, &retval );
	UNIT_ASSERT( "compTest24 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest24 return value", retval->data.i == 2 );
	varDestruct( retval );
	
	err = qinterpRun( qip, "main", "main2", 0, &retval );
	UNIT_ASSERT( "compTest24 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest24 return value", retval->data.i == 99 );
	varDestruct( retval );

	err = qinterpRun( qip, "main", "main3", 0, &retval );
	UNIT_ASSERT( "compTest24 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest24 return value", retval->data.i == 2 );
	varDestruct( retval );

	qinterpDestruct( qip );
	logWriteOkFail(NULL);
}

static void compTest25(  )
{
	qinterp *qip;
	program *pp;
	enum InterpErrors err;
	variant *retval = NULL;
	bool prepareOk;
	char *src;

	logWriteCheck( NULL, "compile/run test 25 ... " );

	qip = qinterpConstruct();
	src = 
		"class base {\n" \
		"	protected var val;\n" \
		"	public base(x) {val = x;}\n" \
		"	public method() {return val;}\n" \
		"}\n" \
		"class test : base { \n" \
		"	public test(x) { } \n" \
		"	public method() { return ::method(); }\n" \
		"	public method2(arg) { return arg; }\n" \
		"	public static main() { var t = new test(1); return t.method(); }\n" \
		"	public static main2() { var t = new test(1); return t.method2(\"HI\"); }\n" \
		"	public static main3() { var t = new test(\"HI\"); return t.method(); }\n" \
		"}\n";

	pp = parseChar( src, NULL );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	qinterpCheckMem( qip );
	programCheckMem( pp );
	UNIT_ASSERT_MEM_NOTED( "testcompile 25 1" );

	UNIT_ASSERT( "compTest25 parse", pp->errornum == 0 );
	prepareOk = qinterpPrepare( qip, pp );
	UNIT_ASSERT( "compTest25 prepare", prepareOk );

	DEBUG_CLEAR_MEM_CHECK_POINTS();
	qinterpCheckMem( qip );
	programCheckMem( pp );
	UNIT_ASSERT_MEM_NOTED( "testcompile 23 1" );

	err = qinterpRun( qip, "test", "main", 0, &retval );
	UNIT_ASSERT( "compTest25 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest25 return value", retval->data.i == 1 );
	varDestruct( retval );
	
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	qinterpCheckMem( qip );
	programCheckMem( pp );
	UNIT_ASSERT_MEM_NOTED( "testcompile 23 1" );

	err = qinterpRun( qip, "test", "main2", 0, &retval );
	UNIT_ASSERT( "compTest25 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest25 return value", TYPE_STRING == retval->type && strcmp(retval->data.sp->cstr, "HI") == 0 );
	varDestruct( retval );
	
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	qinterpCheckMem( qip );
	programCheckMem( pp );
	UNIT_ASSERT_MEM_NOTED( "testcompile 23 1" );

	err = qinterpRun( qip, "test", "main3", 0, &retval );
	UNIT_ASSERT( "compTest25 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest25 return value", TYPE_STRING == retval->type && strcmp(retval->data.sp->cstr, "HI") == 0 );
	varDestruct( retval );

	qinterpDestruct( qip );
	logWriteOkFail(NULL);
}

static void compTest26(  )
{
	qinterp *qip;
	program *pp;
	enum InterpErrors err;
	variant *retval = NULL;
	bool prepareOk;
	char *src;

	logWriteCheck( NULL, "compile/run test 26 ... " );

	qip = qinterpConstruct();
	src = 
		"class test { \n" \
		"	public static main() { \n" \
		"		// this is a comment\n" \
		"		/* /*return 1;*/ return 3; */\n" \
		"		return 2; \n" \
		"}\n" \
		"}\n";

	pp = parseChar( src, NULL );

	UNIT_ASSERT( "compTest26 parse", pp->errornum == 0 );
	prepareOk = qinterpPrepare( qip, pp );
	UNIT_ASSERT( "compTest26 prepare", prepareOk );

	err = qinterpRun( qip, "test", "main", 0, &retval );
	UNIT_ASSERT( "compTest26 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest26 return value", retval->data.i == 2 );
	varDestruct( retval );

	qinterpDestruct( qip );
	logWriteOkFail(NULL);
}

static void compTest27(  )
{
	qinterp *qip;
	program *pp;
	enum InterpErrors err;
	variant *retval = NULL;
	bool prepareOk;
	char *src;

	logWriteCheck( NULL, "compile/run test 27 ... " );

	qip = qinterpConstruct();
	src = 
		"class test { \n" \
		"	private static var arr = {1,2,3};\n" \
		"	public static main() { \n" \
		"		return arr[2]; \n" \
		"	}\n" \
		"}\n";

	pp = parseChar( src, qinterpGetPrograms(qip) );

	/*programDump( pp, "c:\\temp\\compTest27.txt" );*/

	UNIT_ASSERT( "compTest27 parse", pp->errornum == 0 );
	prepareOk = qinterpPrepare( qip, pp );
	UNIT_ASSERT( "compTest27 prepare", prepareOk );

	err = qinterpRun( qip, "test", "main", 0, &retval );
	UNIT_ASSERT( "compTest27 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest27 return value", retval->data.i == 3 );
	varDestruct( retval );

	qinterpDestruct( qip );
	logWriteOkFail(NULL);
}

static void compTest28(  )
{
	qinterp *qip;
	program *pp;
	enum InterpErrors err;
	variant *retval = NULL;
	bool prepareOk;
	char *src;

	logWriteCheck( NULL, "compile/run test 28 ... " );

	qip = qinterpConstruct();
	src = 
		"class test { \n" \
		"	public var x = new Vector();\n" \
		"	public static var arr = {1,2,3};\n" \
		"	public var y = new Array(1);\n" \
		"	public static main() { \n" \
		"		var obj = new test();\n" \
		"		obj.x.add( arr[1] );\n" \
		"		obj.y[0] = arr[2];\n" \
		"		return obj.x[0] * obj.y[0]; \n" \
		"	}\n" \
		"}\n";

	pp = parseChar( src, qinterpGetPrograms(qip) );

	UNIT_ASSERT( "compTest28 parse", pp->errornum == 0 );
	prepareOk = qinterpPrepare( qip, pp );
	UNIT_ASSERT( "compTest28 prepare", prepareOk );

	err = qinterpRun( qip, "test", "main", 0, &retval );
	UNIT_ASSERT( "compTest28 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest28 return value", retval->data.i == 6 );
	varDestruct( retval );

	qinterpDestruct( qip );
	logWriteOkFail(NULL);
}

static void compTest29(  )
{
	qinterp *qip;
	program *pp;
	enum InterpErrors err;
	variant *retval = NULL;
	bool prepareOk;
	char *src;

	logWriteCheck( NULL, "compile/run test 29 ... " );

	qip = qinterpConstruct();
	src = 
		"class test{\nprivate var i = 1;\npublic method() {var ar = new Array(i); ar[0] = 6; return ar[0];}\npublic static main()\n{var obj = new test(); return obj.method();} }\n";

	pp = parseChar( src, qinterpGetPrograms(qip) );

	UNIT_ASSERT( "compTest29 parse", pp->errornum == 0 );
	prepareOk = qinterpPrepare( qip, pp );
	UNIT_ASSERT( "compTest29 prepare", prepareOk );

	err = qinterpRun( qip, "test", "main", 0, &retval );
	UNIT_ASSERT( "compTest29 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest29 return value", retval->data.i == 6 );
	varDestruct( retval );

	qinterpDestruct( qip );
	logWriteOkFail( NULL );
}

static void compTest30( p )
{
	qinterp *qip;
	program *pp;
	enum InterpErrors err;
	variant *retval = NULL;
	bool prepareOk;
	char *src;

	logWriteCheck( NULL, "compile/run test 30 ... " );

	qip = qinterpConstruct();
	src = 
		"class test{\n" \
		"	public static main()\n{var i = 0; var x = 1;var y=1;\n if(x && y){i++;} if( x || 0 ){i++;} if ( x || y ) {i++;} if ( ! (y && 0) ) {i++;} return i; }\n" \
		"	public static main2()\n{var ins = new Array(4,2); return ins.length(1); }\n" \
		"	public static main3()\n{var x = 1; var str = \"hi\"; return x + str + \"\\n\"; }\n" \
		"	public static createNetwork(in, h1, h2, out) { if (in == 0 || h1 == 0 || h2 == 0 || out == 0) { return 1; } else { return -1; } }\n" \
		"	public static main4()\n{ var a = 2; var b = 4; var c = 4; var d = 2; return test::createNetwork(a,b,c,d); }\n" \
		"	public callme() {var sum = 0; for ( var x = 0; x < 9; x++ ) {var q = 1; sum+=1.1*q;} return sum;}\n" \
		"	public static main5()\n{ var b = 0; var a = new test(); b += a.callme(); return b; }\n" \
		"	private static var y = \"\";\n" \
		"	public callme2(a, b) {return a + b;}\n" \
		"	public static main6()\n{ var a = new test(); return a.callme2(test::y, test::y.length()); }\n" \
		"}\n";

	pp = parseChar( src, qinterpGetPrograms(qip) );

	UNIT_ASSERT( "compTest30 parse", pp->errornum == 0 );
	prepareOk = qinterpPrepare( qip, pp );
	UNIT_ASSERT( "compTest30 prepare", prepareOk );

	err = qinterpRun( qip, "test", "main", 0, &retval );
	UNIT_ASSERT( "compTest30 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest30 return value", retval->data.i == 4 );
	varDestruct( retval );

	err = qinterpRun( qip, "test", "main2", 0, &retval );
	UNIT_ASSERT( "compTest30 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest30 return value", retval->data.i == 2 );
	varDestruct( retval );

	err = qinterpRun( qip, "test", "main3", 0, &retval );
	UNIT_ASSERT( "compTest30 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest30 return value", TYPE_STRING == retval->type && strcmp(retval->data.sp->cstr, "1hi\n") == 0 );
	varDestruct( retval );

	err = qinterpRun( qip, "test", "main4", 0, &retval );
	UNIT_ASSERT( "compTest30 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest30 return value", retval->data.i == -1 );
	varDestruct( retval );

	err = qinterpRun( qip, "test", "main5", 0, &retval );
	UNIT_ASSERT( "compTest30 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest30 return value", ((int)(retval->data.f*10)) == 99 );
	varDestruct( retval );

	err = qinterpRun( qip, "test", "main6", 0, &retval );
	UNIT_ASSERT( "compTest30 interp error", err == IERR_NONE );
	UNIT_ASSERT( "compTest30 return value", TYPE_STRING == retval->type && retval->data.sp->cstr[0] == '0' );
	varDestruct( retval );

	qinterpDestruct( qip );
	logWriteOkFail(NULL);
}

void testcompile(  )
{
	compTest00(  );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "testcompile 0" );

	compTest0(  );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "testcompile 0" );

	compTest1(  );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "testcompile 1" );

	compTest2(  );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "testcompile 2" );

	compTest3(  );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "testcompile 3" );

	compTest4(  );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "testcompile 4" );

	compTest5(  );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "testcompile 5" );

	compTest6(  );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "testcompile 6" );

	compTest7(  );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "testcompile 7" );

	compTest7_1a(  );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "testcompile 7a" );

	compTest7_1b(  );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "testcompile 7b" );

	compTest7_1c(  );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "testcompile 7c" );

	compTest8(  );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "testcompile 8" );

	compTest9(  );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "testcompile 9" );

	compTest10(  );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "testcompile 10" );

	compTest11(  );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "testcompile 12" );

	compTest12(  );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "testcompile 12" );

	compTest13(  );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "testcompile 13" );

	compTest14_1(  );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "testcompile 14_1" );

	compTest14(  );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "testcompile 14" );

	compTest15(  );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "testcompile 15" );

	compTest16(  );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "testcompile 16" );

	compTest17(  );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "testcompile 17" );

	compTest18(  );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "testcompile 18" );

	compTest19(  );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "testcompile 19" );

	compTest20(  );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "testcompile 20" );

	compTest20_1(  );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "testcompile 20_1" );

	compTest21(  );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "testcompile 21" );

	compTest21_1(  );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "testcompile 21_1" );

	compTest22(  );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "testcompile 22" );

	compTest23(  );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "testcompile 23" );

	compTest24(  );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "testcompile 24" );

	compTest25(  );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "testcompile 25" );

	compTest26(  );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "testcompile 26" );

	compTest27(  );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "testcompile 27" );

	compTest28(  );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "testcompile 28" );

	compTest29(  );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "testcompile 29" );

	compTest30(  );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "testcompile 30" );
}

#endif /*DEBUG*/

