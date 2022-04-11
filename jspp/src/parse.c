#ifdef HAVE_CONFIG_H
#include <jspp/config.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <spl/debug.h>
#include <jspp/parse.h>
#include <jspp/lex.h>
#include <jspp/parse.h>
#include <spl/cstring.h>
#include <jspp/parseerrors.h>

extern vardef *createVar( const char *name, const bool isstatic, const enum ArgSource, const bool isptr );
extern void deleteVar( const vardef *vdp );

static void stmtblock( jspplex *lex, program *pp );
static void localvar( jspplex *lex, program *pp );
static void expr( jspplex *lex, program *pp );
static void factor( jspplex *lex, program *pp );
static void term( jspplex *lex, program *pp );
static void stmts( jspplex *lex, program *pp );
static void new_expr( jspplex *lex, program *pp );


static vardef *symtabLookup( program *pp, const char *id )
{
	int x;
	symtab *stp;
	vardef *vdp;
	int len = (int)strlen(id);

	for ( x = vectorCount( pp->symstack )-1; x >= 0 ; x-- )
	{
		stp = (symtab *)vectorElementAt( pp->symstack, x );
		vdp = (vardef *)hashtableGetChar( stp->syms, id, len );
		if ( NULL != vdp )
		{
			ASSERT_MEM( vdp, sizeof(struct vardef) );
			return vdp;
		}
	}
	/* try the class */
	vdp = jsclassFindField( pp->curclass, id );
	return vdp;
}

static bool symtabEnter( program *pp, const int patchupPc )
{
	symtab *stp;
	stp = (symtab *)malloc( sizeof(struct symtab) );
	if ( NULL == stp )
	{
		return false;
	}
	stp->symlist = vectorConstruct( sizeof(vardef *) );
	if ( NULL == stp->symlist )
	{
		free( stp );
		return false;
	}
	stp->syms = hashtableConstruct();
	if ( NULL == stp->syms )
	{
		vectorDestruct( stp->symlist );
		free( stp );
		return false;
	}
	if ( vectorPeek( pp->symstack ) == NULL )
	{
		stp->cumulative = 0;
	}
	else
	{
		stp->cumulative = ((symtab *)vectorPeek( pp->symstack ))->cumulative;
	}
	stp->count = 0;
	stp->patchupPc = patchupPc;
	return vectorAdd( pp->symstack, &stp );
}

static int symtabExit( program *pp )
{
	int x;
	int count;
	int veccount;
	symtab *stp = vectorPop( pp->symstack );

	ASSERT_MEM( stp, sizeof(struct symtab) );
	ASSERT( stp->patchupPc != -1 );
	programEmitArgAt( pp, stp->patchupPc, stp->count );
	count = stp->count;
	veccount = vectorCount( stp->symlist );
	for ( x = 0; x < veccount; x++ )
	{
		vardef *vdp = (vardef *)vectorElementAt( stp->symlist, x );
		deleteVar( vdp );
	}

	hashtableDestruct( stp->syms );
	vectorDestruct( stp->symlist );
	free( stp );

	return count;
}

/*static int symtabLocalCount( program *pp )
{
	symtab *stp = vectorPeek( pp->symstack );
	ASSERT_MEM( stp, sizeof(struct symtab) );
	return stp->count;
}*/

static vardef *symtabNote( program *pp, const char *name, const bool isptr )
{
	symtab *stp = vectorPeek( pp->symstack );
	vardef *vdp = createVar( name, false, MODE_BLOCK, isptr );
	if ( NULL == vdp )
	{
		return NULL;
	}
	vdp->isauto = true;
	vdp->offset = stp->cumulative;
	stp->cumulative++;
	stp->count++;
	vectorAdd( stp->symlist, &vdp );
	hashtableAdd( stp->syms, vdp->name, vdp );
	
	return vdp;
}

static void setupError( const jspplex *lex, program *ilp, const int parseerror )
{
	char buf[512];
	char *msg;

	ilp->errorline = lex->lineno;

	if ( lex->token == LEXERROR )
	{
		ilp->errornum = atoi( lex->lex );
		switch ( ilp->errornum )
		{
		case LEX_INVALID_NUMBER_FORMAT:
			msg = "invalid number format";
			break;
		case LEX_UNEXPECTED_ESCAPE:
			msg = "unexpected escape sequence";
			break;
		case LEX_UNTERMINATED_CHAR:
			msg = "unterminated char";
			break;
		case LEX_UNTERMINATED_STRING:
			msg = "unterminated string";
			break;
		case LEX_ERROR:
			msg = "internal error";
			break;
		default:
			msg = "unknown error";
			break;
		}
		sprintf(buf, "ERROR in lexical analysis of '%s' on line %d\n%s\n", lex->lex, ilp->errorline, msg );
	}
	else
	{
		ilp->errornum = parseerror;
		switch ( ilp->errornum )
		{
		case YYER_EXPECTED_CLASS_DEF:
			msg = "expected class definition";
			break;
		case YYER_EXPECTED_CLASS_NAME:
			msg = "expected class name";
			break;
		case YYER_OUT_OF_MEM:
			msg = "out of memory";
			break;
		case YYER_EXPECTED_SUPER_CLASS:
			msg = "expected base class name";
			break;
		case YYER_EXPECTED_LBRACE:
			msg = "expected '{'";
			break;
		case YYER_EXPECTED_RBRACE:
			msg = "expected '}'";
			break;
		case YYER_EXPECTED_METHOD_NAME:
			msg = "expected method name";
			break;
		case YYER_EXPECTED_LPAR:
			msg = "expected '('";
			break;
		case YYER_EXPECTED_RPAR:
			msg = "expected ')'";
			break;
		case YYER_EXPECTED_VAR_NAME:
			msg = "expected variable name";
			break;
		case YYER_EXPECTED_SEMI:
			msg = "expected semi colon";
			break;
		case YYER_ASSIGN_SYNTAX:
			msg = "assignment syntax error";
			break;
		case YYER_NEW_SYNTAX:
			msg = "'new' operator syntax error";
			break;
		case YYER_EXPECTED_ARRAY_INDEX:
			msg = "expected array index";
			break;
		case YYER_EXPECTED_RBRAC:
			msg = "expected ']'";
			break;
		case YYER_EXPECTED_COMMA:
			msg = "expected ','";
			break;
		case YYERR_SYNTAX:
			msg = "syntax error";
			break;
		case YYER_EXPECTED_ID:
			msg = "expected identifier";
			break;
		case YYER_EXPECTED_COLON:
			msg = "expected ':'";
			break;
		case YYER_EXPECTED_CATCH:
			msg = "expected 'catch'";
			break;
		case YYER_UNKNOWN_CLASS:
			msg = "undefined class name";
			break;
		case YYER_EXPECTED_SCOPE_OP:
			msg = "expected '::'";
			break;
		case YYER_EXPECTED_ACCESS:
			msg = "expected public, protected, or private";
			break;
		case YYER_UNKNOWN_METHOD:
			msg = "undefined method name";
			break;
		case YYER_UNKNOWN_ID:
			msg = "undefined indentifier";
			break;
		case YYER_ILLEGAL_STATIC_REF:
			msg = "illegal static reference";
			break;
		case YYER_INVALID_ARRAY_INDEX:
			msg = "invalid array index";
			break;
		case YYER_INST_REF_IN_STATIC:
			msg = "instance method cannot be called from static method";
			break;
		case YYER_EXPECTED_VAR_OR_CLASS:
			msg = "expected var or class";
			break;
		case YYER_ILLEGAL_CASE_VALUE:
			msg = "illegal case value";
			break;
		case YYER_ILLEGAL_ARRAY_INIT:
			msg = "illegal array initializer";
			break;
		case YYER_METH_RETURNS_VAL:
			msg = "Method must return a value";
			break;
		default:
			msg = "unknown error";
			break;
		}
		sprintf(buf, "ERROR parsing of '%s' on line %d\n%s\n", lex->lex, ilp->errorline, msg );
	}
	ilp->errmsg = stringConstructChar( buf );
}

