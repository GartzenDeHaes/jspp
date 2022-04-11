Java Script++ (JSPP)

JSPP is Java/JavaScript like scripting language designed for use in
other applications.  It's unique selling points are:

- It's like Java with variants, or JavaScript with (real) classes.

- Extensive unit tests

Running From the Command Line

	The command line interface is only designed for testing.  The 
	syntax is:	jspp ([-t] | [-d filename] filename).
	
		To disassemble a file:	jspp -d outfilename infilename
		To run a program:		jspp filename
		To run the unit tests:	jspp -t

		NOTE: unit tests will not correctly report errors in 
		release builds.

Embedding

	See "mainishere.c" and "testcompile.c" for details.  Here's a 
	short example:
	
	
		#include "program.h"
		#include "interp.h"
		#include "parse.h"
		
		.
		.
		.
		
		qinterp *qip;
		program *pp;
		enum InterpErrors err;
		variant *retval = NULL;
		bool prepareOk;
		
		qip = qinterpConstruct();
		pp = parseChar( "class test {public static main(){return 3*2;}}", qinterpGetIntrinsics(qip) );
		if ( NULL == pp )
		{
			printf( "Internal error\n" );
			qinterpDestruct( qip );
			exit(20);
		}
		prepareOk = qinterpPrepare( qip, pp );
		if ( ! prepareOk )
		{
			printf("Error number %d on line %d, %s\n", pp->errornum, pp->errorline);
			qinterpDestruct( qip );
			exit(20);
		}
		err = qinterpRun( qip, "test", "main", &retval );
		if ( err != IERR_NONE )
		{
			printf( "Runtime error %d\n", err );
		}
		qinterpDestruct( qip );


