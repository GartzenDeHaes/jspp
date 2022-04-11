#include <string.h>
#include <spl/debug.h>
#include <jspp/variant.h>

#ifdef DEBUG

static void varCreateDestructTest(  )
{
	variant *var;
	
	logWriteCheck(NULL, "variant create/destroy test ... " );

	UNIT_ASSERT_MEM_NOTED( "var createDestroy NULL" );

	DEBUG_CHECK_POINT_HEAP();

	var = varConstruct();
	varCheckMem( var );
	UNIT_ASSERT_MEM_NOTED( "var createDestroy alloced" );

	varDestruct( var );

	UNIT_ASSERT_CHECK_POINT( "var createDestroy" );
	logWriteOkFail( NULL );
}

static void varCastTest(  )
{
	variant *var;

	logWriteCheck(NULL, "variant cast test ... " );

	DEBUG_CHECK_POINT_HEAP();

	var = varConstruct();
	varAssignInt( var, 1 );
	varCast( var, TYPE_STRING );
	UNIT_ASSERT( "var convert", var->type == TYPE_STRING );
	UNIT_ASSERT( "var convert", strcmp(var->data.sp->cstr, "1") == 0 );

	varDestruct( var );
	UNIT_ASSERT_CHECK_POINT( "var convert" );
	logWriteOkFail( NULL );
}

void runVariantTests(  )
{
	varCreateDestructTest(  );
	varCastTest(  );
}

#endif /*DEBUG*/