static void delop( jspplex *lex, program *ilp )
{
	/*string *idname = stringConstructChar( lex->lex );
	if ( NULL == idname )
	{
		setupError( lex, ilp, YYER_OUT_OF_MEM );
		return;
	}*/

	if ( ID != lex->token )
	{
		setupError( lex, ilp, YYER_EXPECTED_ID );
		return;
	}
	jsppLex( lex );
}

/*
arglist		: literal [COMMA arglist]-
			| ID [COMMA arglist]-
			;
 */
static int arglist_call
( 
	jspplex *lex, 
	program *pp, 
	jsclass *jsp,
	const string *methname,
	const bool basecall,
	const bool constructor
)
{
	string *sig;
	int argcount = 0;

	ASSERT( LPAR != lex->token );
	
	if ( NULL == jsp )
	{
		/*
		* instance call -- this is on the stack
		*/

		sig = stringConstructDup( methname );
		if ( NULL == sig )
		{
			setupError( lex, pp, YYER_OUT_OF_MEM );
			return -1;
		}
		if ( ! stringAppendChar(sig, ":Ivv" ) )
		{
			setupError( lex, pp, YYER_OUT_OF_MEM );
			return -1;
		}
	}
	else
	{
		/* static call */
		jsclassValidateMem( jsp );

		sig = stringConstructDup( jsp->name );
		if ( NULL == sig )
		{
			setupError( lex, pp, YYER_OUT_OF_MEM );
			return -1;
		}
		if ( ! stringAppendChar( sig, "." ) )
		{
			setupError( lex, pp, YYER_OUT_OF_MEM );
			return -1;
		}
		if ( ! stringAppend( sig, methname ) )
		{
			setupError( lex, pp, YYER_OUT_OF_MEM );
			return -1;
		}
		if ( ! stringAppendChar(sig, ":Sv" ) )
		{
			setupError( lex, pp, YYER_OUT_OF_MEM );
			return -1;
		}
	}
	while ( RPAR != lex->token )
	{
		argcount++;
		if ( ! constructor )
		{
			stringAppendChar( sig, "v" );
		}
		expr( lex, pp );
		
		if ( COMMA != lex->token && lex->token != RPAR )
		{
			setupError( lex, pp, YYER_EXPECTED_COMMA );
			return -1;
		}
		if ( COMMA == lex->token )
		{
			jsppLex( lex );
		}
	}
	if ( RPAR != lex->token )
	{
		setupError( lex, pp, YYER_EXPECTED_RPAR );
		return -1;
	}
	if ( NULL == jsp )
	{
		if ( basecall )
		{
			programEmit2( pp, OP_BASECALL, MODE_CONST, MODE_IM );
		}
		else if ( constructor )
		{
			programEmit2( pp, OP_CONSTRUCT, MODE_CONST, MODE_IM );
		}
		else
		{
			programEmit2( pp, OP_CALL, MODE_CONST, MODE_IM );
		}
	}
	else
	{
		ASSERT( false == basecall );
		programEmit1( pp, OP_SCALL, MODE_CONST );
	}
	programEmitArg( pp, programEmitConstChars( pp, sig->cstr ) );
	if ( NULL == jsp )
	{
		programEmitArg( pp, argcount );
	}
	stringDestruct( sig );

	return argcount;
}

static void deref( jspplex *lex, program *pp );

/*
objderef	: ID deref							// class instance field 
			| ID ASSIGN expr
			| ID LPAR arglist_call RPAR deref	// out-of-class method call
			;
 */
static void objderef( jspplex *lex, program *pp )
{
	string *idname;

	if ( ID != lex->token )
	{
		setupError( lex, pp, YYER_EXPECTED_ID );
		return;
	}

	idname = stringConstructChar( lex->lex );
	if ( NULL == idname )
	{
		setupError( lex, pp, YYER_OUT_OF_MEM );
		return;
	}
	if ( ID == lex->token )
	{
		jsppLex( lex );
	}
	else
	{
		setupError( lex, pp, YYER_EXPECTED_ID );
		return;
	}
	if ( LPAR == lex->token )
	{
		/* method call */
		jsppLex(lex);

		arglist_call( lex, pp, NULL, idname, false, false );
		
		if ( RPAR != lex->token )
		{
			setupError( lex, pp, YYER_EXPECTED_RPAR );
			return;
		}
		jsppLex( lex );
		deref( lex, pp );
	}
	else
	{
		/* field */
		programEmit1( pp, OP_DEREF, MODE_CONST );
		programEmitArg( pp, programEmitConstChars( pp, idname->cstr ) );

		if ( ASSIGN == lex->token )
		{
			jsppLex( lex );
			pp->isrhs = true;
			expr( lex, pp );
			programEmit0( pp, OP_RVALUE );
			programEmit0( pp, OP_ASSIGN );
			pp->isrhs = false;
		}
		else
		{
			deref( lex, pp );
		}
	}
	stringDestruct( idname );
}

static void arrayIndexCall( jspplex *lex, program *pp )
{
	int argcount = 0;
	string *sig;

	jsppLex( lex );
	sig = stringConstructChar( "[]:Ivv" );
	if ( NULL == sig )
	{
		setupError( lex, pp, YYER_OUT_OF_MEM );
		return;
	}
	while ( RBRAC != lex->token && 0 == pp->errornum )
	{
		argcount++;
		stringAppendChar( sig, "v" );

		/*if ( ID != lex->token && INT != lex->token )
		{
			stringDestruct( sig );
			setupError( lex, pp, YYER_INVALID_ARRAY_INDEX );
			return;
		}*/
		expr( lex, pp );

		if ( COMMA == lex->token )
		{
			jsppLex( lex );
		}
	}
	if ( RBRAC != lex->token )
	{
		stringDestruct( sig );
		setupError( lex, pp, YYER_EXPECTED_RBRAC );
		return;
	}
	jsppLex( lex );

	programEmit2( pp, OP_CALL, MODE_CONST, MODE_IM );
	programEmitArg( pp, programEmitConstChars( pp, sig->cstr ) );
	programEmitArg( pp, argcount );

	stringDestruct( sig );
}

/*
deref		: DOT objref
			|									
			;
*/
static void deref( jspplex *lex, program *pp )
{
	if ( DOT == lex->token )
	{
		jsppLex( lex );

		if ( ID != lex->token )
		{
			setupError( lex, pp, YYER_EXPECTED_ID );
			return;
		}
		objderef( lex, pp );
	}
	else if ( LBRAC == lex->token )
	{
		arrayIndexCall( lex, pp );
	}
}

static void singletonCallHandler( jspplex *lex, program *pp )
{
	/*
	 *  id is a (local) method or class name
	 */
	string *idname = stringConstructChar( lex->lex );
	if ( NULL == idname )
	{
		setupError( lex, pp, YYER_OUT_OF_MEM );
		return;
	}
	jsppLex( lex );
	if ( SCOPE == lex->token )
	{
		/*
		 *  Static reference or call
		 */
		jsclass *jsp;
		jsp = programFindClass( pp, idname->cstr );
		if ( NULL == jsp )
		{/*
			if ( NULL != pp->intrinsics )
			{
				jsp = programFindClass( pp->intrinsics, idname->cstr );
				if ( NULL == jsp )
				{
					stringDestruct( idname );
					setupError( lex, pp, YYER_UNKNOWN_CLASS );
					return;
				}
			}
			else
			{*/
				stringDestruct( idname );
				setupError( lex, pp, YYER_UNKNOWN_CLASS );
				return;
			/*}*/
		}
		stringDestruct( idname );
		jsppLex( lex );
		if ( ID != lex->token )
		{
			setupError( lex, pp, YYER_EXPECTED_ID );
			return;
		}
		idname = stringConstructChar( lex->lex );
		jsppLex( lex );
		if ( LPAR == lex->token )
		{
			/*
			 *  Static method call
			 */
			jsppLex( lex );
			arglist_call( lex, pp, jsp, idname, false, false );
			if ( RPAR != lex->token )
			{
				setupError( lex, pp, YYER_EXPECTED_RPAR );
				return;
			}
			jsppLex( lex );
		}
		else
		{
			/*
			 *  Static field ref
			 */
			vardef *vdp;

			programEmit2( pp, OP_SDEREF, MODE_CONST, MODE_IM );
			programEmitArg( pp, programEmitConstChars( pp, jsp->name->cstr ) );
			vdp = jsclassFindField( jsp, idname->cstr );
			if ( NULL == vdp )
			{
				setupError( lex, pp, YYER_UNKNOWN_ID );
				return;
			}
			programEmitArg( pp, vdp->offset );
		}
	}
	else if ( LPAR == lex->token )
	{
		/*
		 *  In this case, it's a call to a method in the current class.
		 *  We need to push THIS on the stack.
		 */
		programEmit0( pp, OP_THIS );
		jsppLex(lex);

		arglist_call( lex, pp, NULL, idname, false, false );
		
		if ( RPAR != lex->token )
		{
			setupError( lex, pp, YYER_EXPECTED_RPAR );
			return;
		}
		jsppLex( lex );
	}
	else
	{
		stringDestruct( idname );
		setupError( lex, pp, YYER_UNKNOWN_ID );
		return;
	}
	stringDestruct( idname );
	deref( lex, pp );
	return;
}

