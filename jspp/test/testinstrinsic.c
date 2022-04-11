#include <spl/debug.h>
#include <jspp/intrinsics.h>
#include <jspp/parse.h>

#ifdef DEBUG

static void testObjectParseInt(  )
{
	qinterp *qip;
	program *pp;
	enum InterpErrors err;
	variant *retval = NULL;
	bool prepareOk;
	
	logWriteCheck( NULL, "object parseInt test ... " );

	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "testObjectParseInt" );

	qip = qinterpConstruct();
	pp = parseChar( 
		"class test {\n" \
		"public static main1(){return Object::parseInt(100);}\n" \
		"public static main2(){return Object::parseInt(100.1);}\n" \
		"public static main3(){return Object::parseInt(\"100\");}\n" \
		"public static main4(){return Object::parseInt(\"100.1\");}\n" \
		"public static main5(){var x = 1; return Object::parseInt(x);}\n" \
		"}\n", qinterpGetPrograms(qip) );

	UNIT_ASSERT( "testObjectParseInt parse", pp->errornum == 0 );
	/*programDump( pp, "c:\\temp\\objectparseint.txt" );*/

	prepareOk = qinterpPrepare( qip, pp );
	UNIT_ASSERT( "compTest1 prepare", prepareOk );

	DEBUG_CLEAR_MEM_CHECK_POINTS();
	qinterpCheckMem( qip );
	UNIT_ASSERT_MEM_NOTED( "testObjectParseInt" );

	err = qinterpRun( qip, "test", "main1", 0, &retval );
	UNIT_ASSERT( "testObjectParseInt interp error", err == IERR_NONE );
	UNIT_ASSERT( "testObjectParseInt return value not NULL", NULL != retval );
	UNIT_ASSERT( "testObjectParseInt return value", retval->data.i == 100 );
	varDestruct( retval );

	DEBUG_CLEAR_MEM_CHECK_POINTS();
	qinterpCheckMem( qip );
	DEBUG_DUMP_MEM_LEAKS();
	UNIT_ASSERT_MEM_NOTED( "testObjectParseInt" );

	err = qinterpRun( qip, "test", "main2", 0, &retval );
	UNIT_ASSERT( "testObjectParseInt interp error", err == IERR_NONE );
	UNIT_ASSERT( "testObjectParseInt return value not NULL", NULL != retval );
	UNIT_ASSERT( "testObjectParseInt return value", TYPE_INT == retval->type && retval->data.i == 100 );
	varDestruct( retval );

	DEBUG_CLEAR_MEM_CHECK_POINTS();
	qinterpCheckMem( qip );
	UNIT_ASSERT_MEM_NOTED( "testObjectParseInt" );

	err = qinterpRun( qip, "test", "main3", 0, &retval );
	UNIT_ASSERT( "testObjectParseInt interp error", err == IERR_NONE );
	UNIT_ASSERT( "testObjectParseInt return value not NULL", NULL != retval );
	UNIT_ASSERT( "testObjectParseInt return value", TYPE_INT == retval->type && retval->data.i == 100 );
	varDestruct( retval );

	DEBUG_CLEAR_MEM_CHECK_POINTS();
	qinterpCheckMem( qip );
	DEBUG_DUMP_MEM_LEAKS();
	UNIT_ASSERT_MEM_NOTED( "testObjectParseInt" );

	err = qinterpRun( qip, "test", "main4", 0, &retval );
	UNIT_ASSERT( "testObjectParseInt interp error", err == IERR_NONE );
	UNIT_ASSERT( "testObjectParseInt return value not NULL", NULL != retval );
	UNIT_ASSERT( "testObjectParseInt return value", TYPE_INT == retval->type && retval->data.i == 100 );
	varDestruct( retval );

	err = qinterpRun( qip, "test", "main5", 0, &retval );
	UNIT_ASSERT( "testObjectParseInt interp error", err == IERR_NONE );
	UNIT_ASSERT( "testObjectParseInt return value not NULL", NULL != retval );
	UNIT_ASSERT( "testObjectParseInt return value", TYPE_INT == retval->type && retval->data.i == 1 );
	varDestruct( retval );

	qinterpDestruct( qip );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "testObjectParseInt" );
	logWriteOkFail( NULL );
}

