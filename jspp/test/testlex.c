#include <string.h>
#include <spl/debug.h>
#include <jspp/lex.h>

static void nullTest(  )
{
	jspplex *lex;
	
	logWriteCheck(NULL, "lex null test ... " );

	lex = jspplexConstructChar( "" );
	UNIT_ASSERT( "Lex NULL", jsppLex( lex ) == JEOF );
	jspplexDestruct( lex );

	logWriteOkFail( NULL );
}

static void emptyTest(  )
{
	jspplex *lex;
	
	logWriteCheck(NULL, "lex empty test ... " );

	lex = jspplexConstructChar( ";" );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == SEMI );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == JEOF );
	jspplexDestruct( lex );

	logWriteOkFail( NULL );
}

static void singlesTest(  )
{
	jspplex *lex;
	
	logWriteCheck(NULL, "lex singles test ... " );

	lex = jspplexConstructChar( "; ! % ^ & * ( ) - + = < > , . / : ~ | { } [ ]" );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == SEMI );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == BANG );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == MOD );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == XOR );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == AMPR );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == STAR );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == LPAR );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == RPAR );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == MINUS );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == PLUS );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == ASSIGN );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == LT );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == GT );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == COMMA );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == DOT );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == SLASH );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == COLON );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == COMP );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == PIPE );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == LBRACE );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == RBRACE );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == LBRAC );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == RBRAC );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == JEOF );
	jspplexDestruct( lex );

	logWriteOkFail( NULL );
}

static void singlesTest2(  )
{
	jspplex *lex;
	
	logWriteCheck(NULL, "lex singles test 2 ... " );

	lex = jspplexConstructChar( ";!%^&*()-+<>,./:~|{}[]" );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == SEMI );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == BANG );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == MOD );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == XOR );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == AMPR );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == STAR );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == LPAR );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == RPAR );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == MINUS );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == PLUS );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == LT );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == GT );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == COMMA );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == DOT );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == SLASH );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == COLON );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == COMP );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == PIPE );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == LBRACE );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == RBRACE );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == LBRAC );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == RBRAC );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == JEOF );
	jspplexDestruct( lex );

	logWriteOkFail( NULL );
}

static void keywordsTest(  )
{
	jspplex *lex;
	
	logWriteCheck(NULL, "lex keywords test ... " );

	lex = jspplexConstructChar( "finally try catch null const new delete switch case if else while for var return class public private protected break do continue" );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == FINALLY );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == TRY );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == CATCH );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == JNULL );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == CONST );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == NEW );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == DELETE );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == SWITCH );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == CASE );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == IF );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == ELSE );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == WHILE );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == FOR );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == VAR );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == RETURN );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == CLASS );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == PUBLIC );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == PRIVATE );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == PROTECTED );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == BREAK );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == DO );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == CONTINUE );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == JEOF );
	jspplexDestruct( lex );

	logWriteOkFail( NULL );
}

static void twoopTest(  )
{
	jspplex *lex;
	
	logWriteCheck(NULL, "lex two char op test ... " );

	lex = jspplexConstructChar( "~= != %= ^= &= *= /= -- ++ <= >=" );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == COMPEQ );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == ISNEQ );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == MODEQ );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == XOREQ );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == ANDEQ );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == TIMESEQ );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == DIVEQ );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == DEC );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == INC );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == LTEQ );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == GTEQ );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == JEOF );
	jspplexDestruct( lex );

	logWriteOkFail( NULL );
}

static void stringNullTest(  )
{
	jspplex *lex;
	
	logWriteCheck(NULL, "lex string null test ... " );

	lex = jspplexConstructChar( "\"\"" );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == STRING );
	UNIT_ASSERT( "Lex ;", strcmp(lex->lex, "") == 0 );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == JEOF );
	jspplexDestruct( lex );

	logWriteOkFail( NULL );
}

static void stringTest(  )
{
	jspplex *lex;
	
	logWriteCheck(NULL, "lex string test ... " );

	lex = jspplexConstructChar( "\"hi mom\"" );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == STRING );
	UNIT_ASSERT( "Lex ;", strcmp(lex->lex, "hi mom") == 0 );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == JEOF );
	jspplexDestruct( lex );

	logWriteOkFail( NULL );
}

static void charTest1(  )
{
	jspplex *lex;
	
	logWriteCheck(NULL, "lex char test ... " );

	lex = jspplexConstructChar( "\n'\\n'" );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == CHAR );
	UNIT_ASSERT( "lex", lex->lex[0] == '\n' );
	UNIT_ASSERT( "lex line", lex->lineno == 2 );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == JEOF );
	jspplexDestruct( lex );

	logWriteOkFail( NULL );
}