static void singletonVarHandler( jspplex *lex, program *pp, const vardef *vdp )
{
	/*
	 *  id is a variable
	 */
	jsppLex( lex );
	if ( pp->curmethod->isstatic && !vdp->isstatic && !vdp->isauto )
	{
		setupError( lex, pp, YYER_INST_REF_IN_STATIC );
		return;
	}
	/*
	 *  Put the variable on the stack
	 */
	programEmit1( pp, OP_LVALUE, vdp->srcmode );
	programEmitArg( pp, vdp->offset );

	if ( LBRAC == lex->token )
	{
		/*
		 *  Array reference
		 */
		arrayIndexCall( lex, pp );
	}
	if ( ASSIGN == lex->token )
	{
		jsppLex( lex );
		pp->isrhs = true;
		expr( lex, pp );
		programEmit0( pp, OP_RVALUE );
		programEmit0( pp, OP_ASSIGN );
		pp->isrhs = false;
	}
	deref( lex, pp );
}

static void arraydef( jspplex *lex, program *pp )
{
	jsclass *jsp = programFindClass( pp/*->intrinsics*/, "Vector" );
	int addidx = programEmitConstChars( pp, "add:Ivvv" );
	int clsidx = programEmitConstChars( pp, "Vector" );

	if ( NULL == jsp )
	{
		setupError( lex, pp, YYER_UNKNOWN_CLASS );
		return;
	}

	/* save a spot for this
	programEmit1( pp, OP_PUSH, MODE_IM );
	programEmitArg( pp, -2 );
	*/
	/* setup the constructor call */

	programEmit1( pp, OP_NEW, MODE_CONST );
	programEmitArg( pp, clsidx );
	
	/* now, THIS is on top of the stack */
	/* copy THIS */
	/*programEmit0( pp, OP_DUP ); */

	/* call constructor */
	programEmit2( pp, OP_CONSTRUCT, MODE_CONST, MODE_IM );
	programEmitArg( pp, clsidx );
	programEmitArg( pp, 0 );

	while ( lex->token != RBRACE && lex->token != JEOF )
	{
		/* copy THIS */
		programEmit0( pp, OP_DUP );

		/* push arg */
		/* there should be an INT, FLOAT, or STRING */
		switch ( lex->token )
		{
		case INT:
			programEmit1( pp, OP_PUSH, MODE_IM );
			programEmitArg( pp, atoi(lex->lex) );
			break;
		case REAL:
			programEmit1( pp, OP_PUSH, MODE_CONST );
			programEmitArg( pp, programEmitConstFloat( pp, (float)atof(lex->lex) ) );
			break;
		case STRING:
			programEmit1( pp, OP_PUSH, MODE_CONST );
			programEmitArg( pp, programEmitConstChars( pp, lex->lex ) );
			break;
		case CHAR:
			programEmit1( pp, OP_PUSH, MODE_CONST );
			programEmitArg( pp, programEmitConstChar( pp, lex->lex[0] ) );
			break;
		case YYTRUE:
			programEmit1( pp, OP_PUSH, MODE_IM );
			programEmitArg( pp, true );
			break;
		case YYFALSE:
			programEmit1( pp, OP_PUSH, MODE_IM );
			programEmitArg( pp, false );
			break;
		case JNULL:
			programEmit0( pp, OP_NULL );
			break;
		default:
			setupError( lex, pp, YYER_ILLEGAL_ARRAY_INIT );
			return;
		}
		jsppLex( lex );
		if ( COMMA == lex->token )
		{
			jsppLex( lex );
		}
		
		/* make the call */
		programEmit2( pp, OP_CALL, MODE_CONST, MODE_IM );
		programEmitArg( pp, addidx );
		programEmitArg( pp, 1 );
	}
	if ( lex->token != RBRACE )
	{
		setupError( lex, pp, YYER_EXPECTED_RBRAC );
		return;
	}
	jsppLex( lex );
}

/*
singleton	: ID deref
			| ID SCOPE ID LPAR arglist RPAR deref	// static method call	
			| ID LPAR arglist_call RPAR deref		// in-class method call
			| ID LBRAC expr RBRAC deref
			| ID ASSIGN expr
			| literal
			| LBRACE expr [expr COMMA]* RBRACE
			| LPAR expr RPAR
			| new_expr
			| SCOPE ID LPAR arglist_call RPAR deref	// base class call
			|
			;
literal		: INT
			| STRING
			| CHAR
			| REAL
			;
 */
static void singleton( jspplex *lex, program *pp )
{
	vardef *vdp;

	switch ( lex->token )
	{
	case ID:
		/*
		*  The current lexum is either: 
		*		- a variable in the current scope
		*		- a class name, followed by a "::"
		*		- or a method in the current class
		*/
		vdp = symtabLookup( pp, lex->lex );
		if ( NULL == vdp )
		{
			singletonCallHandler( lex, pp );
		}
		else
		{
			singletonVarHandler( lex, pp, vdp );
		}
		break;

	case LBRACE:
		/* static array init */
		jsppLex( lex );
		arraydef( lex, pp );
		break;

	case INT:
		programEmit1( pp, OP_PUSH, MODE_IM );
		programEmitArg( pp, atoi(lex->lex) );
		jsppLex( lex );
		break;

	case JNULL:
		programEmit0( pp, OP_NULL );
		jsppLex( lex );
		break;

	case REAL:
		programEmit1( pp, OP_PUSH, MODE_CONST );
		programEmitArg( pp, programEmitConstFloat( pp, (float)atof(lex->lex) ) );
		jsppLex( lex );
		break;

	case STRING:
		programEmit1( pp, OP_PUSH, MODE_CONST );
		programEmitArg( pp, programEmitConstChars( pp, lex->lex ) );
		jsppLex( lex );
		break;

	case CHAR:
		programEmit1( pp, OP_PUSH, MODE_CONST );
		programEmitArg( pp, programEmitConstChar( pp, lex->lex[0] ) );
		jsppLex( lex );
		break;

	case YYTRUE:
		jsppLex( lex );
		programEmit1( pp, OP_PUSH, MODE_IM );
		programEmitArg( pp, true );
		break;

	case YYFALSE:
		jsppLex( lex );
		programEmit1( pp, OP_PUSH, MODE_IM );
		programEmitArg( pp, false );
		break;

	case LPAR:
		jsppLex( lex );
		expr( lex, pp );
		if ( RPAR != lex->token )
		{
			setupError( lex, pp, YYER_EXPECTED_RPAR );
			return;
		}
		jsppLex( lex );
		break;

	case NEW:
		new_expr( lex, pp );
		break;

	case SCOPE:
		{
			/* base class method call */
			string *idname;

			jsppLex( lex );
			if ( ID != lex->token )
			{
				setupError( lex, pp, YYER_EXPECTED_ID );
				return;
			}
			idname = stringConstructChar( lex->lex );
			if ( NULL == idname )
			{
				setupError( lex, pp, YYER_OUT_OF_MEM );
				return;
			}
			jsppLex( lex );
			if ( LPAR != lex->token )
			{
				setupError( lex, pp, YYER_EXPECTED_LPAR );
				stringDestruct( idname );
				return;
			}
			jsppLex(lex);

			programEmit0( pp, OP_THIS );
			arglist_call( lex, pp, NULL, idname, true, false );
			stringDestruct( idname );

			if ( RPAR != lex->token )
			{
				setupError( lex, pp, YYER_EXPECTED_RPAR );
				return;
			}
			jsppLex( lex );
			deref( lex, pp );
		}
		break;

	default:
		setupError( lex, pp, YYERR_SYNTAX );
		break;
	}
}

