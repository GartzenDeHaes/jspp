#include <string.h>
#include <spl/debug.h>
#include <jspp/parse.h>
#include <jspp/interp.h>

static void smallProgTest00(  )
{
	program *ilp;
	
	logWriteCheck(NULL, "parse 00 test ... " );

	ilp = parseChar( "class test {public method(){}}", NULL );

	UNIT_ASSERT( "simple parse 1", ilp->errornum == 0 );

	programDestruct( ilp );
	logWriteOkFail( NULL );
}

static void smallProgTest0(  )
{
	program *ilp;
	
	logWriteCheck(NULL, "parse 0 test ... " );

	ilp = parseChar( "class test {}", NULL );

	UNIT_ASSERT( "simple parse 1", ilp->errornum == 0 );

	programDestruct( ilp );
	logWriteOkFail( NULL );
}

static void smallProgTest1(  )
{
	program *ilp;
	
	logWriteCheck(NULL, "parse prog 1 test ... " );

	ilp = parseChar( "class test {\n\tpublic method()\n{var x = 1; return x;}\n \t}\n", NULL );

	UNIT_ASSERT( "simple parse 1", ilp->errornum == 0 );

	programDestruct( ilp );
	logWriteOkFail( NULL );
}

static void smallProgTestFor(  )
{
	program *ilp;
	
	logWriteCheck(NULL, "parse for test ... " );

	ilp = parseChar( "class test {\n\tpublic method()\n{var x; var y = 0;\n for(x = 0; x < 100; x++){++y;}}\n \t}\n", NULL );

	UNIT_ASSERT( "simple parse for", ilp->errornum == 0 );

	programDestruct( ilp );
	logWriteOkFail( NULL );
}

static void smallProgTestWhile(  )
{
	program *ilp;
	
	logWriteCheck(NULL, "parse while test ... " );

	ilp = parseChar( "class test {\n\tpublic method()\n{var x = 0; var y = 0;\n while(x < 100){++y;\nx++;\ncontinue;\n}}\n \t}\n", NULL );

	UNIT_ASSERT( "simple parse for", ilp->errornum == 0 );

	programDestruct( ilp );
}

static void smallProgTestIf(  )
{
	program *ilp = parseChar( "class test {\n\tpublic method()\n{var x = 0; var y = 0;\n if(x < 100){++y;\nx++;\n}}\n \t}\n", NULL );

	UNIT_ASSERT( "simple parse for", ilp->errornum == 0 );

	programDestruct( ilp );
	logWriteOkFail( NULL );
}

static void smallProgTestSwitch(  )
{
	program *ilp;
	
	logWriteCheck(NULL, "parse switch test ... " );

	ilp = parseChar( "class test {\n\tpublic method()\n{var x = 0; var y = 0;\n switch(x){case 0:\ny = 0;\nbreak;\n}}\n \t}\n", NULL );

	UNIT_ASSERT( "simple parse switch", ilp->errornum == 0 );

	programDestruct( ilp );
	logWriteOkFail( NULL );
}

static void smallProgTestVars(  )
{
	program *ilp;
	
	logWriteCheck(NULL, "parse vars test ... " );

	ilp = parseChar( "class test {\n\tpublic method()\n{var x = 0; var y = 0;\nx = y + 1; y *=2; x = y%3; y = x|1;}\n \t}\n", NULL );

	UNIT_ASSERT( "simple vars", ilp->errornum == 0 );

	programDestruct( ilp );
	logWriteOkFail( NULL );
}

static void smallProgTestMem(  )
{
	program *ilp;
	
	logWriteCheck(NULL, "parse new test ... " );

	ilp = parseChar( "class test {public test(){} public toString(){}\n\tpublic static method()\n{var x = new test();\nvar y = x.toString(); delete x;}\n \t}\n", NULL );

	UNIT_ASSERT( "simple parse new", ilp->errornum == 0 );

	programDestruct( ilp );
	logWriteOkFail( NULL );
}

static void smallProgTestArray(  )
{
	qinterp *qip = qinterpConstruct();
	program *ilp;

	qinterpCheckMem( qip );
	UNIT_ASSERT_MEM_NOTED( "smallProgTestArray" );

	logWriteCheck(NULL, "parse array test ... " );

	ilp = parseChar( "class test {\n\tpublic method()\n{var x = new Array(1);\nx[0] = 6;var y = x[0]; delete x;}\n \t}\n", qinterpGetPrograms( qip ) );

	qinterpCheckMem( qip );
	programCheckMem( ilp );
	UNIT_ASSERT_MEM_NOTED( "smallProgTestArray" );

	UNIT_ASSERT( "simple parse if", ilp->errornum == 0 );

	programDestruct( ilp );
	qinterpDestruct( qip );

	UNIT_ASSERT_MEM_NOTED( "smallProgTestArray" );

	logWriteOkFail( NULL );
}

