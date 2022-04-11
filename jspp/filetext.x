#include "debug.h"
#include "file.h"
#include "log.h"
static void teststream( log *l )
{
	string *str;
	
	logWriteCheck(l, "file stream test ... " );

	DEBUG_CLEAR_MEM_CHECK_POINTS();
	logCheckMem( l );
	UNIT_ASSERT_MEM_NOTED("teststream 2", l);

	str = fileLoadText( "blarb.x" );

	UNIT_ASSERT( "fileload 1", str == NULL, l );
	str = fileLoadText( "testfile.c" );

	DEBUG_CLEAR_MEM_CHECK_POINTS();
	stringCheckMem( str );
	logCheckMem( l );
	UNIT_ASSERT_MEM_NOTED("teststream 2", l);

	UNIT_ASSERT( "fileload 2", str != NULL, l );
	UNIT_ASSERT( "fileload 3", stringStartsWith( str, "#include "debug.h" ) != 0, l );
	stringDestruct( str );

	logWriteOkFail( l );
}
	void testfile( log *l )
	{
	teststream( l );
	}