/*
morefactors	: STAR factor morefactors
			| SLASH factor morefactors
			| MOD factor morefactors
			|
			;
 */
static void morefactors( jspplex *lex, program *pp )
{
	switch( lex->token )
	{
	case STAR:
		jsppLex( lex );
		factor( lex, pp );
		programEmit0( pp, OP_MULT );
		break;
	case SLASH:
		jsppLex( lex );
		factor( lex, pp );
		programEmit0( pp, OP_DIV );
		break;
	case MOD:
		jsppLex( lex );
		factor( lex, pp );
		programEmit0( pp, OP_MOD );
		break;
	case LSHIFT:
		jsppLex( lex );
		factor( lex, pp );
		programEmit0( pp, OP_LSHIFT );
		break;
	case RSHIFT:
		jsppLex( lex );
		factor( lex, pp );
		programEmit0( pp, OP_RSHIFT );
		break;
	default:
		return;
	}
	if ( 0 == pp->errornum )
	{
		morefactors( lex, pp );
	}
}

/*
factor	: MINUS singleton
		| PLUS singleton
		| INC singleton
		| DEC singleton
		| BANG singleton
		| singleton INC
		| singleton DEC
		| singleton
		;
 */
static void factor( jspplex *lex, program *pp )
{
	switch ( lex->token )
	{
	case MINUS:
		jsppLex( lex );
		singleton( lex, pp );
		programEmit0( pp, OP_NEGATE );
		break;
	case PLUS:
		jsppLex( lex );
		singleton( lex, pp );
		break;
	case INC:
		jsppLex( lex );
		singleton( lex, pp );
		programEmit0( pp, OP_INC );
		if ( ! pp->isrhs )
		{
			programEmit0( pp, OP_POP );
		}
		break;
	case DEC:
		jsppLex( lex );
		singleton( lex, pp );
		programEmit0( pp, OP_DEC );
		if ( ! pp->isrhs )
		{
			programEmit0( pp, OP_POP );
		}
		break;
	case BANG:
		jsppLex( lex );
		singleton( lex, pp );
		programEmit0( pp, OP_NOT );
		break;
	default:
		singleton( lex, pp );
		break;
	}
	if ( INC == lex->token )
	{
		jsppLex( lex );
		programEmit0( pp, OP_PINC );
		if ( ! pp->isrhs )
		{
			programEmit0( pp, OP_POP );
		}
	}
	if ( DEC == lex->token )
	{
		jsppLex( lex );
		programEmit0( pp, OP_PDEC );
		if ( ! pp->isrhs )
		{
			programEmit0( pp, OP_POP );
		}
	}
}

/*
moreterms	: PLUS term moreterms
			| MINUS term moreterms
			| PLUSEQ term moreterms
			| MINSEQ term moreterms
			| TIMESEQ term moreterms
			| DIVEQ term moreterms
			| MODEEQ term moreterms
			| XOREQ term moreterms
			|
			;
 */
static void moreterms( jspplex *lex, program *pp )
{
	switch( lex->token )
	{
	case PLUSEQ:
		jsppLex( lex );
		pp->isrhs = true;
		programEmit0( pp, OP_DUP );
		expr( lex, pp );
		programEmit0( pp, OP_RVALUE );
		programEmit0( pp, OP_ADD );
		programEmit0( pp, OP_ASSIGN );
		pp->isrhs = false;
		break;
	case MINUSEQ:
		jsppLex( lex );
		pp->isrhs = true;
		programEmit0( pp, OP_DUP );
		expr( lex, pp );
		programEmit0( pp, OP_RVALUE );
		programEmit0( pp, OP_SUB );
		programEmit0( pp, OP_ASSIGN );
		pp->isrhs = false;
		break;
	case TIMESEQ:
		jsppLex( lex );
		pp->isrhs = true;
		programEmit0( pp, OP_DUP );
		expr( lex, pp );
		programEmit0( pp, OP_RVALUE );
		programEmit0( pp, OP_MULT );
		programEmit0( pp, OP_ASSIGN );
		pp->isrhs = false;
		break;
	case DIVEQ:
		jsppLex( lex );
		pp->isrhs = true;
		programEmit0( pp, OP_DUP );
		expr( lex, pp );
		programEmit0( pp, OP_RVALUE );
		programEmit0( pp, OP_DIV );
		programEmit0( pp, OP_ASSIGN );
		pp->isrhs = false;
		break;
	case MODEQ:
		jsppLex( lex );
		pp->isrhs = true;
		programEmit0( pp, OP_DUP );
		expr( lex, pp );
		programEmit0( pp, OP_RVALUE );
		programEmit0( pp, OP_MOD );
		programEmit0( pp, OP_ASSIGN );
		pp->isrhs = false;
		break;
	case XOREQ:
		jsppLex( lex );
		pp->isrhs = true;
		programEmit0( pp, OP_DUP );
		expr( lex, pp );
		programEmit0( pp, OP_RVALUE );
		programEmit0( pp, OP_XOR );
		programEmit0( pp, OP_ASSIGN );
		pp->isrhs = false;
		break;
	case OREQ:
		jsppLex( lex );
		pp->isrhs = true;
		programEmit0( pp, OP_DUP );
		expr( lex, pp );
		programEmit0( pp, OP_RVALUE );
		programEmit0( pp, OP_BINOR );
		programEmit0( pp, OP_ASSIGN );
		pp->isrhs = false;
		break;
	case ANDEQ:
		jsppLex( lex );
		pp->isrhs = true;
		programEmit0( pp, OP_DUP );
		expr( lex, pp );
		programEmit0( pp, OP_RVALUE );
		programEmit0( pp, OP_BINAND );
		programEmit0( pp, OP_ASSIGN );
		pp->isrhs = false;
		break;
	case PLUS:
		jsppLex( lex );
		term( lex, pp );
		programEmit0( pp, OP_ADD );
		break;
	case MINUS:
		jsppLex( lex );
		term( lex, pp );
		programEmit0( pp, OP_SUB );
		break;
	default:
		return;
	}
	if ( 0 == pp->errornum )
	{
		moreterms( lex, pp );
	}
}

/*
term	: factor morefactors
		|
		;
 */
static void term( jspplex *lex, program *ilp )
{
	factor( lex, ilp );
	if ( 0 == ilp->errornum )
	{
		morefactors( lex, ilp );
	}
}

/*
relop	: term moreterms
		|
		;
 */
static void relop( jspplex *lex, program *ilp )
{
	term( lex, ilp );
	if ( 0 == ilp->errornum )
	{
		moreterms( lex, ilp );
	}
}

/*
morerelops	: LT relop morerelops
			| GT relop morerelops
			| LTEQ relop morerelops
			| GTEQ relop morerelops
			|
			;
 */
static void morerelops( jspplex *lex, program *pp )
{
	switch ( lex->token )
	{
	case LT:
		jsppLex( lex );
		relop( lex, pp );
		programEmit0( pp, OP_LT );
		break;
	case GT:
		jsppLex( lex );
		relop( lex, pp );
		programEmit0( pp, OP_GT );
		break;
	case LTEQ:
		jsppLex( lex );
		relop( lex, pp );
		programEmit0( pp, OP_LTEQ );
		break;
	case GTEQ:
		jsppLex( lex );
		relop( lex, pp );
		programEmit0( pp, OP_GTEQ );
		break;
	case ISEQUAL:
		jsppLex( lex );
		relop( lex, pp );
		programEmit0( pp, OP_EQ );
		break;
	case ISNEQ:
		jsppLex( lex );
		relop( lex, pp );
		programEmit0( pp, OP_NEQ );
		break;
	default:
		return;
	}
	if ( 0 == pp->errornum )
	{
		morerelops( lex, pp );
	}
}

