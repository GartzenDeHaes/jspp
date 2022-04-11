#ifdef HAVE_CONFIG_H
#include <jspp/config.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <spl/debug.h>
#include <spl/Log.h>
#include <spl/net/csocket.h>
#include <jspp/program.h>
#include <jspp/interp.h>
#include <jspp/intrinsics.h>
#include <jspp/native.h>
#include <jspp/parse.h>

#ifdef DEBUG
#define USAGESTR "usage: jspp [-t][-v][-d dumpfilename][-?/h] filename\n"
#else
#define USAGESTR "usage: jspp [-v][-d dumpfilename][-?/h] filename\n"
#endif

extern bool stackPush( qinterp *qip, variant *var );

static void cleanup()
{
	socketShutdownService();
	DEBUG_DUMP_MEM_LEAKS();
	DEBUG_TEAR_DOWN(true);
	ASSERT_MEM_FREE();
}

static void parseArgs
( 
	int argc, 
	char **argv, 
	char **filename, 
	bool *dumpfile, 
	char **dumpfilename,
	vector **scriptArgs
)
{
	int argnum;
	*filename = NULL;
	*scriptArgs = NULL;

	for ( argnum = 1; argnum < argc; argnum++ )
	{
		if ( *filename == NULL && (argv[argnum][0] == '-' || argv[argnum][0] == '/') )
		{
			switch ( argv[argnum][1] )
			{
				case 'd':
					*dumpfile = true;
					if ( argnum == argc-1 )
					{
						printf( "filename is required for -d, disasmemble, option\n");
					}
					*dumpfilename = argv[++argnum];
					break;	
#ifdef DEBUG
				case 't':
					printf("running unit tests...\n");
					jsppRunUnitTests();
					ASSERT_MEM_FREE();
					DEBUG_TEAR_DOWN(true);
					exit( logMessageCount(NULL) );
#endif
				case '?':
				case 'h':
					printf( USAGESTR );
					break;

				case 'v':
					printf("jspp V0.1.0 %s\n", __DATE__ );
					DEBUG_TEAR_DOWN(true);
					exit( 0 );

				default:
					printf( "Unknown option %s\n", argv[argnum] );
					break;
			}
		}
		else if ( *filename == NULL )
		{
			*filename = argv[argnum];
		}
		else
		{
			// script arg
			variant *var;
			if ( NULL == *scriptArgs )
			{
				if ( NULL == (*scriptArgs = vectorConstruct( sizeof(variant *) )) )
				{
					printf("Out of memory\n");
					DEBUG_TEAR_DOWN(true);
					exit( 0 );
				}
			}
			var = varConstruct();
			if (NULL == var)
			{
				printf("Out of memory\n");
				DEBUG_TEAR_DOWN(true);
				exit( 0 );
			}
			vectorAdd( *scriptArgs, &var );
			varAssignChar( var, argv[argnum] );
		}
	}
}

static void delScriptArgs( vector *scriptArgs )
{
	int x;
	variant *var;
	int count;
	if ( NULL == scriptArgs )
	{
		return;
	}
	ASSERT_MEM( scriptArgs, sizeof(vector) );
	vectorValidateMem( scriptArgs );
	count = vectorCount(scriptArgs);
	for( x = 0; x < count; x++ )
	{
		var = (variant *)vectorElementAt( scriptArgs, x );
		ASSERT_MEM( var, sizeof(variant) );
		varDestruct( var );
	}
	vectorDestruct( scriptArgs );
}

static void argsCheckMem( vector *scriptArgs )
{
	int x;
	variant *var;
	int count;
	if ( NULL == scriptArgs )
	{
		return;
	}
	DEBUG_NOTE_MEM( scriptArgs );
	vectorCheckMem( scriptArgs );
	count = vectorCount(scriptArgs);
	for( x = 0; x < count; x++ )
	{
		var = (variant *)vectorElementAt( scriptArgs, x );
		DEBUG_NOTE_MEM( var );
		varCheckMem( var );
	}
}

