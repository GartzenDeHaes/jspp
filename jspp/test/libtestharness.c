#include <spl/debug.h>

#if defined(DEBUG) || defined(_DEBUG)

void desTest(  );
void desCompTest(  );
void httpReqTest(  );
void httpReqCompTest(  );

void runLibTests(  )
{
	//desTest( lp );
	desCompTest(  );
	//httpReqTest( lp );
	//httpReqCompTest( lp );
}

#endif