static void logop( jspplex *lex, program *pp );

/*
morelogop	: PIPE lopop morelogop
			| AMPR lopop morelogop
			| AND lopop morelogop
			| OR lopop morelogop
			| ISNEQ lopop morelogop
			| ISEQUAL lopop morelogop
			|
			;
 */
static void morelogop( jspplex *lex, program *pp )
{
	switch ( lex->token )
	{
	case PIPE:
		jsppLex( lex );
		logop( lex, pp );
		programEmit0( pp, OP_BINOR );
		break;
	case AMPR:
		jsppLex( lex );
		logop( lex, pp );
		programEmit0( pp, OP_BINAND );
		break;
	case AND:
		jsppLex( lex );
		logop( lex, pp );
		programEmit0( pp, OP_AND );
		break;
	case OR:
		jsppLex( lex );
		logop( lex, pp );
		programEmit0( pp, OP_OR );
		break;
	case XOR:
		jsppLex( lex );
		logop( lex, pp );
		programEmit0( pp, OP_XOR );
		break;
	case ISNEQ:
		jsppLex( lex );
		programEmit0( pp, OP_NEQ );
		logop( lex, pp );
		break;
	case ISEQUAL:
		jsppLex( lex );
		programEmit0( pp, OP_EQ );
		logop( lex, pp );
		break;
	case ASSIGN:
		jsppLex( lex );
		pp->isrhs = true;
		expr( lex, pp );
		programEmit0( pp, OP_RVALUE );
		programEmit0( pp, OP_ASSIGN );
		pp->isrhs = false;
		break;
	default:
		return;
	}
	if ( 0 == pp->errornum )
	{
		morelogop( lex, pp );
	}
}

/*
lopop		: relop
			| morerelops
			;
 */
static void logop( jspplex *lex, program *ilp )
{
	relop( lex, ilp );
	if ( 0 == ilp->errornum )
	{
		morerelops( lex, ilp );
	}
}


/*
expr	: logop morelogop
		;
 */
static void expr( jspplex *lex, program *ilp )
{
	logop( lex, ilp );
	if ( 0 == ilp->errornum )
	{
		morelogop( lex, ilp );
	}
}

static void sif( jspplex *lex, program *ilp );

/*
selse	: ELSE ifstmt
		| ELSE stmtblock
		;
*/
static void selse( jspplex *lex, program *ilp )
{
	ASSERT( ELSE == lex->token );
	jsppLex( lex );

	if ( IF == lex->token )
	{
		sif( lex, ilp );
	}
	else
	{
		stmtblock( lex, ilp );
	}
}

/*
sif		: LPAR expr RPAR stmtblock selse
		;
 */
static void sif( jspplex *lex, program *pp )
{
	int exitPc, endPc;
	ASSERT( IF == lex->token );
	jsppLex( lex );

	if ( LPAR != lex->token )
	{
		setupError( lex, pp, YYER_EXPECTED_LPAR );
		return;
	}
	jsppLex( lex );

	expr( lex, pp );

	if ( RPAR != lex->token )
	{
		setupError( lex, pp, YYER_EXPECTED_RPAR );
		return;
	}
	jsppLex( lex );
	programEmit1( pp, OP_JMPZ, MODE_IM );
	exitPc = programEmitArg( pp, 0 );

	stmtblock( lex, pp );
	programEmit1( pp, OP_JMP, MODE_IM );
	endPc = programEmitArg( pp, 0 );

	programEmitArgAt( pp, exitPc, pp->curmethod->pc );

	if ( ELSE == lex->token )
	{
		selse( lex, pp );
	}
	programEmitArgAt( pp, endPc, pp->curmethod->pc );
}

/*
swhile	: LPAR expr RPAR stmtblock
		;
 */
static void swhile( jspplex *lex, program *pp )
{
	int topPc, exitPc, breakPc;

	ASSERT( WHILE == lex->token );
	jsppLex( lex );

	if ( LPAR != lex->token )
	{
		setupError( lex, pp, YYER_EXPECTED_LPAR );
		return;
	}
	jsppLex( lex );

	programEmit1( pp, OP_SETBREAK, MODE_IM );
	breakPc = programEmitArg( pp, 0 );

	pp->isrhs = true;
	topPc = pp->curmethod->pc;
	expr( lex, pp );
	pp->isrhs = false;

	if ( RPAR != lex->token )
	{
		setupError( lex, pp, YYER_EXPECTED_RPAR );
		return;
	}
	jsppLex( lex );

	programEmit1( pp, OP_JMPZ, MODE_IM );
	exitPc = programEmitArg( pp, 0 );

	stmtblock( lex, pp );
	programEmit1( pp, OP_JMP, MODE_IM );
	programEmitArg( pp, topPc );

	programEmitArgAt( pp, exitPc, pp->curmethod->pc );
	programEmitArgAt( pp, breakPc, pp->curmethod->pc );
	programEmit1( pp, OP_SETBREAK, MODE_IM );
	programEmitArg( pp, -1 );
}

/*
postfor	: expr LPAR stmtblock
		| LPAR stmtblock
		;
 */
static void postfor( jspplex *lex, program *pp, const int evalPc )
{
	int postPc;
	ASSERT( SEMI != lex->token );

	if ( RPAR != lex->token )
	{
		int skipPostPc;
		programEmit1( pp, OP_JMP, MODE_IM );
		skipPostPc = programEmitArg( pp, 0 );
		postPc = pp->curmethod->pc;
		expr( lex, pp );
		programEmit1( pp, OP_JMP, MODE_IM );
		programEmitArg( pp, evalPc );
		programEmitArgAt( pp, skipPostPc, pp->curmethod->pc );
	}
	if ( RPAR != lex->token )
	{
		setupError( lex, pp, YYER_EXPECTED_RPAR );
		return;
	}
	jsppLex( lex );

	stmtblock( lex, pp );
	programEmit1( pp, OP_JMP, MODE_IM );
	programEmitArg( pp, postPc );
}

/*		
evalfor	: expr SEMI postfor
		| SEMI postfor
		;
 */
static void evalfor( jspplex *lex, program *pp, const int breakPc )
{
	int evalPc = pp->curmethod->pc;
	int exitPc;

	if ( SEMI != lex->token )
	{
		expr( lex, pp );
	}
	if ( SEMI != lex->token )
	{
		setupError( lex, pp, YYERR_SYNTAX );
		return;
	}
	jsppLex( lex );

	programEmit1( pp, OP_JMPZ, MODE_IM );
	exitPc = programEmitArg( pp, 0 );

	postfor( lex, pp, evalPc );
	programEmitArgAt( pp, exitPc, pp->curmethod->pc );
	programEmitArgAt( pp, breakPc, pp->curmethod->pc );
}

/*
sfor	: LPAR (localvar|expr) SEMI evalfor
		;
 */
static void sfor( jspplex *lex, program *pp )
{
	int breakPc;
	ASSERT( FOR == lex->token );
	jsppLex( lex );

	if ( LPAR != lex->token )
	{
		setupError( lex, pp, YYER_EXPECTED_LPAR );
		return;
	}
	jsppLex( lex );

	if ( SEMI != lex->token )
	{
		if ( VAR == lex->token )
		{
			localvar( lex, pp );
		}
		else
		{
			expr( lex, pp );
		}
	}
	if ( SEMI != lex->token )
	{
		setupError( lex, pp, YYERR_SYNTAX );
		return;
	}
	jsppLex( lex );

	programEmit1( pp, OP_SETBREAK, MODE_IM );
	breakPc = programEmitArg( pp, 0 );
	evalfor( lex, pp, breakPc );

	programEmit1( pp, OP_SETBREAK, MODE_IM );
	programEmitArg( pp, -1 );
}

/*
switchblock	: CASE (INT|REAL|STRING|CHAR) COLON stmts switchblock
			|
			;
 */
