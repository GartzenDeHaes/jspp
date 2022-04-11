#ifdef HAVE_CONFIG_H
#include <jspp/config.h>
#endif
#include <stdlib.h>
#ifdef HAVE_STRING_H
#include <string.h>
#endif
#include <ctype.h>
#include <spl/debug.h>
#include <jspp/lex.h>
#include <jspp/parseerrors.h>
#include <spl/strbuf.h>

jspplex *jspplexConstructChar( const char *buffer )
{
	jspplex *lex = (jspplex *)malloc( sizeof(struct jspplex) );
	if ( NULL == lex )
	{
		return NULL;
	}
	lex->buffer = stringConstructChar( buffer );
	lex->lex[0] = '\0';
	lex->lineno = 1;
	lex->pos = 0;

	return lex;
}

jspplex *jspplexConstructBuf( const string *buffer )
{
	jspplex *lex = (jspplex *)malloc( sizeof(struct jspplex) );
	if ( NULL == lex )
	{
		return NULL;
	}
	lex->buffer = stringConstructDup( buffer );
	lex->lex[0] = '\0';
	lex->lineno = 1;
	lex->pos = 0;
	if ( '#' == lex->buffer->cstr[0] )
	{
		while ( '\r' != lex->buffer->cstr[lex->pos] && '\n' != lex->buffer->cstr[lex->pos] )
		{
			lex->pos++;
		}
	}
	return lex;
}

jspplex *jspplexConstructFile( FILE *fp )
{
	size_t count;
	char cbuf[513];
	jspplex *ret;
	strbuf *buf = strbufConstructBuf( 1000 );
	string *sbuf;

	if ( NULL == buf )
	{
		return NULL;
	}
	while ( (count = fread( cbuf, 1, 512, fp )) > 0 )
	{
		ASSERT( count < 513 );
		cbuf[count] = '\0';
		strbufAppend( buf, cbuf );
	}
	sbuf = strbufToString( buf );
	if ( NULL == sbuf )
	{
		strbufDestruct( buf );
		return NULL;
	}
	ret = jspplexConstructBuf( sbuf );
	stringDestruct( sbuf );
	strbufDestruct( buf );
	return ret;
}

void jspplexDestruct( jspplex *lex )
{
	stringDestruct( lex->buffer );
	free( lex );
}

static int getch( jspplex *lexp )
{
	int ch;
	if ( lexp->pos >= lexp->buffer->len )
	{
		ASSERT( lexp->pos == lexp->buffer->len );
		ASSERT( lexp->buffer->cstr[lexp->pos] == '\0' );
		return -1;
	}
	ch = lexp->buffer->cstr[lexp->pos++];
	if ( '\n' == ch )
	{
		lexp->lineno++;
	}
	return ch;
}

static int ungetch( jspplex *lexp, const int ch )
{
	if ( lexp->buffer->len == 0 )
	{
		ASSERT( ch == -1 );
		return -1;
	}
	if ( ch == -1 )
	{
		ASSERT( lexp->pos == lexp->buffer->len );
		return -1;
	}
	else if ( ch == '\n' )
	{
		lexp->lineno--;
	}
	ASSERT( lexp->buffer->cstr[lexp->pos-1] == ch );
	lexp->pos--;
	return lexp->buffer->cstr[lexp->pos-1];
}

static int getescape( jspplex *lexp )
{
	int ch = getch( lexp );
	switch ( ch )
	{
	case 'n':
		ch = '\n';
		break;
	case 'r':
		ch = '\r';
		break;
	case 't':
		ch = '\t';
		break;
	case 'v':
		ch = '\v';
		break;
	case '\'':
		ch = '\'';
		break;
	case '\\':
		ch = '\\';
		break;
	default:
		ch = -1;
	}
	return ch;
}

static int isws( const int ch )
{
	return ! (isalnum(ch) || ispunct( ch ) || ch == -1);
}

static void stripWS( jspplex *lexp )
{
	int ch = getch( lexp );

	while ( isws(ch) )
	{
		ch = getch( lexp );
	}
	ungetch( lexp, ch );
}