static void testObjectParseFloat(  )
{
	qinterp *qip;
	program *pp;
	enum InterpErrors err;
	variant *retval = NULL;
	bool prepareOk;
	float f;
	int i;
	
	logWriteCheck( NULL, "object parseFloat test ... " );

	qip = qinterpConstruct();
	pp = parseChar( 
		"class test {\n" \
		"public static main1(){return Object::parseFloat(100);}\n" \
		"public static main2(){return Object::parseFloat(100.1);}\n" \
		"public static main3(){return Object::parseFloat(\"100\");}\n" \
		"public static main4(){return Object::parseFloat(\"100.7\");}\n" \
		"public static main5(){var x = 1.9; return Object::parseFloat(x);}\n" \
		"}\n", qinterpGetPrograms(qip) );

	UNIT_ASSERT( "testObjectParseInt parse", pp->errornum == 0 );

	prepareOk = qinterpPrepare( qip, pp );
	UNIT_ASSERT( "compTest1 prepare", prepareOk );

	err = qinterpRun( qip, "test", "main1", 0, &retval );
	UNIT_ASSERT( "testObjectParseInt interp error", err == IERR_NONE );
	UNIT_ASSERT( "testObjectParseInt return value not NULL", NULL != retval );
	UNIT_ASSERT( "testObjectParseInt return value", TYPE_FLOAT == retval->type && retval->data.f == 100 );
	varDestruct( retval );

	err = qinterpRun( qip, "test", "main2", 0, &retval );
	UNIT_ASSERT( "testObjectParseInt interp error", err == IERR_NONE );
	UNIT_ASSERT( "testObjectParseInt return value not NULL", NULL != retval );
	f = retval->data.f;
	f *= 100;
	i = (int)f;
	// VC6 return 10009
	UNIT_ASSERT( "testObjectParseInt return value", TYPE_FLOAT == retval->type && (i == 10010 || i == 10009) );
	varDestruct( retval );

	err = qinterpRun( qip, "test", "main3", 0, &retval );
	UNIT_ASSERT( "testObjectParseInt interp error", err == IERR_NONE );
	UNIT_ASSERT( "testObjectParseInt return value not NULL", NULL != retval );
	UNIT_ASSERT( "testObjectParseInt return value", TYPE_FLOAT == retval->type && retval->data.f == 100 );
	varDestruct( retval );

	err = qinterpRun( qip, "test", "main4", 0, &retval );
	UNIT_ASSERT( "testObjectParseInt interp error", err == IERR_NONE );
	UNIT_ASSERT( "testObjectParseInt return value not NULL", NULL != retval );
	f = 100 * retval->data.f;
	i = (int)f;
	// VC6 returns 10069
	UNIT_ASSERT( "testObjectParseInt return value", TYPE_FLOAT == retval->type && (i == 10070 || i == 10069) );
	varDestruct( retval );

	err = qinterpRun( qip, "test", "main5", 0, &retval );
	UNIT_ASSERT( "testObjectParseInt interp error", err == IERR_NONE );
	UNIT_ASSERT( "testObjectParseInt return value not NULL", NULL != retval );
	f = 100 * retval->data.f;
	i = (int)f;
	// VC6 returns 189
	UNIT_ASSERT( "testObjectParseInt return value", TYPE_FLOAT == retval->type && (i == 190 || i == 189) );
	varDestruct( retval );

	qinterpDestruct( qip );
	logWriteOkFail( NULL );
}