static void switchblock( jspplex *lex, program *pp )
{
	int nextPc;
	int defaultPc = -1;

	if ( LBRACE != lex->token )
	{
		setupError( lex, pp, YYER_EXPECTED_LBRACE );
		return;
	}
	jsppLex( lex );
	programEmit1( pp, OP_ENTER, MODE_IM );
	symtabEnter( pp, programEmitArg( pp, 0 ) );

	while ( CASE == lex->token )
	{

		/* duplicate the conditional exp */
		programEmit0( pp, OP_DUP );

		jsppLex( lex );
		switch ( lex->token )
		{
		case INT:
			programEmit1( pp, OP_PUSH, MODE_IM );
			programEmitArg( pp, atoi( lex->lex ) );
			jsppLex( lex );
			break;
		case STRING:
			programEmit1( pp, OP_PUSH, MODE_CONST );
			programEmitArg( pp, programEmitConstChars( pp, lex->lex ) );
			jsppLex( lex );
			break;
		case CHAR:
			programEmit1( pp, OP_PUSH, MODE_CONST );
			programEmitArg( pp, programEmitConstChar( pp, lex->lex[0] ) );
			jsppLex( lex );
			break;
		case ID:
			singleton( lex, pp );
			break;
		default:
			setupError( lex, pp, YYER_ILLEGAL_CASE_VALUE );
			return;
		}
		if ( COLON != lex->token )
		{
			setupError( lex, pp, YYER_EXPECTED_COLON );
			return;
		}
		jsppLex( lex );

		programEmit0( pp, OP_EQ );
		programEmit1( pp, OP_JMPZ, MODE_IM );
		nextPc = programEmitArg( pp, 0 );
		stmts( lex, pp );
		programEmitArgAt( pp, nextPc, pp->curmethod->pc );
		
		if ( DEFAULT == lex->token )
		{
			int exitPc;
			jsppLex( lex );
			if ( COLON != lex->token )
			{
				setupError( lex, pp, YYER_EXPECTED_COLON );
				return;
			}
			jsppLex( lex );
			programEmit1( pp, OP_JMP, MODE_IM );
			exitPc = programEmitArg( pp, 0 );
			defaultPc = pp->curmethod->pc;
			stmts( lex, pp );
			programEmitArgAt( pp, exitPc, pp->curmethod->pc );
		}
	}
	programEmit1( pp, OP_JMP, MODE_IM );
	programEmitArg( pp, defaultPc );

	programEmit0( pp, OP_LEAVE );
	symtabExit( pp );

	if ( RBRACE != lex->token )
	{
		setupError( lex, pp, YYER_EXPECTED_RBRACE );
		return;
	}
	jsppLex( lex );
}

/*
sswitch		: LPAR expr RPAR switchblock
			;
 */
static void sswitch( jspplex *lex, program *pp )
{
	int breakPc;
	ASSERT( SWITCH == lex->token );

	jsppLex( lex );
	if ( LPAR != lex->token )
	{
		setupError( lex, pp, YYER_EXPECTED_LPAR );
		return;
	}
	jsppLex( lex );

	expr ( lex, pp );

	if ( RPAR != lex->token )
	{
		setupError( lex, pp, YYER_EXPECTED_RPAR );
		return;
	}
	jsppLex( lex );

	programEmit1( pp, OP_SETBREAK, MODE_IM );
	breakPc = programEmitArg( pp, 0 );
	
	switchblock( lex, pp );
	
	programEmitArgAt( pp, breakPc, pp->curmethod->pc );
}

/*
sreturn		: expr
			|
			;
 */
static void sreturn( jspplex *lex, program *pp )
{
	ASSERT( RETURN == lex->token );

	if ( SEMI != jsppLex( lex ) )
	{
		pp->isrhs = true;
		pp->curmethod->hasretval = true;
		expr( lex, pp );
		programEmit0( pp, OP_RET );
		pp->isrhs = false;
	}
	else
	{
		if ( pp->curmethod->hasretval )
		{
			setupError( lex, pp, YYER_METH_RETURNS_VAL );
			return;
		}
		programEmit0( pp, OP_LEAVE );
		programEmit0( pp, OP_VRET );
	}
}

/*
localvar	: ID [ASSIGN (expr)]- [COMMA localvar]-
			;
 */
static void localvar( jspplex *lex, program *pp )
{
	vardef *vdp = NULL;

	ASSERT( lex->token == VAR );

	while ( SEMI != lex->token && 0 == pp->errornum )
	{
		if ( ID != jsppLex( lex ) )
		{
			setupError( lex, pp, YYER_EXPECTED_VAR_NAME );
			return;
		}
		vdp = symtabNote( pp, lex->lex, false );
		if ( NULL == vdp )
		{
			setupError( lex, pp, YYER_OUT_OF_MEM );
			return;
		}
		if ( ASSIGN == jsppLex( lex ) )
		{
			jsppLex( lex );
			pp->isrhs = true;
			programEmit1( pp, OP_LVALUE, vdp->srcmode );
			programEmitArg( pp, vdp->offset );
			expr( lex, pp );
			programEmit0( pp, OP_ASSIGN );
			pp->isrhs = false;
		}
		if ( 0 != pp->errornum )
		{
			return;
		}
		if ( COMMA == lex->token )
		{
			jsppLex( lex );
		}
		else if ( SEMI != lex->token )
		{
			setupError( lex, pp, YYER_EXPECTED_SEMI );
			return;
		}
	}
}

/*
dotry		: stmtblock CATCH LPAR VAR ID RPAR stmtblock [FINALLY stmtblock]-
			;
 */
static void dotry( jspplex *lex, program *pp )
{
	int trypc;
	int endtrypc;
	int startcatchpc;
	int finallypc;
	vardef *vdp;

	ASSERT( TRY != lex->token );

	programEmit1( pp, OP_TRY, MODE_IM );
	trypc = programEmitArg( pp, -1 );

	stmtblock( lex, pp );

	programEmit1( pp, OP_PUSH, MODE_IM );
	programEmitArg( pp, false );
	programEmit1( pp, OP_JMP, MODE_IM );
	endtrypc = programEmitArg( pp, 0 );

	programEmitArgAt( pp, trypc, pp->curmethod->pc );

	ASSERT( RBRACE != lex->token );
	if ( CATCH != lex->token )
	{
		setupError( lex, pp, YYER_EXPECTED_CATCH );
		return;
	}
	jsppLex( lex );
	if ( LPAR != lex->token )
	{
		setupError( lex, pp, YYER_EXPECTED_LPAR );
		return;
	}
	jsppLex( lex );
	if ( VAR == lex->token )
	{
		jsppLex( lex );
	}
	if ( ID != lex->token )
	{
		setupError( lex, pp, YYER_EXPECTED_ID );
		return;
	}
	vdp = symtabNote( pp, lex->lex, false );
	jsppLex( lex );
	if ( RPAR != lex->token )
	{
		setupError( lex, pp, YYER_EXPECTED_RPAR );
		return;
	}
	jsppLex( lex );

	/* move the thrown value into the catch argument */
	programEmit1( pp, OP_LVALUE, MODE_BLOCK );
	programEmitArg( pp, vdp->offset );
	programEmit0( pp, OP_ROT );
	programEmit0( pp, OP_ASSIGN );

	/* call the finially block */
	programEmit1( pp, OP_PUSH, MODE_IM );
	programEmitArg( pp, true );
	programEmit1( pp, OP_JMP, MODE_IM );
	finallypc = programEmitArg( pp, 0 );
	startcatchpc = pp->curmethod->pc;

	stmtblock( lex, pp );

	programEmitArgAt( pp, endtrypc, pp->curmethod->pc );
	programEmitArgAt( pp, finallypc, pp->curmethod->pc );
	if ( FINALLY == lex->token )
	{		
		jsppLex( lex );
		stmtblock( lex, pp );
	}
	programEmit1( pp, OP_JMPNZ, MODE_IM );
	programEmitArg( pp, startcatchpc );
}

/*
stmts	: IF sif
		| WHILE swhile
		| FOR sfor
		| SWITCH sswitch
		| RETURN sreturn
		| VAR localvar
		| SEMI
		| BREAK
		| CONTINUE
		| DELETE delop
		| TRY dotry
		| LEXERROR
		| expr
		;
 */