static void smallProgTestClasses(  )
{
	program *ilp;
	
	logWriteCheck(NULL, "parse classes test ... " );

	ilp = parseChar( "class test1 {public var x = 1; public test1(){} }\nclass test2 {public var y = 2; public var _test1 = new test1();\npublic method(arg){_test1.x = 2;}}", NULL );

	UNIT_ASSERT( "simple parse if", ilp->errornum == 0 );

	programDestruct( ilp );
	logWriteOkFail( NULL );
}

static void smallProgTestTry(  )
{
	program *ilp;
	
	logWriteCheck(NULL, "parse try test ... " );

	ilp = parseChar( "class test1 {public mymethod(){var x = 0; try{x /= 0;}catch(exception){}}}", NULL );

	UNIT_ASSERT( "parse try", ilp->errornum == 0 );

	programDestruct( ilp );
	logWriteOkFail( NULL );
}

static void outofClassMethod1(  )
{
	program *ilp;
	
	logWriteCheck(NULL, "parse calls test ... " );

	ilp = parseChar( "class test{}\n public test::method()\n{var x = 1; return x;}\n", NULL );

	UNIT_ASSERT( "parse calls", ilp->errornum == 0 );

	programDestruct( ilp );
	logWriteOkFail( NULL );
}

static void staticArrayDef(  )
{
	program *ilp;
	qinterp *qip = qinterpConstruct();
	
	logWriteCheck(NULL, "parse static array def test ... " );

	ilp = parseChar( "class test{\n private var arr = {1,2,3}; public method()\n{return arr[2];} }\n", qinterpGetPrograms(qip) );

	UNIT_ASSERT( "parse static array", ilp->errornum == 0 );

	qinterpDestruct( qip );
	programDestruct( ilp );
	logWriteOkFail( NULL );
}

static void complexVarDef(  )
{
	program *ilp;
	qinterp *qip = qinterpConstruct();
	
	logWriteCheck(NULL, "parse complex var def test ... " );

	ilp = parseChar( "class test{\n private var LiveVal = 16;\n public method()\n{return LiveVal;} }\n", qinterpGetPrograms(qip) );
	UNIT_ASSERT( "parse complexVarDef 1", ilp->errornum == 0 );
	programDestruct( ilp );

	ilp = parseChar( "class test{\n private var LiveVal = 16; private var cell_live_set = 1<<3; public method()\n{return cell_live_set;} }\n", qinterpGetPrograms(qip) );
	UNIT_ASSERT( "parse complexVarDef 2", ilp->errornum == 0 );
	programDestruct( ilp );

	ilp = parseChar( "class test{\n private var LiveVal = 16; private var cell_live_set = (1<<3); public method()\n{return cell_live_set;} }\n", qinterpGetPrograms(qip) );
	UNIT_ASSERT( "parse complexVarDef 3", ilp->errornum == 0 );
	programDestruct( ilp );

	ilp = parseChar( "class test{\n private var LiveVal = 16; private var cell_live_set = (1<<3) | 1<<(3+LiveVal) | 1<<(2+LiveVal); public method()\n{return cell_live_set;} }\n", qinterpGetPrograms(qip) );
	UNIT_ASSERT( "parse complexVarDef 4", ilp->errornum == 0 );
	programDestruct( ilp );

	qinterpDestruct( qip );
	logWriteOkFail( NULL );
}

static void parseStaticArray(  )
{
	program *pp;
	qinterp *qip = qinterpConstruct();
	
	logWriteCheck(NULL, "parse static array test ... " );

	pp = parseChar( "class test{\n private static var ar = {1,2,3};\n public method()\n{var sum = 0; for(var x = 0; x < test::ar.length(0); x++) {sum += test::ar[x];} return sum;} }\n", qinterpGetPrograms(qip) );
	UNIT_ASSERT( "parse parseStaticArray", pp->errornum == 0 );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	programCheckMem( pp );
	qinterpCheckMem( qip );
	UNIT_ASSERT_MEM_NOTED( "parseStaticArray" );
	
	programDestruct( pp );

	DEBUG_CLEAR_MEM_CHECK_POINTS();
	qinterpCheckMem( qip );
	UNIT_ASSERT_MEM_NOTED( "parseStaticArray" );

	qinterpDestruct( qip );
	logWriteOkFail( NULL );
}