static void charTest2(  )
{
	jspplex *lex;
	
	logWriteCheck(NULL, "lex char 2 test ... " );

	lex = jspplexConstructChar( "\n'\\\\'" );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == CHAR );
	UNIT_ASSERT( "lex", lex->lex[0] == '\\' );
	UNIT_ASSERT( "lex line", lex->lineno == 2 );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == JEOF );
	jspplexDestruct( lex );

	logWriteOkFail( NULL );
}

static void idTest(  )
{
	jspplex *lex;
	
	logWriteCheck(NULL, "lex ID test ... " );

	lex = jspplexConstructChar( "_mybob my_bob mybob_ my1bob _123" );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == ID );
	UNIT_ASSERT( "lex id", strcmp(lex->lex, "_mybob") == 0 );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == ID );
	UNIT_ASSERT( "lex id", strcmp(lex->lex, "my_bob") == 0 );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == ID );
	UNIT_ASSERT( "lex id", strcmp(lex->lex, "mybob_") == 0 );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == ID );
	UNIT_ASSERT( "lex id", strcmp(lex->lex, "my1bob") == 0 );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == ID );
	UNIT_ASSERT( "lex id", strcmp(lex->lex, "_123") == 0 );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == JEOF );
	jspplexDestruct( lex );

	logWriteOkFail( NULL );
}

static void numTest(  )
{
	jspplex *lex;
	
	logWriteCheck(NULL, "lex num test ... " );

	lex = jspplexConstructChar( "1 1.0 .1 0x10 1.2e5 1.2e-5" );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == INT );
	UNIT_ASSERT( "lex id", strcmp(lex->lex, "1") == 0 );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == REAL );
	UNIT_ASSERT( "lex id", strcmp(lex->lex, "1.0") == 0 );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == REAL );
	UNIT_ASSERT( "lex id", strcmp(lex->lex, ".1") == 0 );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == HEX );
	UNIT_ASSERT( "lex id", strcmp(lex->lex, "0x10") == 0 );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == REAL );
	UNIT_ASSERT( "lex id", strcmp(lex->lex, "1.2e5") == 0 );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == REAL );
	UNIT_ASSERT( "lex id", strcmp(lex->lex, "1.2e-5") == 0 );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == JEOF );
	jspplexDestruct( lex );

	logWriteOkFail( NULL );
}

static void smallProgTest(  )
{
	jspplex *lex;
	
	logWriteCheck(NULL, "lex small prog test ... " );

	lex = jspplexConstructChar( "class test {\n\tpublic method()\n{var x = 1; return x;}\n \t}\n" );
	UNIT_ASSERT( "lex prog 1", jsppLex( lex ) == CLASS );
	UNIT_ASSERT( "lex prog 1", jsppLex( lex ) == ID );
	UNIT_ASSERT( "lex prog 1", jsppLex( lex ) == LBRACE );
	UNIT_ASSERT( "lex prog 1", jsppLex( lex ) == PUBLIC );
	UNIT_ASSERT( "lex prog 1", jsppLex( lex ) == ID );
	UNIT_ASSERT( "lex prog 1", jsppLex( lex ) == LPAR );
	UNIT_ASSERT( "lex prog 1", jsppLex( lex ) == RPAR );
	UNIT_ASSERT( "lex prog 1", jsppLex( lex ) == LBRACE );
	UNIT_ASSERT( "lex prog 1", jsppLex( lex ) == VAR );
	UNIT_ASSERT( "lex prog 1", jsppLex( lex ) == ID );
	UNIT_ASSERT( "lex prog 1", jsppLex( lex ) == ASSIGN );
	UNIT_ASSERT( "lex prog 1", jsppLex( lex ) == INT );
	UNIT_ASSERT( "lex prog 1", jsppLex( lex ) == SEMI );
	UNIT_ASSERT( "lex prog 1", jsppLex( lex ) == RETURN );
	UNIT_ASSERT( "lex prog 1", jsppLex( lex ) == ID );
	UNIT_ASSERT( "lex prog 1", jsppLex( lex ) == SEMI );
	UNIT_ASSERT( "lex prog 1", jsppLex( lex ) == RBRACE );
	UNIT_ASSERT( "lex prog 1", jsppLex( lex ) == RBRACE );
	UNIT_ASSERT( "Lex ;", jsppLex( lex ) == JEOF );
	jspplexDestruct( lex );

	logWriteOkFail( NULL );
}

void runLexTests(  )
{
	nullTest(  );
	emptyTest(  );
	singlesTest(  );
	singlesTest2(  );
	keywordsTest(  );
	twoopTest(  );
	stringNullTest(  );
	stringTest(  );
	charTest1(  );
	charTest2(  );
	idTest(  );
	numTest(  );
	smallProgTest(  );
}


