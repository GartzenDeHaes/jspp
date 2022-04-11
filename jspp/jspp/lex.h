#ifndef _lex_h
#define _lex_h

#ifdef _WIN32
#include <spl/configwin32.h>
#endif

#include <stdio.h>
#include <jspp/y.tab.h>
#include <spl/cstring.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LEX_TOKE_BUF 512

typedef struct jspplex
{
	string *buffer;
	int pos;
	int lineno;
	int token;
	char lex[LEX_TOKE_BUF];
} jspplex;

jspplex *jspplexConstructChar( const char *buffer );
jspplex *jspplexConstructBuf( const string *buffer );
jspplex *jspplexConstructFile( FILE *fp );
void jspplexDestruct( jspplex *lex );

int jsppLex( jspplex *lexp );

void jspplexValidateMem( const jspplex *lex );
void jspplexCheckMem( const jspplex *lex );

#ifdef __cplusplus
}
#endif

#endif
