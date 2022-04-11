#ifndef _sqlparser_h
#define _sqlparser_h

#ifdef _WIN32
#include <spl/configwin32.h>
#endif

#include <spl/types.h>
#include <spl/cstring.h>
#include <spl/strbuf.h>
#include <spl/cvector.h>
#include <spl/strbuf.h>
#include <spl/chashtable.h>
#include <hpsql/sqltypes.h>

#define PRMIDXLEN 10

typedef enum _sqltoken
{
	TOK_EOS = 0,
	TOK_CREATE = 1,
	TOK_PROC = 2,
	TOK_AS = 3,
	TOK_VAR = 4,
	TOK_ID = 5,
	TOK_DOT = 6,
	TOK_SELECT = 7,
	TOK_FROM = 8,
	TOK_SPLAT = 9,
	TOK_LPAR = 10,
	TOK_RPAR = 11,
	TOK_STRLIT = 12,
	TOK_EQ = 13,
	TOK_SEMI = 14,
	TOK_COMMA = 15,
	TOK_INT =16,
	TOK_SMALLINT = 17,
	TOK_BIGINT = 18,
	TOK_CHAR = 19,
	TOK_NUM = 20,
	TOK_WHERE = 21,
	TOK_AND = 22,
	TOK_INNER = 23,
	TOK_JOIN = 24,
	TOK_RIGHT = 25,
	TOK_LEFT = 26,
	TOK_ON = 27,
	TOK_OR = 28,
	TOK_INSERT = 29,
	TOK_INTO = 30,
	TOK_VALUES = 31,
	TOK_UPDATE = 32,
	TOK_SET = 33,
	TOK_ORDER = 34,
	TOK_BY = 35,
	TOK_DESC = 36,
	TOK_ASC = 37,
	TOK_IN = 38,
	TOK_FUNC = 39,
	TOK_DELETE = 40,
	TOK_GROUP = 41,
	TOK_HAVING = 42,
	TOK_GT = 43,
	TOK_LT = 44,
	TOK_GTEQ = 45,
	TOK_LTEQ = 46,
	TOK_NEQ = 47,
	TOK_LIKE = 48,
	TOK_IS = 49,
	TOK_NULL = 50,
	TOK_NOT = 51,
	TOK_UNION = 52,
	TOK_CASE = 53,
	TOK_WHEN = 54,
	TOK_THEN = 55,
	TOK_ELSE = 56,
	TOK_END = 57,
	TOK_DISTINCT = 58
} sqltoken;

typedef enum _sqlparseerror
{
	ERR_NONE = 0,
	ERR_SYNTAX = 1,
	ERR_OUT_OF_MEM = 2,
	ERR_EXP_CREATE = 3,
	ERR_EXP_PROC = 4,
	ERR_EXP_DOT = 5,
	ERR_EXP_RPAR = 6,
	ERR_EXP_LPAR = 7,
	ERR_EXP_AS = 7,
	ERR_EXP_FIELDS = 8,
	ERR_EXP_FROM = 9,
	ERR_EXP_TABLE = 10,
	ERR_EXP_SEMI = 11,
	ERR_EXP_TYPE = 12,
	ERR_EXP_LEN = 13,
	ERR_EXP_OP = 14,
	ERR_EXP_COND = 15,
	ERR_EXP_UNDEFPARAM = 16,
	ERR_EXP_DOT_FIELD = 17,
	ERR_EXP_JOIN = 18,
	ERR_EXP_ON = 19,
	ERR_EXP_INTO = 20,
	ERR_EXP_VALUES = 21,
	ERR_EXP_SET = 22,
	ERR_EXP_ORDERBY = 23,
	ERR_UNKNOWN_FN = 24,
	ERR_EXP_WHEN = 25,
	ERR_ONLY_ONE_ELSE = 26,
	ERR_EXP_THEN = 27,
	ERR_EXP_VALUE = 28,
	ERR_EXP_END = 29,
	ERR_UNEXP_RPAR = 30
} sqlparseerror;

extern char *sqlparseerrormsgs[];

typedef enum _sqlop
{
	SQLOP_SELECT,
	SQLOP_INSERT,
	SQLOP_UPDATE,
	SQLOP_DELETE
} sqlop;

typedef struct _sqlparam
{
	string *name;
	datatype type;
	int len;
	bool likepattern;
	int idxs[PRMIDXLEN];
} sqlparam;

typedef struct _sqldef
{
	string *sys;
	string *name;
	string *src;
	strbuf *tsql;
	int codepos;
	sqlop stmttype;
	vector *uparams;	/* SP Param list */
	vector *ufields;	/* SP Field Names */
	hashtable *uparamsLookup;
	vector *tparams;
	vector *tfields;
} sqldef;

sqldef *parseSQL( char *src, sqlparseerror *errcode, int *lineno );
void sqldefDestruct( sqldef *dp );

bool sqltokenizer( string *src, int *pos, int *linenum, sqltoken *token, strbuf *lexum );

#endif