static void testObjectToString(  )
{
	qinterp *qip;
	program *pp;
	enum InterpErrors err;
	variant *retval = NULL;
	bool prepareOk;
	
	logWriteCheck( NULL, "object toString test ... " );

	qip = qinterpConstruct();
	pp = parseChar( 
		"class test {\n" \
		"public static main1(){return Object::toString(100);}\n" \
		"public static main2(){return Object::toString(100.1);}\n" \
		"public static main3(){return Object::toString(\"hi\");}\n" \
		"public static main4(){return Object::toString(true);}\n" \
		"public static main5(){var x = 1; return Object::toString(x);}\n" \
		"}\n", qinterpGetPrograms(qip) );	

	UNIT_ASSERT( "testObjectParseInt parse", pp->errornum == 0 );

	prepareOk = qinterpPrepare( qip, pp );
	UNIT_ASSERT( "compTest1 prepare", prepareOk );

	err = qinterpRun( qip, "test", "main1", 0, &retval );
	UNIT_ASSERT( "toString interp error", err == IERR_NONE );
	UNIT_ASSERT( "toString return value not NULL", NULL != retval );
	UNIT_ASSERT( "toString return value", TYPE_STRING == retval->type && strcmp(retval->data.sp->cstr, "100") == 0 );
	varDestruct( retval );

	err = qinterpRun( qip, "test", "main2", 0, &retval );
	UNIT_ASSERT( "toString interp error", err == IERR_NONE );
	UNIT_ASSERT( "toString return value not NULL", NULL != retval );
	UNIT_ASSERT( "toString return value", TYPE_STRING == retval->type && strncmp(retval->data.sp->cstr, "100.", 4) == 0 );
	varDestruct( retval );

	err = qinterpRun( qip, "test", "main3", 0, &retval );
	UNIT_ASSERT( "toString interp error", err == IERR_NONE );
	UNIT_ASSERT( "toString return value not NULL", NULL != retval );
	UNIT_ASSERT( "toString return value", TYPE_STRING == retval->type && strcmp(retval->data.sp->cstr, "hi") == 0 );
	varDestruct( retval );

	err = qinterpRun( qip, "test", "main4", 0, &retval );
	UNIT_ASSERT( "toString interp error", err == IERR_NONE );
	UNIT_ASSERT( "toString return value not NULL", NULL != retval );
	UNIT_ASSERT( "toString return value", TYPE_STRING == retval->type && strcmp(retval->data.sp->cstr, "1") == 0 );
	varDestruct( retval );

	err = qinterpRun( qip, "test", "main5", 0, &retval );
	UNIT_ASSERT( "toString interp error", err == IERR_NONE );
	UNIT_ASSERT( "toString return value not NULL", NULL != retval );
	UNIT_ASSERT( "toString return value", TYPE_STRING == retval->type && strcmp(retval->data.sp->cstr, "1") == 0 );
	varDestruct( retval );

	qinterpDestruct( qip );
	logWriteOkFail( NULL );
}

static void testArray(  )
{
	qinterp *qip;
	program *pp;
	enum InterpErrors err;
	variant *retval = NULL;
	bool prepareOk;
	
	logWriteCheck( NULL, "array test ... " );

	qip = qinterpConstruct();
	pp = parseChar( 
		"class test {\n" \
		"public static main1(){var ar = new Array(1); return ar;}\n" \
		"}\n", qinterpGetPrograms(qip) );	

	UNIT_ASSERT( "testArray parse", pp->errornum == 0 );

	prepareOk = qinterpPrepare( qip, pp );
	UNIT_ASSERT( "testArray prepare", prepareOk );

	err = qinterpRun( qip, "test", "main1", 0, &retval );
	UNIT_ASSERT( "testArray interp error", err == IERR_NONE );
	varDestruct( retval );
	qinterpCheckMem( qip );
	UNIT_ASSERT_MEM_NOTED( "testArray" );

	qinterpDestruct( qip );
	logWriteOkFail( NULL );
}

