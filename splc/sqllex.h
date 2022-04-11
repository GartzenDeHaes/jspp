#ifndef _sqllex_h
#define _sqllex_h

#include <spl/types.h>
#include <spl/debug.h>
#include <spl/string.h>
#include <spl/stringbuffer.h>

#include "sqltoken.h"

typedef struct _sqllex
{
	int m_token;
	string* m_lexum;
	int m_lineNum;
	int m_pos;				/* private */
	string* m_text;			/* private */
	stringbuffer *m_buf;	/* private */
} sqllex;

sqllex* sqllexNew(string* sql);
void sqllexDelete(sqllex* lex);

bool sqllexNext(sqllex* lex);
bool sqllexMatch(sqllex* lex, const char* cp);
bool sqllexMatchToken(sqllex* lex, const int tok);

#ifdef DEBUG
void sqllexValidateMem(sqllex* lex);
#else
#define sqllexValidateMem(l)
#endif

#endif
