#include <spl/debug.h>
#include <jspp/interp.h>

#ifdef DEBUG

extern void progtestEmitStaticAdd( program *pp );
extern void programEmitStaticFactorial( program *pp, int i );
extern void programEmitSimpleNew( program *pp, char *clsname );

static void createDestroy(  )
{
	qinterp *qip;

	logWriteCheck(NULL, "interp create/destroy test ... " );

	UNIT_ASSERT_MEM_NOTED( "qinterp createDestroy NULL" );

	DEBUG_CHECK_POINT_HEAP();

	qip = qinterpConstruct();
	qinterpCheckMem( qip );
	UNIT_ASSERT_MEM_NOTED( "qinterp createDestroy" );
	qinterpDestruct( qip );

	UNIT_ASSERT_CHECK_POINT( "qinterp createDestroy" );
	logWriteOkFail( NULL );
}

static void interp1(  )
{
	qinterp *qip;
	program *pp;
	jsclass *jsp;
	enum InterpErrors err;
	variant *loc;
	variant *retval = NULL;

	logWriteCheck(NULL, "interp 1 test ... " );

	qip = qinterpConstruct();
	qinterpValidateMem( qip );
	pp = programConstruct();	/* destroyed by interp */
	programValidateMem( pp );
	progtestEmitStaticAdd( pp );
	programValidateMem( pp );
	qinterpPrepare( qip, pp );
	qinterpValidateMem( qip );

	qinterpCheckMem( qip );
	UNIT_ASSERT_MEM_NOTED( "qinterp interp1" );

	err = qinterpRun( qip, "test", "call", 0, &retval );
	UNIT_ASSERT( "interp1", err == IERR_NONE );
	if ( NULL != retval )
	{
		varDestruct( retval );
	}

	jsp = (jsclass *)hashtableGetChar( qip->classes, "test", 4 );
	loc = (variant *)vectorElementAt( jsp->staticlocations, 0 );
	UNIT_ASSERT_MEM( "interp1", loc, sizeof( variant ) );
	UNIT_ASSERT( "interp1 loc==2", loc->data.i == 2 );

	qinterpDestruct( qip );
	logWriteOkFail( NULL );
}

static void factorialTest(  )
{
	qinterp *qip;
	program *pp;
	/*jsclass *jsp;*/
	enum InterpErrors err;
	/*variant *loc; */
	variant *retval = NULL;

	logWriteCheck(NULL, "interp factorial test ... " );

	/*
	 *  n(1)
	 */
	qip = qinterpConstruct();
	qinterpValidateMem( qip );
	pp = programConstruct();	/* destroyed by interp */
	programValidateMem( pp );
	programEmitStaticFactorial( pp, 1 );
	programValidateMem( pp );
	qinterpPrepare( qip, pp );
	qinterpValidateMem( qip );

	qinterpCheckMem( qip );
	UNIT_ASSERT_MEM_NOTED( "qinterp interp2" );

	err = qinterpRun( qip, "test", "main", 0, &retval );
	UNIT_ASSERT( "interp2", err == IERR_NONE );
	UNIT_ASSERT( "interp2", NULL != retval );
	UNIT_ASSERT( "interp2", TYPE_INT == retval->type );
	UNIT_ASSERT( "interp2", 1 == retval->data.i );
	if ( NULL != retval )
	{
		varDestruct( retval );
		retval = NULL;
	}
	qinterpDestruct( qip );

	/*
	 *  n(2)
	 */
	qip = qinterpConstruct();
	qinterpValidateMem( qip );
	pp = programConstruct();	/* destroyed by interp */
	programValidateMem( pp );
	programEmitStaticFactorial( pp, 2 );
	programValidateMem( pp );
	qinterpPrepare( qip, pp );
	qinterpValidateMem( qip );

	qinterpCheckMem( qip );
	UNIT_ASSERT_MEM_NOTED( "qinterp interp2" );

	err = qinterpRun( qip, "test", "main", 0, &retval );
	UNIT_ASSERT( "interp2", err == IERR_NONE );
	UNIT_ASSERT( "interp2", NULL != retval );
	UNIT_ASSERT( "interp2", TYPE_INT == retval->type );
	UNIT_ASSERT( "interp2", 2 == retval->data.i );
	if ( NULL != retval )
	{
		varDestruct( retval );
		retval = NULL;
	}
	qinterpDestruct( qip );

	/*
	 *  n(3)
	 */
	qip = qinterpConstruct();
	qinterpValidateMem( qip );
	pp = programConstruct();	/* destroyed by interp */
	programValidateMem( pp );
	programEmitStaticFactorial( pp, 3 );
	programValidateMem( pp );
	qinterpPrepare( qip, pp );
	qinterpValidateMem( qip );

	qinterpCheckMem( qip );
	UNIT_ASSERT_MEM_NOTED( "qinterp interp2" );

	err = qinterpRun( qip, "test", "main", 0, &retval );
	UNIT_ASSERT( "interp2", err == IERR_NONE );
	UNIT_ASSERT( "interp2", NULL != retval );
	UNIT_ASSERT( "interp2", TYPE_INT == retval->type );
	UNIT_ASSERT( "interp2", 6 == retval->data.i );
	if ( NULL != retval )
	{
		varDestruct( retval );
		retval = NULL;
	}
	qinterpDestruct( qip );
	
	logWriteOkFail( NULL );
}