static void testArray2(  )
{
	qinterp *qip;
	program *pp;
	enum InterpErrors err;
	variant *retval = NULL;
	bool prepareOk;
	
	logWriteCheck( NULL, "array 2 test ... " );

	qip = qinterpConstruct();
	pp = parseChar( 
		"class test {\n" \
		"public static main2(){var ar = new Array(1); ar[0] = \"mom\"; return ar[0];}\n" \
		"}\n", qinterpGetPrograms(qip) );	

	/*programDump( pp, "c:\\temp\\testarray2.txt" );*/

	UNIT_ASSERT( "testArray2 parse", pp->errornum == 0 );

	prepareOk = qinterpPrepare( qip, pp );
	UNIT_ASSERT( "testArray prepare", prepareOk );

	err = qinterpRun( qip, "test", "main2", 0, &retval );
	UNIT_ASSERT( "testArray2 interp error", err == IERR_NONE );
	UNIT_ASSERT( "testArray2 return value not NULL", NULL != retval );
	UNIT_ASSERT( "testArray2 return value", TYPE_STRING == retval->type && strcmp(retval->data.sp->cstr, "mom") == 0 );
	varDestruct( retval );
	qinterpCheckMem( qip );
	UNIT_ASSERT_MEM_NOTED( "testArray2" );

	qinterpDestruct( qip );
	logWriteOkFail( NULL );
}

static void testArrayDim2(  )
{
	qinterp *qip;
	program *pp;
	enum InterpErrors err;
	variant *retval = NULL;
	bool prepareOk;
	
	logWriteCheck( NULL, "array dim 2 test ... " );

	qip = qinterpConstruct();
	pp = parseChar( 
		"class test {\n" \
		"public static main2(){var ar = new Array(1,1); ar[0,0] = \"mom\"; return ar[0,0];}\n" \
		"}\n", qinterpGetPrograms(qip) );	

	UNIT_ASSERT( "testArrayDim2 parse", pp->errornum == 0 );

	prepareOk = qinterpPrepare( qip, pp );
	UNIT_ASSERT( "testArrayDim2 prepare", prepareOk );

	err = qinterpRun( qip, "test", "main2", 0, &retval );
	UNIT_ASSERT( "testArrayDim2 interp error", err == IERR_NONE );
	UNIT_ASSERT( "testArrayDim2 return value not NULL", NULL != retval );
	UNIT_ASSERT( "testArrayDim2 return value", TYPE_STRING == retval->type && strcmp(retval->data.sp->cstr, "mom") == 0 );
	varDestruct( retval );
	qinterpCheckMem( qip );
	UNIT_ASSERT_MEM_NOTED( "testArrayDim2" );

	qinterpDestruct( qip );
	logWriteOkFail( NULL );
}

static void testArrayDim3(  )
{
	qinterp *qip;
	program *pp;
	enum InterpErrors err;
	variant *retval = NULL;
	bool prepareOk;
	
	logWriteCheck( NULL, "array dim 3 test ... " );

	qip = qinterpConstruct();
	pp = parseChar( 
		"class test {\n" \
		"public static main2(){var ar = new Array(1,1,1); ar[0,0,0] = \"mom\"; return ar[0,0,0];}\n" \
		"}\n", qinterpGetPrograms(qip) );	

	UNIT_ASSERT( "testArrayDim3 parse", pp->errornum == 0 );

	prepareOk = qinterpPrepare( qip, pp );
	UNIT_ASSERT( "testArrayDim3 prepare", prepareOk );

	err = qinterpRun( qip, "test", "main2", 0, &retval );
	UNIT_ASSERT( "testArrayDim3 interp error", err == IERR_NONE );
	UNIT_ASSERT( "testArrayDim3 return value not NULL", NULL != retval );
	UNIT_ASSERT( "testArrayDim3 return value", TYPE_STRING == retval->type && strcmp(retval->data.sp->cstr, "mom") == 0 );
	varDestruct( retval );
	qinterpCheckMem( qip );
	UNIT_ASSERT_MEM_NOTED( "testArrayDim3" );

	qinterpDestruct( qip );
	logWriteOkFail( NULL );
}

