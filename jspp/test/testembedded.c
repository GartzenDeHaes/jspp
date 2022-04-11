#include <string.h>
#include <spl/debug.h>
#include <jspp/parse.h>
#include <jspp/interp.h>
#include <jspp/native.h>

void testembedded(  )
{
	qinterp *qip;
	program *pp;
	enum InterpErrors err;
	variant *retval = NULL, *arg1;
	jsinst *inst;
	string *classname;
	bool prepareOk;

	logWriteCheck( NULL, "embedded test 1 ... " );

	classname = stringConstructChar( "test" );
	qip = qinterpConstruct();
	pp = parseChar( "class test {public var x = 3; public var y = 2; public getX(){return x;} public getY() {return y;} public setX( _x ) {x = _x;} public setY( _y ) {y = _y;} }", NULL );

	DEBUG_CLEAR_MEM_CHECK_POINTS();
	stringCheckMem( classname );
	programCheckMem( pp );
	qinterpCheckMem( qip );
	DEBUG_DUMP_MEM_LEAKS();
	UNIT_ASSERT_MEM_NOTED( "test embedded mem leak" );

	UNIT_ASSERT( "testembedded parse", pp->errornum == 0 );

	prepareOk = qinterpPrepare( qip, pp );
	UNIT_ASSERT( "testembedded prepare", prepareOk );

	DEBUG_CLEAR_MEM_CHECK_POINTS();
	stringCheckMem( classname );
	qinterpCheckMem( qip );
	DEBUG_DUMP_MEM_LEAKS();
	UNIT_ASSERT_MEM_NOTED( "test embedded mem leak" );

	err = createJSClassInstance( qip, classname, &inst, 0 );
	UNIT_ASSERT( "testembedded create inst error", err == IERR_NONE );
	UNIT_ASSERT( "instance ref count", inst->refcount == 1 );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	jsinstCheckMem( inst );
	stringCheckMem( classname );
	qinterpCheckMem( qip );
	DEBUG_DUMP_MEM_LEAKS();
	UNIT_ASSERT_MEM_NOTED( "test embedded mem leak" );

	err = qinterpCall( qip, inst, "getX", &retval, 0 );
	UNIT_ASSERT( "instance ref count", inst->refcount == 1 );
	UNIT_ASSERT( "testembedded interp error", err == IERR_NONE );
	UNIT_ASSERT( "testembedded return value not NULL", NULL != retval );
	UNIT_ASSERT( "testembedded return value", retval->type == TYPE_INT && retval->data.i == 3 );
	varDestruct( retval );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	jsinstCheckMem( inst );
	stringCheckMem( classname );
	qinterpCheckMem( qip );
	UNIT_ASSERT_MEM_NOTED( "test embedded mem leak 3" );

	err = qinterpCall( qip, inst, "getY", &retval, 0 );
	UNIT_ASSERT( "testembedded interp error", err == IERR_NONE );
	UNIT_ASSERT( "testembedded return value not NULL", NULL != retval );
	UNIT_ASSERT( "testembedded return value", retval->type == TYPE_INT && retval->data.i == 2 );
	varDestruct( retval );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	jsinstCheckMem( inst );
	stringCheckMem( classname );
	qinterpCheckMem( qip );
	UNIT_ASSERT_MEM_NOTED( "test embedded mem leak 4" );

	arg1 = varConstruct();
	varAssignInt( arg1, 99 );
	err = qinterpCall( qip, inst, "setY", &retval, 1, arg1 );
	UNIT_ASSERT( "testembedded interp error", err == IERR_NONE );
	err = qinterpCall( qip, inst, "getY", &retval, 0 );
	UNIT_ASSERT( "testembedded interp error", err == IERR_NONE );
	UNIT_ASSERT( "testembedded return value not NULL", NULL != retval );
	UNIT_ASSERT( "testembedded return value", retval->type == TYPE_INT && retval->data.i == 99 );
	varDestruct( retval );
	varDestruct( arg1 );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	jsinstCheckMem( inst );
	stringCheckMem( classname );
	qinterpCheckMem( qip );
	UNIT_ASSERT_MEM_NOTED( "test embedded mem leak 5" );

	qinterpDestruct( qip );
	jsinstDestruct( inst );
	stringDestruct( classname );

	UNIT_ASSERT_MEM_NOTED( "test embedded mem leak 2" );

	logWriteOkFail( NULL );
}