static void stmts( jspplex *lex, program *pp )
{
	ASSERT( LBRACE != lex->token );
	
	while ( RBRACE != lex->token && CASE != lex->token && 0 == pp->errornum )
	{
		if ( SEMI != lex->token && pp->isdebuging )
		{
			programEmit1( pp, OP_LINE, MODE_IM);
			programEmitArg( pp, lex->lineno );
		}
		switch ( lex->token )
		{
		case IF:
			sif( lex, pp );
			break;
		case WHILE:
			swhile( lex, pp );
			break;
		case FOR:
			sfor( lex, pp );
			break;
		case SWITCH:
			sswitch( lex, pp );
			break;
		case RETURN:
			sreturn( lex, pp );
			break;
		case VAR:
			localvar( lex, pp );
			break;
		case SEMI:
			jsppLex( lex );
			break;
		case BREAK:
			programEmit0( pp, OP_BREAK );
			jsppLex( lex );
			break;
		case CONTINUE:
			programEmit0( pp, OP_CONTINUE );
			jsppLex( lex );
			break;
		case DELETE:
			jsppLex( lex );
			delop( lex, pp );
			break;
		case TRY:
			jsppLex( lex );
			dotry( lex, pp );
			break;
		case THROW:
			jsppLex( lex );
			expr( lex, pp );
			programEmit0( pp, OP_THROW );
			break;
		case LEXERROR:
			setupError( lex, pp, YYERR_SYNTAX );
			return;
		case DEFAULT:
			return;
		default:
			expr( lex, pp );
			break;
		}
	}
}

/*
stmtblock	: LBRACE stmts RBRACE
			;
 */
static void stmtblock( jspplex *lex, program *pp )
{
	if ( LBRACE != lex->token )
	{
		setupError( lex, pp, YYER_EXPECTED_LBRACE );
		return;
	}
	jsppLex( lex );

	programEmit1( pp, OP_ENTER, MODE_IM );
	symtabEnter( pp, programEmitArg( pp, 0 ) );

	stmts( lex, pp );
	if ( 0 != pp->errornum )
	{
		return;
	}

	programEmit0( pp, OP_LEAVE );
	symtabExit(pp);

	if ( RBRACE != lex->token )
	{
		setupError( lex, pp, YYER_EXPECTED_RBRACE );
		return;
	}
	jsppLex( lex );
}

/*
methodbody	: LBRACE stmts RBRACE
			;
 */
static void methodbody( jspplex *lex, program *pp )
{
	if ( LBRACE != lex->token )
	{
		setupError( lex, pp, YYER_EXPECTED_LBRACE );
		return;
	}
	jsppLex( lex );

	stmts( lex, pp );
	if ( 0 != pp->errornum )
	{
		return;
	}

	programEmit0( pp, OP_LEAVE );
	symtabExit(pp);

	if ( RBRACE != lex->token )
	{
		setupError( lex, pp, YYER_EXPECTED_RBRACE );
		return;
	}
	jsppLex( lex );
}

/*
argdec	: LPAR arglist RPAR
		;
 */
static void argdec( jspplex *lex, program *pp )
{
	if ( LPAR != jsppLex( lex ) )
	{
		setupError( lex, pp, YYER_EXPECTED_LPAR );
		return;
	}
	jsppLex( lex );

	while ( lex->token == ID || lex->token == AMPR )
	{
		bool isptr = false;
		if ( AMPR == lex->token )
		{
			jsppLex( lex );
			isptr = true;
		}
		jsmethodAddArg( pp->curmethod, lex->lex, isptr );
		symtabNote( pp, lex->lex, isptr );

		if ( COMMA == jsppLex( lex ) )
		{
			jsppLex( lex );
		}
		else if ( RPAR != lex->token )
		{
			setupError( lex, pp, YYER_EXPECTED_RPAR );
			return;
		}
	}
	if ( RPAR != lex->token )
	{
		setupError( lex, pp, YYER_EXPECTED_RPAR );
		return;
	}
	jsppLex( lex );
}

/*
method	: accessspec storagetype ID LPAR idlist RPAR methodbody 
		;
 */
static void method( const enum jsaccess acc, const bool isstatic, jspplex *lex, program *pp )
{
	if ( ID != lex->token )
	{
		setupError( lex, pp, YYER_EXPECTED_METHOD_NAME );
		return;
	}
	programDefineMethod( pp, lex->lex, isstatic, acc );

	programEmit1( pp, OP_ENTER, MODE_IM );
	symtabEnter( pp, programEmitArg( pp, 0 ) );

	if ( ! isstatic )
	{
		jsmethodAddArg( pp->curmethod, "this", false );
		symtabNote( pp, "this", false );
	}
	argdec( lex, pp );
	if ( 0!= pp->errornum )
	{
		return;
	}
	methodbody( lex, pp );
}

/*
new_expr	: NEW ID LPAR idlist RPAR
			| NEW ID LBRAC INT RBRAC
			| NEW ID LBRAC ID RBRAC
			;
 */
static void new_expr( jspplex *lex, program *pp )
{
	string *clsname;
	int clsidx;

	ASSERT( NEW == lex->token );
	jsppLex( lex );

	if ( ID != lex->token )
	{
		setupError( lex, pp, YYER_EXPECTED_ID );
		return;
	}
	clsname = stringConstructChar( lex->lex );
	if ( NULL == clsname )
	{
		setupError( lex, pp, YYER_OUT_OF_MEM );
		return;
	}
	clsidx = programEmitConstChars( pp, lex->lex );
	jsppLex( lex );

	if ( LPAR == lex->token )
	{
		/*
		 *  construct class
		 */
		jsclass *jsp = programFindClass( pp, clsname->cstr );
		if ( NULL == jsp )
		{/*
			if ( NULL != pp->intrinsics )
			{
				jsp = programFindClass( pp->intrinsics, clsname->cstr );
				if ( NULL == jsp )
				{
					stringDestruct( clsname );
					setupError( lex, pp, YYER_UNKNOWN_CLASS );
					return;
				}
			}
			else
			{*/
				stringDestruct( clsname );
				setupError( lex, pp, YYER_UNKNOWN_CLASS );
				return;
			/*}*/
		}
		jsppLex( lex );

		/* save a spot for this */
		/*programEmit1( pp, OP_PUSH, MODE_IM ); */
		/*programEmitArg( pp, -2 ); */

		programEmit1( pp, OP_NEW, MODE_CONST );
		programEmitArg( pp, clsidx );
		/*programEmitArg( pp, argcount ); */

		/* setup the constructor call */
		arglist_call( lex, pp, NULL, clsname, false, true );

		if ( 0 != pp->errornum )
		{
			stringDestruct( clsname );
			return;
		}
		if ( RPAR != lex->token )
		{
			stringDestruct( clsname );
			setupError( lex, pp, YYER_EXPECTED_LPAR );
			return;
		}
		jsppLex( lex );
	}
	else
	{
		stringDestruct( clsname );
		setupError( lex, pp, YYER_NEW_SYNTAX );
		return;
	}
	stringDestruct( clsname );
}

/*
decspec	: ASSIGN expr
		|
		;
 */
static void decspec( jspplex *lex, program *pp )
{
	if ( ASSIGN != lex->token )
	{
		return;
	}
	jsppLex( lex );

	expr( lex, pp );
}

/*
vardef		: accessspec storagetype VAR ID decspec SEMI
			;
accessspec	: PUBLIC | PRIVATE | PROTECTED
			;
storagetype	: STATIC | CONST |
			;
 */