static void simpleNew(  )
{
	qinterp *qip;
	program *pp;
	enum InterpErrors err;
	variant *retval = NULL;

	logWriteCheck(NULL, "interp simple new test ... " );

	qip = qinterpConstruct();
	qinterpValidateMem( qip );
	pp = programConstruct();	/* destroyed by interp */
	programValidateMem( pp );
	programEmitSimpleNew( pp, "test" );
	programValidateMem( pp );
	qinterpPrepare( qip, pp );
	qinterpValidateMem( qip );

	qinterpCheckMem( qip );
	UNIT_ASSERT_MEM_NOTED( "qinterp simplenew" );

	err = qinterpRun( qip, "test", "main", 0, &retval );
	UNIT_ASSERT( "interp1", err == IERR_NONE );
	UNIT_ASSERT( "qinterp simplenew",  NULL == retval );

	qinterpDestruct( qip );
	logWriteOkFail( NULL );
}

static void interpDiv(  )
{
	qinterp *qip;
	program *pp;
	enum InterpErrors err;
	variant *retval = NULL;

	logWriteCheck(NULL, "interp div test ... " );

	qip = qinterpConstruct();
	qinterpValidateMem( qip );
	pp = programConstruct();	/* destroyed by interp */

	programDefineClass( pp, "test", "Object", NULL );
	programDefineMethod( pp, "@@constructor", true, ACC_PRIVATE );
	programEmit0( pp, OP_VRET );
	jsmethodAddArg( programDefineMethod( pp, "@@constructor", false, ACC_PRIVATE ), "this", false );
	programEmit0( pp, OP_VRET );
	programDefineMethod( pp, "main", true, ACC_PUBLIC );

	programEmit1( pp, OP_PUSH, MODE_IM );
	programEmitArg( pp, 6 );
	programEmit1( pp, OP_PUSH, MODE_IM );
	programEmitArg( pp, 2 );
	programEmit0( pp, OP_DIV );
	programEmit0( pp, OP_RET );

	qinterpPrepare( qip, pp );
	qinterpValidateMem( qip );

	qinterpCheckMem( qip );
	UNIT_ASSERT_MEM_NOTED( "qinterp simplenew" );

	err = qinterpRun( qip, "test", "main", 0, &retval );
	UNIT_ASSERT( "interp def", err == IERR_NONE );
	UNIT_ASSERT( "interp div",  NULL != retval );
	UNIT_ASSERT( "interp div",  retval->type == TYPE_INT );
	UNIT_ASSERT( "interp div",  retval->data.i == 3 );
	
	varDestruct( retval );

	qinterpDestruct( qip );
	logWriteOkFail( NULL );
}

void runInterpTests(  )
{
	createDestroy(  );
	interp1(  );
	factorialTest(  );
	simpleNew(  );
	interpDiv(  );
}

#endif /*DEBUG */