int main(int argc, char *argv[])
{
	qinterp *qip;
	program *pp;
	string *strmain;
	string *msig;
	jsclass *jsp = NULL;
	enum InterpErrors err;
	variant *retval = NULL;
	bool prepareOk;

	char *filename = NULL;
	bool dumpfile = false;
	char *dumpfilename = NULL;
	vector *scriptArgs = NULL;
	int argcount = 0;

	parseArgs( argc, argv, &filename, &dumpfile, &dumpfilename, &scriptArgs );
	if ( NULL == filename )
	{
		delScriptArgs(scriptArgs);
		printf( USAGESTR );
		cleanup();
		exit(0);
	}
	if ( NULL == (qip = qinterpConstruct()) )
	{
		delScriptArgs(scriptArgs);
		printf("Error constructing interpreter\n");
		cleanup();
		exit(20);
	}
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	argsCheckMem( scriptArgs );
	qinterpCheckMem( qip );
	DEBUG_DUMP_MEM_LEAKS();
	DEBUG_ASSERT_MEM_NOTED( );
	pp = parseFile( filename, qinterpGetPrograms(qip) /*qinterpGetIntrinsics(qip)*/ );	
	if ( NULL == pp )
	{
		delScriptArgs(scriptArgs);
		printf( "%s not found\n", filename );
		qinterpDestruct( qip );
		cleanup();
		exit( 0 );
	}
	else if ( 0 != pp->errornum )
	{
		if ( NULL == pp->errmsg )
		{
			printf("Error number %d on line %d\n", pp->errornum, pp->errorline);
		}
		else
		{
			printf("%s", pp->errmsg->cstr );
		}
		delScriptArgs(scriptArgs);
		programDestruct( pp );
		qinterpDestruct( qip );
		cleanup();
		exit( 20 );
	}
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	argsCheckMem( scriptArgs );
	programCheckMem( pp );
	qinterpCheckMem( qip );
	DEBUG_DUMP_MEM_LEAKS();
	DEBUG_ASSERT_MEM_NOTED( );

	if ( dumpfile )
	{
		delScriptArgs(scriptArgs);
		ASSERT( NULL != dumpfilename );
		programDump( pp, dumpfilename );
		programDestruct( pp );
		qinterpDestruct( qip );
		cleanup();
		printf( "dump complete.\n" );
		exit( 0 );
	}
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	argsCheckMem( scriptArgs );
	programCheckMem( pp );
	qinterpCheckMem( qip );
	DEBUG_DUMP_MEM_LEAKS();
	DEBUG_ASSERT_MEM_NOTED( );

	prepareOk = qinterpPrepare( qip, pp );
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	argsCheckMem( scriptArgs );
	qinterpCheckMem( qip );
	DEBUG_DUMP_MEM_LEAKS();
	DEBUG_ASSERT_MEM_NOTED( );

	if ( ! prepareOk )
	{
		delScriptArgs(scriptArgs);
		printf( "Internal error preparing to execute\n" );
		qinterpCheckMem( qip );
		DEBUG_ASSERT_MEM_NOTED( );
		qinterpDestruct( qip );
		cleanup();
		exit( 20 );
	}
	if ( NULL != scriptArgs )
	{
		strmain = stringConstructChar( "main" );
		msig = buildMethodSig( strmain, true );
		methodSigAppendArg( msig );
		jsp = programFindClassWithMethod( pp, msig->cstr, true );
		stringDestruct( msig );
		stringDestruct( strmain );

		DEBUG_CLEAR_MEM_CHECK_POINTS();
		argsCheckMem( scriptArgs );
		qinterpCheckMem( qip );
		DEBUG_DUMP_MEM_LEAKS();
		DEBUG_ASSERT_MEM_NOTED( );

		if ( NULL != jsp )
		{
			variant *var;
			int count = vectorCount(scriptArgs);
			int x;
			enum InterpErrors err;
			jsinst *inst;
			string *classname = stringConstructChar( "Array" );
			ASSERT( NULL != classname );

			err = createJSClassInstance( qip, classname, &inst, 0 );
			ASSERT( err == IERR_NONE );
			DEBUG_CLEAR_MEM_CHECK_POINTS();
			argsCheckMem( scriptArgs );
			qinterpCheckMem( qip );
			DEBUG_NOTE_MEM(classname);
			stringCheckMem(classname);
			DEBUG_DUMP_MEM_LEAKS();
			DEBUG_ASSERT_MEM_NOTED( );

			err = arrayHelperConstruct( inst, 1, count );
			ASSERT( err == IERR_NONE );

			for ( x = 0; x < count; x++ )
			{
				var = (variant *)vectorElementAt( scriptArgs, x );
				err = arrayHelperSet( inst, var, 1, x );
				ASSERT( err == IERR_NONE );
			}
			argcount = 1;
			var = varConstruct();
			varAssignInst( var, inst );
			stackPush(qip, var);
			var->type = TYPE_VOID;
			varDestruct( var );

			jsinstDeref(inst);
			delScriptArgs( scriptArgs );
			stringDestruct( classname );

			DEBUG_CLEAR_MEM_CHECK_POINTS();
			qinterpCheckMem( qip );
			DEBUG_DUMP_MEM_LEAKS();
			DEBUG_ASSERT_MEM_NOTED( );
		}
	}
	if ( NULL == jsp )
	{
		delScriptArgs(scriptArgs);

		strmain = stringConstructChar( "main" );
		msig = buildMethodSig( strmain, true );
		jsp = programFindClassWithMethod( pp, msig->cstr, true );
		stringDestruct( msig );
		stringDestruct( strmain );

		if ( NULL == jsp )
		{
			printf( "main method not found\n" );
			err = IERR_MAIN_CLASS_NOT_FOUND;
			qinterpDestruct( qip );
			cleanup();
			return err;
		}
	}
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	qinterpCheckMem( qip );
	DEBUG_DUMP_MEM_LEAKS();
	DEBUG_ASSERT_MEM_NOTED( );
	err = qinterpRun( qip, jsp->name->cstr, "main", argcount, &retval );
	if ( err != IERR_NONE )
	{
		char buf[256];
		printf( "Runtime error on line %d: %s\n", qip->linenum, qinterpGetErrorMessage(err, buf, 256) );
	}
	if ( NULL != retval )
	{
		string *str = varToString( retval );
		printf( "%s\n", str->cstr );
		stringDestruct( str );
		varDestruct( retval );
	}
	DEBUG_CLEAR_MEM_CHECK_POINTS();
	qinterpCheckMem( qip );
	DEBUG_DUMP_MEM_LEAKS();
	DEBUG_ASSERT_MEM_NOTED( );
	qinterpDestruct( qip );

	DEBUG_CLEAR_MEM_CHECK_POINTS();
	DEBUG_DUMP_MEM_LEAKS();
	DEBUG_ASSERT_MEM_NOTED( );
	cleanup();

	return err;
}