static void testArrayLen(  )
{
	qinterp *qip;
	program *pp;
	enum InterpErrors err;
	variant *retval = NULL;
	bool prepareOk;
	
	logWriteCheck( NULL, "array len test ... " );

	qip = qinterpConstruct();
	pp = parseChar( 
		"class test {\n" \
		"public static main2(){var ar = new Array(10); return ar.length(0);}\n" \
		"public static main3(){var ar = new Array(2, 2); return ar.length(1);}\n" \
		"public static main4(){var ar = new Array(10, 10, 10); return ar.length(0);}\n" \
		"public static main5(){var ar = new Array(10, 10, 10); return ar.dims();}\n" \
		"}\n", qinterpGetPrograms(qip) );	

	UNIT_ASSERT( "testArrayDim3 parse", pp->errornum == 0 );

	prepareOk = qinterpPrepare( qip, pp );
	UNIT_ASSERT( "testArrayDim3 prepare", prepareOk );

	err = qinterpRun( qip, "test", "main2", 0, &retval );
	UNIT_ASSERT( "testArrayDim3 interp error", err == IERR_NONE );
	UNIT_ASSERT( "testArrayDim3 return value not NULL", NULL != retval );
	UNIT_ASSERT( "testArrayDim3 return value", TYPE_INT == retval->type && retval->data.i == 10 );
	varDestruct( retval );

	err = qinterpRun( qip, "test", "main3", 0, &retval );
	UNIT_ASSERT( "testArrayDim3 interp error", err == IERR_NONE );
	UNIT_ASSERT( "testArrayDim3 return value not NULL", NULL != retval );
	UNIT_ASSERT( "testArrayDim3 return value", TYPE_INT == retval->type && retval->data.i == 2 );
	varDestruct( retval );

	err = qinterpRun( qip, "test", "main4", 0, &retval );
	UNIT_ASSERT( "testArrayDim3 interp error", err == IERR_NONE );
	UNIT_ASSERT( "testArrayDim3 return value not NULL", NULL != retval );
	UNIT_ASSERT( "testArrayDim3 return value", TYPE_INT == retval->type && retval->data.i == 10 );
	varDestruct( retval );

	err = qinterpRun( qip, "test", "main5", 0, &retval );
	UNIT_ASSERT( "testArrayDim3 interp error", err == IERR_NONE );
	UNIT_ASSERT( "testArrayDim3 return value not NULL", NULL != retval );
	UNIT_ASSERT( "testArrayDim3 return value", TYPE_INT == retval->type && retval->data.i == 3 );
	varDestruct( retval );

	qinterpCheckMem( qip );
	UNIT_ASSERT_MEM_NOTED( "testArrayDim3" );

	qinterpDestruct( qip );
	logWriteOkFail( NULL );
}

static void testArrayMore(  )
{
	qinterp *qip;
	program *pp;
	enum InterpErrors err;
	variant *retval = NULL;
	bool prepareOk;
	
	logWriteCheck( NULL, "array test ... " );

	qip = qinterpConstruct();
	pp = parseChar( 
		"class test {\n" \
		"public static main5(){var ar = new Array(10); for ( var x = 0; x < 10; x++ ) { ar[x] = x; } return ar[5];}\n" \
		"public static main6(){var ar = new Array(10); for ( var x = 0; x < ar.length(0); x++ ) { ar[x] = x; } return ar[5];}\n" \
		"public static main3(){var ar = new Array(10,10,10); ar[10,10,10] = \"mom\"; return ar[10,10,10];}\n" \
		"public static main4(){var ar = new Array(10,10,10); ar[10,10,10] = \"mom\"; return ar[10,10,9];}\n" \
		"}\n", qinterpGetPrograms(qip) );	

	UNIT_ASSERT( "testArrayMore parse", pp->errornum == 0 );

	prepareOk = qinterpPrepare( qip, pp );
	UNIT_ASSERT( "testArrayMore prepare", prepareOk );

	err = qinterpRun( qip, "test", "main3", 0, &retval );
	UNIT_ASSERT( "testArrayMore interp error", err == IERR_NONE );
	UNIT_ASSERT( "testArrayMore return value not NULL", NULL != retval );
	UNIT_ASSERT( "testArrayMore return value", TYPE_STRING == retval->type && strcmp(retval->data.sp->cstr, "mom") == 0 );
	varDestruct( retval );

	err = qinterpRun( qip, "test", "main4", 0, &retval );
	UNIT_ASSERT( "testArrayMore interp error", err == IERR_NONE );
	UNIT_ASSERT( "testArrayMore return value not NULL", NULL != retval );
	UNIT_ASSERT( "testArrayMore return value", TYPE_NULL == retval->type );
	varDestruct( retval );

	err = qinterpRun( qip, "test", "main5", 0, &retval );
	UNIT_ASSERT( "testArrayMore interp error", err == IERR_NONE );
	UNIT_ASSERT( "testArrayMore return value not NULL", NULL != retval );
	UNIT_ASSERT( "testArrayMore return value", TYPE_INT == retval->type && retval->data.i == 5 );
	varDestruct( retval );

	err = qinterpRun( qip, "test", "main6", 0, &retval );
	UNIT_ASSERT( "testArrayMore interp error", err == IERR_NONE );
	UNIT_ASSERT( "testArrayMore return value not NULL", NULL != retval );
	UNIT_ASSERT( "testArrayMore return value", TYPE_INT == retval->type && retval->data.i == 5 );
	varDestruct( retval );

	qinterpCheckMem( qip );
	UNIT_ASSERT_MEM_NOTED( "testArrayMore" );

	qinterpDestruct( qip );
	logWriteOkFail( NULL );
}