static void parseErrorMemCheck(  )
{
	program *pp;
	qinterp *qip = qinterpConstruct();
	
	logWriteCheck(NULL, "parse error mem leak test ... " );

	DEBUG_CLEAR_MEM_CHECK_POINTS();
	qinterpCheckMem( qip );
	UNIT_ASSERT_MEM_NOTED( "parseStaticArray start" );

	pp = parseChar( "class test{\npublic method()\n{return sum;} }\n", qinterpGetPrograms(qip) );
	UNIT_ASSERT( "parse parseStaticArray", pp->errornum != 0 );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	qinterpCheckMem( qip );
	programCheckMem( pp );
	UNIT_ASSERT_MEM_NOTED( "parseStaticArray open" );

	programDestruct( pp );
	qinterpDestruct( qip );

	UNIT_ASSERT_MEM_NOTED( "parseStaticArray end" );

	logWriteOkFail( NULL );
}

static void anotherStaticRefTest(  )
{
	program *pp;
	qinterp *qip = qinterpConstruct();
	
	logWriteCheck(NULL, "parse static ref 2 ... " );

	DEBUG_CLEAR_MEM_CHECK_POINTS();
	qinterpCheckMem( qip );
	UNIT_ASSERT_MEM_NOTED( "parseStaticArray start" );

	pp = parseChar( "class test{\nprivate static var wantedTime = 1000;\npublic method()\n{var rate = 1;\nvar cycles = rate*test::wantedTime/10.0; return cycles;} }\n", qinterpGetPrograms(qip) );
	UNIT_ASSERT( "anotherStaticRefTest", pp->errornum == 0 );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	qinterpCheckMem( qip );
	programCheckMem( pp );
	UNIT_ASSERT_MEM_NOTED( "anotherStaticRefTest open" );

	programDestruct( pp );
	qinterpDestruct( qip );

	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "anotherStaticRefTest end" );

	logWriteOkFail( NULL );
}

static void parseStaticAssignment(  )
{
	program *pp;
	qinterp *qip = qinterpConstruct();
	
	logWriteCheck(NULL, "parse static assigment ... " );

	qinterpCheckMem( qip );
	UNIT_ASSERT_MEM_NOTED( "parseStaticAssignment start" );

	pp = parseChar( "class test{\nprivate static var wantedTime = new Vector();\npublic method()\n{test::wantedTime = 1;} }\n", qinterpGetPrograms(qip) );
	UNIT_ASSERT( "parseStaticAssignment", pp->errornum == 0 );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	qinterpCheckMem( qip );
	programCheckMem( pp );
	UNIT_ASSERT_MEM_NOTED( "parseStaticAssignment open" );

	programDestruct( pp );
	qinterpDestruct( qip );

	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "parseStaticAssignment end" );

	logWriteOkFail( NULL );
}

static void parseConstrDynArg(  )
{
	program *pp;
	qinterp *qip = qinterpConstruct();
	
	logWriteCheck(NULL, "parse dyna construct arg ... " );

	qinterpCheckMem( qip );
	UNIT_ASSERT_MEM_NOTED( "parseConstrDynArg start" );

	pp = parseChar( "class test{\nprivate var i = 1;\npublic method()\n{var ar = new Array(i);} }\n", qinterpGetPrograms(qip) );
	UNIT_ASSERT( "parseStaticAssignment", pp->errornum == 0 );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	qinterpCheckMem( qip );
	programCheckMem( pp );
	UNIT_ASSERT_MEM_NOTED( "parseConstrDynArg open" );

	programDestruct( pp );
	qinterpDestruct( qip );

	UNIT_ASSERT_MEM_NOTED( "parseConstrDynArg end" );

	logWriteOkFail( NULL );
}

void testparse(  )
{
	smallProgTest0( );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "testparse 1" );

	smallProgTest00( );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "testparse 2" );

	smallProgTest1( );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "testparse 3" );

	smallProgTestFor( );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "testparse 4" );

	smallProgTestWhile( );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "testparse 5" );

	smallProgTestIf( );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "testparse 6" );

	smallProgTestSwitch( );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "testparse 7" );

	smallProgTestVars( );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "testparse 8" );

	smallProgTestMem( );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "testparse 9" );

	smallProgTestArray( );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "testparse 10" );

	smallProgTestClasses( );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "testparse 11" );

	smallProgTestTry( );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "testparse 12" );

	outofClassMethod1( );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "testparse 13" );

	staticArrayDef( );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "testparse 14" );

	complexVarDef( );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "parse complex var der" );

	parseStaticArray( );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "parseStaticArray" );

	parseErrorMemCheck();
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "parseErrorMemCheck" );

	anotherStaticRefTest( );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "anotherStaticRefTest" );

	parseStaticAssignment( );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "parseStaticAssignment" );

	parseConstrDynArg( );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "parseConstrDynArg" );
}