int jsppLex( jspplex *lexp )
{
	int ch;
	int lexpos = 0;

	stripWS( lexp );

	ch = getch( lexp );
	if ( ch <= 0 )
	{
		lexp->token = JEOF;
		return lexp->token;
	}
	if ( isdigit( ch ) || ch == '.' )
	{
		int toke = INT;

		if ( ch == '.' )
		{
			ch = getch( lexp );
			if ( ! isdigit( ch ) )
			{
				ungetch( lexp, ch );
				lexp->token = DOT;
				return lexp->token;
			}
			toke = REAL;
			ch = ungetch( lexp, ch );
		}

		/*
		 * read some sort of number
		 */
		while ( isdigit(ch) )
		{
			lexp->lex[lexpos++] = (char)ch;
			ch = getch( lexp );
		}
		if ( ch == 'x' )
		{
			if ( lexpos != 1 )
			{
				sprintf( lexp->lex, "%d", LEX_INVALID_NUMBER_FORMAT );
				lexp->token = LEXERROR;
				return lexp->token;
			}
			if ( lexp->lex[0] != '0' )
			{
				sprintf( lexp->lex, "%d", LEX_INVALID_NUMBER_FORMAT );
				lexp->token = LEXERROR;
				return lexp->token;
			}
			lexp->lex[lexpos++] = (char)ch;
			ch = getch( lexp );
			while ( isdigit(ch) )
			{
				lexp->lex[lexpos++] = (char)ch;
				ch = getch( lexp );
			}
			lexp->lex[lexpos] = '\0';
			ungetch( lexp, ch );
			lexp->token = HEX;
			return lexp->token;
		}
		if ( ch == '.' )
		{
			toke = REAL;

			lexp->lex[lexpos++] = (char)ch;
			ch = getch( lexp );
			while ( isdigit(ch) )
			{
				lexp->lex[lexpos++] = (char)ch;
				ch = getch( lexp );
			}
		}
		if ( ch == 'e' || ch == 'E' )
		{
			toke = REAL;
			lexp->lex[lexpos++] = (char)ch;
			ch = getch( lexp );
			if ( ch == '+' || ch == '-' )
			{
				lexp->lex[lexpos++] = (char)ch;
				ch = getch( lexp );
			}
			while ( isdigit(ch) )
			{
				lexp->lex[lexpos++] = (char)ch;
				ch = getch( lexp );
			}
		}
		ungetch( lexp, ch );
		lexp->lex[lexpos] = '\0';
		lexp->token = toke;
		return lexp->token;
	}
	else if ( isalpha( ch ) || ch == '_' )
	{
		/*
		 * identifier
		 */
		while ( isalnum( ch ) || ch == '_' )
		{
			lexp->lex[lexpos++] = (char)ch;
			ch = getch( lexp );
		}
		ungetch( lexp, ch );
		lexp->lex[lexpos] = '\0';
		if ( strcmp(lexp->lex, "var") == 0 )
		{
			lexp->token = VAR;
			return lexp->token;
		}
		if ( strcmp(lexp->lex, "if") == 0 )
		{
			lexp->token = IF;
			return lexp->token;
		}
		if ( strcmp(lexp->lex, "else") == 0 )
		{
			lexp->token = ELSE;
			return lexp->token;
		}
		if ( strcmp(lexp->lex, "return") == 0 )
		{
			lexp->token = RETURN;
			return lexp->token;
		}
		if ( strcmp(lexp->lex, "while") == 0 )
		{
			lexp->token = WHILE;
			return lexp->token;
		}
		if ( strcmp(lexp->lex, "do") == 0 )
		{
			lexp->token = DO;
			return lexp->token;
		}
		if ( strcmp(lexp->lex, "for") == 0 )
		{
			lexp->token = FOR;
			return lexp->token;
		}
		if ( strcmp(lexp->lex, "break") == 0 )
		{
			lexp->token = BREAK;
			return lexp->token;
		}
		if ( strcmp(lexp->lex, "static") == 0 )
		{
			lexp->token = STATIC;
			return lexp->token;
		}
		if ( strcmp(lexp->lex, "class") == 0 )
		{
			lexp->token = CLASS;
			return lexp->token;
		}
		if ( strcmp(lexp->lex, "public") == 0 )
		{
			lexp->token = PUBLIC;
			return lexp->token;
		}
		if ( strcmp(lexp->lex, "private") == 0 )
		{
			lexp->token = PRIVATE;
			return lexp->token;
		}
		if ( strcmp(lexp->lex, "protected") == 0 )
		{
			lexp->token = PROTECTED;
			return lexp->token;
		}
		if ( strcmp(lexp->lex, "continue") == 0 )
		{
			lexp->token = CONTINUE;
			return lexp->token;
		}
		if ( strcmp(lexp->lex, "switch") == 0 )
		{
			lexp->token = SWITCH;
			return lexp->token;
		}
		if ( strcmp(lexp->lex, "case") == 0 )
		{
			lexp->token = CASE;
			return lexp->token;
		}
		if ( strcmp(lexp->lex, "default") == 0 )
		{
			lexp->token = DEFAULT;
			return lexp->token;
		}
		if ( strcmp(lexp->lex, "new") == 0 )
		{
			lexp->token = NEW;
			return lexp->token;
		}
		if ( strcmp(lexp->lex, "delete") == 0 )
		{
			lexp->token = DELETE;
			return lexp->token;
		}
		if ( strcmp(lexp->lex, "const") == 0 )
		{
			lexp->token = CONST;
			return lexp->token;
		}
		if ( strcmp(lexp->lex, "null") == 0 )
		{
			lexp->token = JNULL;
			return lexp->token;
		}
		if ( strcmp(lexp->lex, "try") == 0 )
		{
			lexp->token = TRY;
			return lexp->token;
		}
		if ( strcmp(lexp->lex, "catch") == 0 )
		{
			lexp->token = CATCH;
			return lexp->token;
		}
		if ( strcmp( lexp->lex, "finally") == 0 )
		{
			lexp->token = FINALLY;
			return lexp->token;
		}
		if ( strcmp( lexp->lex, "throw") == 0 )
		{
			lexp->token = THROW;
			return lexp->token;
		}
		if ( strcmp( lexp->lex, "true") == 0 )
		{
			lexp->token = YYTRUE;
			return lexp->token;
		}
		if ( strcmp( lexp->lex, "false") == 0 )
		{
			lexp->token = YYFALSE;
			return lexp->token;
		}
		lexp->token = ID;
		return lexp->token;
	}
	switch ( ch )
	{
	case '\'':
		/*
		 * char
		 */
		ch = getch( lexp );
		if ( ch == '\\' )
		{
			ch = getescape( lexp );
			if ( -1 == ch )
			{
				sprintf( lexp->lex, "%d", LEX_UNEXPECTED_ESCAPE );
				lexp->token = LEXERROR;
				return lexp->token;
			}
		}
		lexp->lex[lexpos++] = (char)ch;
		lexp->lex[lexpos] = '\0';
		ch = getch( lexp );
		if ( ch != '\'' )
		{
			sprintf( lexp->lex, "%d", LEX_UNTERMINATED_CHAR );
			lexp->token = LEXERROR;
			return lexp->token;
		}
		lexp->token = CHAR;
		return lexp->token;

	case '"':
		/*
		 * string
		 */
		while ( (ch = getch( lexp )) != -1 && ch != '"' && lexpos < LEX_TOKE_BUF )
		{
			if ( ch == '\\' )
			{
				ch = getescape( lexp );
				if ( -1 == ch )
				{
					sprintf( lexp->lex, "%d", LEX_UNEXPECTED_ESCAPE );
					lexp->token = LEXERROR;
					return lexp->token;
				}
			}
			lexp->lex[lexpos++] = (char)ch;
		}
		if ( lexpos == LEX_TOKE_BUF || ch == -1 )
		{
			sprintf( lexp->lex, "%d", LEX_UNTERMINATED_STRING );
			lexp->token = LEXERROR;
			return lexp->token;
		}
		lexp->lex[lexpos] = '\0';
		lexp->token = STRING;
		return lexp->token;

	case '|':
		/*
		 * or
		 */
		ch = getch( lexp );
		if ( '|' == ch )
		{
			lexp->token = OR;
			return lexp->token;
		}
		if ( '=' == ch )
		{
			lexp->token = OREQ;
			return lexp->token;
		}
		ungetch( lexp, ch );
		lexp->token = PIPE;
		return lexp->token;

	case '&':
		/*
		 * and
		 */
		ch = getch( lexp );
		if ( '&' == ch )
		{
			lexp->token = AND;
			return lexp->token;
		}
		if ( '=' == ch )
		{
			lexp->token = ANDEQ;
			return lexp->token;
		}
		ungetch( lexp, ch );
		lexp->token = AMPR;
		return lexp->token;

	case '{':
		lexp->token = LBRACE;
		return lexp->token;

	case '}':
		lexp->token = RBRACE;
		return lexp->token;

	case '(':
		lexp->token = LPAR;
		return lexp->token;

	case ')':
		lexp->token = RPAR;
		return lexp->token;

	case '[':
		lexp->token = LBRAC;
		return lexp->token;

	case ']':
		lexp->token = RBRAC;
		return lexp->token;

	case '!':
		ch = getch( lexp );
		if ( '=' == ch )
		{
			lexp->token = ISNEQ;
			return lexp->token;
		}
		ungetch( lexp, ch );
		lexp->token = BANG;
		return lexp->token;

	case '=':
		ch = getch( lexp );
		if ( '=' == ch )
		{
			lexp->token = ISEQUAL;
			return lexp->token;
		}
		ungetch( lexp, ch );
		lexp->token = ASSIGN;
		return lexp->token;

	case '+':
		ch = getch( lexp );
		if ( '=' == ch )
		{
			lexp->token = PLUSEQ;
			return lexp->token;
		}
		if ( '+' == ch )
		{
			lexp->token = INC;
			return lexp->token;
		}
		ungetch( lexp, ch );
		lexp->token = PLUS;
		return lexp->token;

	case '-':
		ch = getch( lexp );
		if ( '=' == ch )
		{
			lexp->token = MINUSEQ;
			return lexp->token;
		}
		if ( '-' == ch )
		{
			lexp->token = DEC;
			return lexp->token;
		}
		ungetch( lexp, ch );
		lexp->token = MINUS;
		return lexp->token;

	case '*':
		ch = getch( lexp );
		if ( '=' == ch )
		{
			lexp->token = TIMESEQ;
			return lexp->token;
		}
		ungetch( lexp, ch );
		lexp->token = STAR;
		return lexp->token;

	case '/':
		ch = getch( lexp );
		if ( '=' == ch )
		{
			lexp->token = DIVEQ;
			return lexp->token;
		}
		if ( '/' == ch )
		{
			/*
			 * EOL comment
			 */
			while ( ch != '\n' && ch > 0 )
			{
				ch = getch( lexp );
			}
			lexp->token = jsppLex( lexp );
			return lexp->token;
		}
		if ( '*' == ch )
		{
			/*
			 *  Mult-line comment
			 */
			int opencount = 1;
			while ( opencount > 0 && ch > 0 )
			{
				while ( ch != '*' && ch != '/' && ch > 0 )
				{
					ch = getch( lexp );
				}
				if ( ch == '*' )
				{
					ch = getch( lexp );
					if ( ch == '/' )
					{
						opencount--;
					}
				}
				if ( ch == '/' )
				{
					ch = getch( lexp );
					if ( ch == '*' )
					{
						opencount++;
					}
				}
			}
			lexp->token = jsppLex( lexp );
			return lexp->token;
		}
		ungetch( lexp, ch );
		lexp->token = SLASH;
		return lexp->token;

	case '%':
		ch = getch( lexp );
		if ( '=' == ch )
		{
			lexp->token = MODEQ;
			return lexp->token;
		}
		ungetch( lexp, ch );
		lexp->token = MOD;
		return MOD;
	
	case '^':
		ch = getch( lexp );
		if ( '=' == ch )
		{
			lexp->token = XOREQ;
			return lexp->token;
		}
		ungetch( lexp, ch );
		lexp->token = XOR;
		return lexp->token;
	
	case '~':
		ch = getch( lexp );
		if ( '=' == ch )
		{
			lexp->token = COMPEQ;
			return lexp->token;
		}
		ungetch( lexp, ch );
		lexp->token = COMP;
		return lexp->token;

	case ':':
		ch = getch( lexp );
		if ( ':' == ch )
		{
			lexp->token = SCOPE;
			return lexp->token;
		}
		ungetch( lexp, ch );
		lexp->token = COLON;
		return lexp->token;

	case ';':
		lexp->token = SEMI;
		return lexp->token;

	case ',':
		lexp->token = COMMA;
		return lexp->token;

	case '<':
		ch = getch( lexp );
		if ( '=' == ch )
		{
			lexp->token = LTEQ;
			return lexp->token;
		}
		else if ( '<' == ch )
		{
			lexp->token = LSHIFT;
			return lexp->token;
		}
		ungetch( lexp, ch );
		lexp->token = LT;
		return lexp->token;

	case '>':
		ch = getch( lexp );
		if ( '=' == ch )
		{
			lexp->token = GTEQ;
			return lexp->token;
		}
		else if ( '>' == ch )
		{
			lexp->token = RSHIFT;
			return lexp->token;
		}
		ungetch( lexp, ch );
		lexp->token = GT;
		return lexp->token;

	case -1:
		lexp->token = JEOF;
		return lexp->token;

	default:
		sprintf( lexp->lex, "%d", LEX_ERROR );
		lexp->token = LEXERROR;
		return lexp->token;
	}
#ifdef _WIN32
	lexp->token = LEXERROR;		/* make certian compilers happy */
	return lexp->token;
#endif
}

void jspplexValidateMem( const jspplex *lex )
{
	ASSERT_MEM( lex, sizeof( jspplex ) );
	stringValidateMem( lex->buffer );
}

void jspplexCheckMem( const jspplex *lex )
{
	jspplexValidateMem( lex );
	DEBUG_NOTE_MEM_ALLOCATION( lex );
	stringCheckMem( lex->buffer );
}