static void testVector(  )
{
	qinterp *qip;
	program *pp;
	enum InterpErrors err;
	variant *retval = NULL;
	bool prepareOk;
	
	logWriteCheck( NULL, "vector test ... " );

	qip = qinterpConstruct();
	pp = parseChar( 
		"class test {\n" \
		"public static main1(){var ar = new Vector(); for ( var x = 0; x < 10; x++ ) { ar.add(x); } return ar.elementAt(5);}\n" \
		"public static main2(){var ar = new Vector(); for ( var x = 0; x < 10; x++ ) { ar.add(x); } return ar[9];}\n" \
		"public static main3(){var ar = new Vector(); for ( var x = 0; x < 10; x++ ) { ar.add(x); } return ar.size();}\n" \
		"public static main4(){var ar = new Vector(); for ( var x = 0; x < 10; x++ ) { ar.add(x); } ar.clear(); return ar.size();}\n" \
		"public static main5(){var ar = new Vector(); for ( var x = 0; x < 10; x++ ) { ar.add(x); } ar.setElementAt(5, 22); return ar[5];}\n" \
		"}\n", qinterpGetPrograms(qip) );	

	UNIT_ASSERT( "testVector parse", pp->errornum == 0 );

	prepareOk = qinterpPrepare( qip, pp );
	UNIT_ASSERT( "testVector prepare", prepareOk );

	err = qinterpRun( qip, "test", "main1", 0, &retval );
	UNIT_ASSERT( "testVector interp error", err == IERR_NONE );
	UNIT_ASSERT( "testVector return value not NULL", NULL != retval );
	UNIT_ASSERT( "testVector return value", TYPE_INT == retval->type && retval->data.i == 5 );
	varDestruct( retval );

	err = qinterpRun( qip, "test", "main2", 0, &retval );
	UNIT_ASSERT( "testVector interp error", err == IERR_NONE );
	UNIT_ASSERT( "testVector return value not NULL", NULL != retval );
	UNIT_ASSERT( "testVector return value", TYPE_INT == retval->type && retval->data.i == 9 );
	varDestruct( retval );

	err = qinterpRun( qip, "test", "main3", 0, &retval );
	UNIT_ASSERT( "testVector interp error", err == IERR_NONE );
	UNIT_ASSERT( "testVector return value not NULL", NULL != retval );
	UNIT_ASSERT( "testVector return value", TYPE_INT == retval->type && retval->data.i == 10 );
	varDestruct( retval );

	err = qinterpRun( qip, "test", "main4", 0, &retval );
	UNIT_ASSERT( "testVector interp error", err == IERR_NONE );
	UNIT_ASSERT( "testVector return value not NULL", NULL != retval );
	UNIT_ASSERT( "testVector return value", TYPE_INT == retval->type && retval->data.i == 0 );
	varDestruct( retval );

	err = qinterpRun( qip, "test", "main5", 0, &retval );
	UNIT_ASSERT( "testVector interp error", err == IERR_NONE );
	UNIT_ASSERT( "testVector return value not NULL", NULL != retval );
	UNIT_ASSERT( "testVector return value", TYPE_INT == retval->type && retval->data.i == 22 );
	varDestruct( retval );

	qinterpCheckMem( qip );
	UNIT_ASSERT_MEM_NOTED( "testVector" );

	qinterpDestruct( qip );
	logWriteOkFail( NULL );
}