static void cvardef
( 
	const enum jsaccess acc, 
	const bool isstatic, 
	const bool isconst, 
	jspplex *lex, 
	program *pp 
)
{
	vardef *vdp;
	jsmethod *outer = pp->curmethod;

	ASSERT( VAR == lex->token );
	jsppLex( lex );

	if ( ID != lex->token )
	{
		setupError( lex, pp, YYER_EXPECTED_ID );
		return;
	}
	vdp = programDefineField( pp, lex->lex, isstatic, TYPE_VOID, TYPE_VOID );
	if ( true == vdp->isstatic )
	{
		pp->curmethod = jsclassFindMethod( pp->curclass, "@@constructor:Sv" );
	}
	else
	{
		pp->curmethod = jsclassFindMethod( pp->curclass, "@@constructor:Ivv" );
	}
	jsppLex( lex );
	
	ASSERT( NULL != pp->curmethod );

	if ( ASSIGN == lex->token )
	{
		pp->isrhs = true;
		programEmit1( pp, OP_LVALUE, vdp->srcmode );
		programEmitArg( pp, vdp->offset );
		decspec( lex, pp );
		programEmit0( pp, OP_ASSIGN );
		pp->isrhs = false;
	}
	if ( SEMI != lex->token )
	{
		setupError( lex, pp, YYER_EXPECTED_SEMI );
		return;
	}
	jsppLex( lex );
	pp->curmethod = outer;
}

/*
classbody 	: vardef classbody
			| method classbody
			|
			;
*/
static void classbody( jspplex *lex, program *pp )
{
	enum jsaccess acc;
	bool isstatic;
	bool isconst;

	ASSERT( LBRACE == lex->token );

	jsppLex( lex );

	while ( 0 == pp->errornum && RBRACE != lex->token )
	{
		isstatic = false;
		isconst = false;
		acc = ACC_PRIVATE;

		switch ( lex->token  )
		{
		case PUBLIC:
			acc = ACC_PUBLIC;
			break;
		case PRIVATE:
			acc = ACC_PRIVATE;
			break;
		case PROTECTED:
			acc = ACC_PROTECTED;
			break;
		case RBRACE:
			return;
		default:
			setupError( lex, pp, YYER_EXPECTED_ACCESS );
			return;
		}
		jsppLex( lex );
		if ( STATIC == lex->token )
		{
			isstatic = true;
			jsppLex( lex );
		}
		if ( CONST == lex->token )
		{
			isconst = true;
			jsppLex( lex );
		}
		switch ( lex->token  )
		{
		case VAR:
			cvardef( acc, isstatic, isconst, lex, pp );
			ASSERT( SEMI != lex->token );
			break;
		case ID:
			method( acc, isstatic, lex, pp );
			break;
		case LEXERROR:
			setupError( lex, pp, 0 );
			return;
		default:
			setupError( lex, pp, YYER_EXPECTED_VAR_OR_CLASS );
			return;
		}
	}
	if( RBRACE != lex->token && 0 == pp->errornum )
	{
		setupError( lex, pp, YYER_EXPECTED_RBRACE );
	}
}

/*
declaration 	: CLASS ID [: ID]- LBRACE classbody RBRACE 
				| accessspec [STATIC]- ID SCOPE ID argdec methodbody 
				;
*/
static void declaration( jspplex *lex, program *pp )
{
	string *classname = NULL;

	if ( CLASS == lex->token )
	{
		jsmethod *prconstructI;
		jsmethod *prconstructS;

		if ( jsppLex( lex ) != ID )
		{
			setupError( lex, pp, YYER_EXPECTED_CLASS_NAME );
			return;
		}
		classname = stringConstructChar( lex->lex );
		if ( NULL == classname )
		{
			setupError( lex, pp, YYER_OUT_OF_MEM );
			return;
		}
		jsppLex( lex );
		
		if ( COLON == lex->token )
		{
			string *basename;

			jsppLex( lex );
			if ( ID != lex->token )
			{
				setupError( lex, pp, YYER_EXPECTED_SUPER_CLASS );
				return;
			}
			basename = stringConstructChar( lex->lex );
			if ( NULL == basename )
			{
				setupError( lex, pp, YYER_OUT_OF_MEM );
				return;
			}
			jsppLex( lex );
			programDefineClass( pp, classname->cstr, basename->cstr, NULL );
			stringDestruct( basename );
		}
		else
		{
			programDefineClass( pp, classname->cstr, "Object", NULL );
		}
		stringDestruct( classname );

		if ( LBRACE != lex->token )
		{
			setupError( lex, pp, YYER_EXPECTED_LBRACE );
			return;
		}
		prconstructI = programDefineMethod( pp, "@@constructor", false, ACC_PRIVATE );
		jsmethodAddArg( pp->curmethod, "this", false );
		prconstructS = programDefineMethod( pp, "@@constructor", true, ACC_PRIVATE );

		classbody( lex, pp );
		if ( 0 != pp->errornum )
		{
			return;
		}
		if ( RBRACE != lex->token )
		{
			setupError( lex, pp, YYER_EXPECTED_RBRACE );
			return;
		}
		jsppLex( lex );

		pp->curmethod = prconstructI;
		programEmit0( pp, OP_VRET );
		pp->curmethod = prconstructS;
		programEmit0( pp, OP_VRET );
	}
	else if ( PUBLIC == lex->token || PRIVATE == lex->token || PROTECTED == lex->token )
	{
		/*
		 * class method (c++ style)
		 */
		enum jsaccess acc;
		bool isstatic = false;

		if ( PUBLIC == lex->token )
		{
			acc = ACC_PUBLIC;
		}
		else if ( PRIVATE == lex->token )
		{
			acc = ACC_PRIVATE;
		}
		else
		{
			acc = ACC_PROTECTED;
		}
		jsppLex( lex );
		if ( STATIC == lex->token )
		{
			isstatic = true;
			jsppLex( lex );
		}
		if ( lex->token != ID )
		{
			setupError( lex, pp, YYER_EXPECTED_CLASS_NAME );
			return;
		}
		pp->curclass = programFindClass( pp, lex->lex );
		if ( NULL == pp->curclass )
		{/*
			if ( NULL != pp->intrinsics )
			{
				pp->curclass = programFindClass( pp->intrinsics, lex->lex );
				if ( NULL == pp->curclass )
				{
					setupError( lex, pp, YYER_UNKNOWN_CLASS );
					return;
				}
			}
			else
			{*/
				setupError( lex, pp, YYER_UNKNOWN_CLASS );
				return;
			/*}*/
		}
		jsppLex( lex );
		if ( SCOPE != lex->token )
		{
			setupError( lex, pp, YYER_EXPECTED_SCOPE_OP );
			return;
		}
		if ( jsppLex( lex ) != ID )
		{
			setupError( lex, pp, YYER_EXPECTED_METHOD_NAME );
			return;
		}
		programDefineMethod( pp, lex->lex, isstatic, acc );
		programEmit1( pp, OP_ENTER, MODE_IM );
		symtabEnter( pp, programEmitArg( pp, 0 ) );
		argdec( lex, pp );
		methodbody( lex, pp );
	}
	else
	{
		pp->errornum = YYER_EXPECTED_CLASS_DEF;
		pp->errorline = lex->lineno;
		return;
	}
}

static void file( jspplex *lex, program *pp )
{	
	ASSERT( pp->errornum == 0);
	
	jsppLex( lex );

	while ( JEOF != lex->token && pp->errornum == 0 )
	{
		if ( LEXERROR == lex->token )
		{
			pp->errornum = atoi( lex->lex );
			pp->errorline = lex->lineno;
			break;
		}
		declaration( lex, pp );
	}
}

/*
 *  *lex is deleted by this function
 */
static program *parse( jspplex *lex, vector *intrinsics )
{
	program *ilp = programConstruct();
	if ( NULL == ilp )
	{
		jspplexDestruct( lex );
		return NULL;
	}
	if ( NULL != intrinsics )
	{
		programSetIntrinsics( ilp, intrinsics );
	}
	file ( lex, ilp );
	jspplexDestruct( lex );

	return ilp;
}

program *parseFile( const char *filename, vector *intrinsics )
{
	jspplex *lex;

	FILE *fp = fopen( filename, "r" );
	if ( NULL == fp )
	{
		return NULL;
	}
	lex = jspplexConstructFile( fp );
	if ( NULL == lex )
	{
		return NULL;
	}
	fclose( fp );

	return parse( lex, intrinsics );
}

program *parseChar( const char *buf, vector *intrinsics )
{
	jspplex *lex = jspplexConstructChar( buf );
	if ( NULL == lex )
	{
		return NULL;
	}
	return parse( lex, intrinsics );
}

