
#include <spl/debug.h>

#ifdef DEBUG

void runSupportUnitTests(  );
void runLexTests(  );
void testparse(  );
void runInterpTests(  );
void runProgramTests(  );
void runVariantTests(  );
void testcompile(  );
void testintrins(  );
void testembedded(  );
void runLibTests(  );

void jsppRunUnitTests()
{
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "unit test 0" );

	runSupportUnitTests(  );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "unit test 1" );

	DEBUG_VALIDATE();

	runLexTests(  );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "unit test 2" );

	testparse(  );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "unit test 3" );

	runVariantTests(  );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "unit test 4" );

	runProgramTests(  );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "unit test 5" );

	testintrins(  );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "unit test 7" );

	runInterpTests(  );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "unit test 6" );

	runLibTests(  );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "runLibTests" );

	DEBUG_VALIDATE();

	testcompile(  );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "unit test 7" );

	testembedded(  );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	UNIT_ASSERT_MEM_NOTED( "unit test 8" );

	DEBUG_VALIDATE();
	logWriteEndOfRunTotal();
}

#endif //DEBUG