#ifdef CONSOLE
static void consoleTest(  )
{
	qinterp *qip;
	program *pp;
	enum InterpErrors err;
	variant *retval = NULL;
	bool prepareOk;
	
	logWriteCheck( NULL, "console test ... " );

	qip = qinterpConstruct();
	pp = parseChar( 
		"class test {\n" \
		"public static main1(){Console::write(\" ... \");}\n" \
		"}\n", qinterpGetPrograms(qip) );	

	UNIT_ASSERT( "consoleTest parse", pp->errornum == 0 );

	prepareOk = qinterpPrepare( qip, pp );
	UNIT_ASSERT( "consoleTest prepare", prepareOk );

	err = qinterpRun( qip, "test", "main1", 0, &retval );
	UNIT_ASSERT( "consoleTest interp error", err == IERR_NONE );
	UNIT_ASSERT( "consoleTest return value not NULL", NULL == retval );

	qinterpCheckMem( qip );
	UNIT_ASSERT_MEM_NOTED( "consoleTest" );

	qinterpDestruct( qip );
	logWriteOkFail( NULL );
}
#endif

static void strbufTest(  )
{
	qinterp *qip;
	program *pp;
	enum InterpErrors err;
	variant *retval = NULL;
	bool prepareOk;
	
	logWriteCheck( NULL, "stringbuffer test ... " );

	qip = qinterpConstruct();
	pp = parseChar( 
		"class test {\n" \
		"public static main1(){var ar = new StringBuffer(); for ( var x = 0; x < 10; x++ ) { ar.append(x); } return ar.toString();}\n" \
		"}\n", qinterpGetPrograms(qip) );	

	UNIT_ASSERT( "strbufTest parse", pp->errornum == 0 );

	prepareOk = qinterpPrepare( qip, pp );
	UNIT_ASSERT( "strbufTest prepare", prepareOk );

	err = qinterpRun( qip, "test", "main1", 0, &retval );
	UNIT_ASSERT( "strbufTest interp error", err == IERR_NONE );
	UNIT_ASSERT( "strbufTest return value not NULL", NULL != retval );
	UNIT_ASSERT( "strbufTest return value", TYPE_STRING == retval->type && strcmp(retval->data.sp->cstr, "0123456789") == 0 );
	varDestruct( retval );

	qinterpCheckMem( qip );
	UNIT_ASSERT_MEM_NOTED( "strbufTest" );

	qinterpDestruct( qip );
	logWriteOkFail( NULL );
}

static void datetimeTest(  )
{
	qinterp *qip;
	program *pp;
	enum InterpErrors err;
	variant *retval = NULL;
	bool prepareOk;
	
	logWriteCheck( NULL, "datetimeTest test ... " );

	qip = qinterpConstruct();
	pp = parseChar( 
		"class test {\n" \
		"public static main1(){var x = DateTime::now(); return x.toMilliseconds();}\n" \
		"}\n", qinterpGetPrograms(qip) );	

	UNIT_ASSERT( "strbufTest parse", pp->errornum == 0 );

	prepareOk = qinterpPrepare( qip, pp );
	UNIT_ASSERT( "strbufTest prepare", prepareOk );

	/*programDump( pp, "c:\\temp\\datetime.now.txt" );*/

	err = qinterpRun( qip, "test", "main1", 0, &retval );
	UNIT_ASSERT( "datetimeTest interp error", err == IERR_NONE );
	UNIT_ASSERT( "datetimeTest return value not NULL", NULL != retval );
	UNIT_ASSERT( "datetimeTest return value", TYPE_INT == retval->type && retval->data.i > 10000 );
	varDestruct( retval );

	qinterpCheckMem( qip );
	UNIT_ASSERT_MEM_NOTED( "datetimeTest" );

	qinterpDestruct( qip );
	logWriteOkFail( NULL );
}

static void mathTest(  )
{
	qinterp *qip;
	program *pp;
	enum InterpErrors err;
	variant *retval = NULL;
	bool prepareOk;
	
	logWriteCheck( NULL, "math test ... " );

	qip = qinterpConstruct();
	pp = parseChar( 
		"class test {\n" \
		"public static main1(){return Math::rand() == Math::rand();}\n" \
		"}\n", qinterpGetPrograms(qip) );	

	UNIT_ASSERT( "mathTest parse", pp->errornum == 0 );

	prepareOk = qinterpPrepare( qip, pp );
	UNIT_ASSERT( "mathTest prepare", prepareOk );

	/*programDump( pp, "c:\\temp\\datetime.now.txt" );*/

	err = qinterpRun( qip, "test", "main1", 0, &retval );
	UNIT_ASSERT( "mathTest interp error", err == IERR_NONE );
	UNIT_ASSERT( "mathTest return value not NULL", NULL != retval );
	UNIT_ASSERT( "mathTest return value", TYPE_BOOL == retval->type && retval->data.i == false );
	varDestruct( retval );

	qinterpCheckMem( qip );
	UNIT_ASSERT_MEM_NOTED( "mathTest" );

	qinterpDestruct( qip );
	logWriteOkFail( NULL );
}

static void hashtableTest(  )
{
	qinterp *qip;
	program *pp;
	enum InterpErrors err;
	variant *retval = NULL;
	bool prepareOk;
	
	logWriteCheck( NULL, "hashtable test ... " );

	qip = qinterpConstruct();
	pp = parseChar( 
		"class test {\n" \
		"public static main1(){var ht = new Hashtable(); ht.put(\"key1\", 4); ht.put(\"key2\", 9); return ht.get(\"key1\");}\n" \
		"}\n", qinterpGetPrograms(qip) );	

	UNIT_ASSERT( "hashtableTest parse", pp->errornum == 0 );

	prepareOk = qinterpPrepare( qip, pp );
	UNIT_ASSERT( "hashtableTest prepare", prepareOk );

	/*programDump( pp, "c:\\temp\\datetime.now.txt" );*/

	err = qinterpRun( qip, "test", "main1", 0, &retval );
	UNIT_ASSERT( "hashtableTest interp error", err == IERR_NONE );
	UNIT_ASSERT( "hashtableTest return value not NULL", NULL != retval );
	UNIT_ASSERT( "hashtableTest return value", TYPE_INT == retval->type && retval->data.i == 4 );
	varDestruct( retval );

	qinterpCheckMem( qip );
	DEBUG_DUMP_MEM_LEAKS();
	UNIT_ASSERT_MEM_NOTED( "hashtableTest" );

	qinterpDestruct( qip );
	logWriteOkFail( NULL );
}

void testintrins(  )
{
	testObjectParseInt(  );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "testObjectParseInt" );

	testObjectParseFloat(  );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "testObjectParseFloat" );

	testObjectToString(  );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "testObjectToString" );

	testArray(  );
	UNIT_ASSERT_MEM_NOTED( "testArray" );

	testArray2(  );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "testArray" );

	testArrayDim2(  );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "testArrayDim2" );

	testArrayDim3(  );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "testArrayDim3" );

	testArrayLen(  );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "testArrayLen" );

	testArrayMore(  );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "testArrayMore" );
	
	testVector(  );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "testVector" );

#ifdef CONSOLE
	consoleTest(  );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "testConsole" );
#endif

	strbufTest(  );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "strbufTest" );

	datetimeTest(  );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "datetimeTest" );

	mathTest(  );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "mathTest" );

	hashtableTest(  );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "hashtableTest" );
}

#endif /*DEBUG*/



