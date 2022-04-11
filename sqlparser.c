#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <spl/debug.h>
#include <hpsql/sqlparser.h>

static bool parseSelect
( 
	sqldef *dp, 
	int *pos, 
	int *lineno, 
	sqltoken *token, 
	strbuf *lexum, 
	sqlparseerror *errcode,
	bool ignoreTrailingRpar
);

void sqldefDestruct( sqldef *dp )
{
	int x;
	if ( NULL != dp->uparamsLookup )
	{
		hashtableDestruct( dp->uparamsLookup );
	}
	if ( dp->uparams != NULL )
	{
		int count = vectorCount( dp->uparams );
		for ( x = 0; x < count; x++ )
		{
			sqlparam *pp = (sqlparam *)vectorElementAt( dp->uparams, x );
			stringDestruct( pp->name );
			free( pp );
		}
		vectorDestruct( dp->uparams );
	}
	if ( dp->ufields != NULL )
	{
		int count = vectorCount( dp->ufields );
		for ( x = 0; x < count; x++ )
		{
			string *str = (string *)vectorElementAt( dp->ufields, x );
			stringDestruct( str );
		}
		vectorDestruct( dp->ufields );
	}
	if ( dp->tparams != NULL )
	{
		int count = vectorCount( dp->tparams );
		for ( x = 0; x < count; x++ )
		{
			sqlparam *pp = (sqlparam *)vectorElementAt( dp->tparams, x );
			stringDestruct( pp->name );
			free( pp );
		}
		vectorDestruct( dp->tparams );
	}
	if ( dp->tfields != NULL )
	{
		int count = vectorCount( dp->tfields );
		for ( x = 0; x < count; x++ )
		{
			sqlparam *pp = (sqlparam *)vectorElementAt( dp->tfields, x );
			stringDestruct( pp->name );
			free( pp );
		}
		vectorDestruct( dp->tfields );
	}
	if ( dp->name != NULL )
	{
		stringDestruct( dp->name );
	}
	if ( dp->src != NULL )
	{
		stringDestruct( dp->src );
	}
	if ( dp->sys != NULL )
	{
		stringDestruct( dp->sys );
	}
	if ( NULL != dp->tsql )
	{
		strbufDestruct(dp->tsql);
	}
	free( dp );
}

static bool parseLit
( 
	sqldef *dp, 
	int *pos, 
	int *lineno, 
	sqltoken *token, 
	strbuf *lexum, 
	sqlparseerror *errcode,
	bool inlike
)
{
	if ( TOK_STRLIT == *token )
	{
		if ( ! strbufAppendChar( dp->tsql, '\'' ) )
		{
			*errcode = ERR_OUT_OF_MEM;
			return false;
		}
		if ( ! strbufAppend( dp->tsql, (char *)lexum->array->data ) )
		{
			*errcode = ERR_OUT_OF_MEM;
			return false;
		}
		if ( inlike )
		{
			if ( ! strbufAppendChar( dp->tsql, '`' ) )
			{
				*errcode = ERR_OUT_OF_MEM;
				return false;
			}
		}
		if ( ! strbufAppendChar( dp->tsql, '\'' ) )
		{
			*errcode = ERR_OUT_OF_MEM;
			return false;
		}
		if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
		{
			*errcode = ERR_SYNTAX;
			return false;
		}
	}
	else if ( TOK_NUM == *token )
	{
		if ( ! strbufAppend( dp->tsql, (char *)lexum->array->data ) )
		{
			*errcode = ERR_OUT_OF_MEM;
			return false;
		}
		if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
		{
			*errcode = ERR_SYNTAX;
			return false;
		}
	}
	else
	{
		return false;
	}
	return true;
}

static bool parseParamList
( 
	sqldef *dp, 
	int *pos, 
	int *lineno, 
	sqltoken *token, 
	strbuf *lexum, 
	sqlparseerror *errcode 
)
{
	bool expectRPAR = false;
	int x;

	if ( NULL == (dp->uparams = vectorConstruct( sizeof(sqlparam) )) )
	{
		*errcode = ERR_OUT_OF_MEM;
		return false;
	}
	if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
	{
		*errcode = ERR_SYNTAX;
		return false;
	}
	if ( TOK_LPAR == *token )
	{
		expectRPAR = true;
		if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
		{
			*errcode = ERR_SYNTAX;
			return false;
		}
	}

	/* read params */
	while ( TOK_VAR == *token )
	{
		sqlparam *pp = (sqlparam *)malloc( sizeof(sqlparam) );
		if ( NULL == pp )
		{
			*errcode = ERR_OUT_OF_MEM;
			return false;
		}
		for ( x = 0; x < PRMIDXLEN; x++ )
		{
			pp->idxs[x] = -1;
		}
		pp->name = NULL;
		pp->likepattern = false;
		if ( ! vectorAdd( dp->uparams, &pp ) )
		{
			*errcode = ERR_OUT_OF_MEM;
			return false;
		}
		if ( NULL == (pp->name = strbufToString(lexum)) )
		{
			*errcode = ERR_OUT_OF_MEM;
			return false;
		}
		if ( ! hashtableAdd( dp->uparamsLookup, pp->name, pp ) )
		{
			*errcode = ERR_OUT_OF_MEM;
			return false;
		}
		if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
		{
			*errcode = ERR_SYNTAX;
			return false;
		}
		if ( TOK_INT == *token )
		{
			pp->type = TYPE_INT;
			pp->len = 4;
		}
		else if ( TOK_SMALLINT == *token )
		{
			pp->type = TYPE_SHORT;
			pp->len = 2;
		}
		else if ( TOK_BIGINT == *token )
		{
			pp->type = TYPE_LONG;
			pp->len = 8;
		}
		else if ( TOK_CHAR == *token )
		{
			pp->type = TYPE_CHAR;
			if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
			{
				*errcode = ERR_SYNTAX;
				return false;
			}
			if ( TOK_LPAR != *token )
			{
				*errcode = ERR_EXP_LPAR;
				return false;
			}
			if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
			{
				*errcode = ERR_SYNTAX;
				return false;
			}
			if ( TOK_NUM != *token )
			{
				*errcode = ERR_EXP_LEN;
				return false;
			}
			pp->len = atoi((char *)lexum->array->data);
			if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
			{
				*errcode = ERR_SYNTAX;
				return false;
			}
			if ( TOK_RPAR != *token )
			{
				*errcode = ERR_EXP_RPAR;
				return false;
			}
		}
		else
		{
			*errcode = ERR_EXP_TYPE;
			return false;
		}
		if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
		{
			*errcode = ERR_SYNTAX;
			return false;
		}

		if ( TOK_COMMA == *token )
		{
			if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
			{
				*errcode = ERR_SYNTAX;
				return false;
			}
		}
	}

	/* close */
	if ( expectRPAR )
	{
		if ( TOK_RPAR != *token )
		{
			*errcode = ERR_EXP_RPAR;
			return false;
		}
		if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
		{
			*errcode = ERR_SYNTAX;
			return false;
		}
	}
	if ( TOK_AS != *token )
	{
		*errcode = ERR_EXP_AS;
		return false;
	}
	if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
	{
		*errcode = ERR_SYNTAX;
		return false;
	}
	return true;
}

static bool parseField
( 
	sqldef *dp, 
	int *pos, 
	int *lineno, 
	sqltoken *token, 
	strbuf *lexum, 
	sqlparseerror *errcode,
	bool inselect
)
{
	string *fieldname;

	if ( TOK_ID != *token )
	{
		return false;
	}
	if ( inselect )
	{
		if ( NULL == (fieldname = stringConstructChar((char *)lexum->array->data)) )
		{
			*errcode = ERR_OUT_OF_MEM;
			return false;
		}
		if ( ! vectorAdd(dp->ufields, &fieldname) )
		{
			*errcode = ERR_OUT_OF_MEM;
			return false;
		}
	}
	if ( ! strbufAppend( dp->tsql, (char *)lexum->array->data ) )
	{
		*errcode = ERR_OUT_OF_MEM;
		return false;
	}
	if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
	{
		*errcode = ERR_SYNTAX;
		return false;
	}
	if ( TOK_DOT == *token )
	{
		if ( ! strbufAppendChar( dp->tsql, '.' ) )
		{
			*errcode = ERR_OUT_OF_MEM;
			return false;
		}
		if ( inselect )
		{
			if ( ! stringAppendChar(fieldname, ".") )
			{
				*errcode = ERR_OUT_OF_MEM;
				return false;
			}
		}
		if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
		{
			*errcode = ERR_SYNTAX;
			return false;
		}
		if ( inselect )
		{
			if ( ! stringAppendChar( fieldname, (char *)lexum->array->data ) )
			{
				*errcode = ERR_OUT_OF_MEM;
				return false;
			}
		}
		if ( TOK_ID != *token )
		{
			*errcode = ERR_EXP_DOT_FIELD;
			return false;
		}
		if ( ! strbufAppend( dp->tsql, (char *)lexum->array->data ) )
		{
			*errcode = ERR_OUT_OF_MEM;
			return false;
		}
		if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
		{
			*errcode = ERR_SYNTAX;
			return false;
		}
	}
	return true;
}

static bool parseFunction
( 
	sqldef *dp, 
	int *pos, 
	int *lineno, 
	sqltoken *token, 
	strbuf *lexum, 
	sqlparseerror *errcode 
)
{
	if ( TOK_FUNC != *token )
	{
		*errcode = ERR_SYNTAX;
		return false;
	}
	ucase((char *)lexum->array->data);
	if ( strcmp( (char *)lexum->array->data, "GETDATE") == 0 )
	{
		if ( ! strbufAppend( dp->tsql, "CURRENT" ) )
		{
			*errcode = ERR_OUT_OF_MEM;
			return false;
		}
		if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
		{
			*errcode = ERR_SYNTAX;
			return false;
		}
		if ( TOK_LPAR != *token )
		{
			*errcode = ERR_EXP_LPAR;
			return false;
		}
	}
	else if ( strcmp( (char *)lexum->array->data, "COUNT") == 0 )
	{
		if ( ! strbufAppend( dp->tsql, "COUNT(*)" ) )
		{
			*errcode = ERR_OUT_OF_MEM;
			return false;
		}
		if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
		{
			*errcode = ERR_SYNTAX;
			return false;
		}
		if ( TOK_LPAR != *token )
		{
			*errcode = ERR_EXP_LPAR;
			return false;
		}
		if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
		{
			*errcode = ERR_SYNTAX;
			return false;
		}
		if ( TOK_SPLAT != *token )
		{
			*errcode = ERR_EXP_FIELDS;
			return false;
		}
	}
	else if ( strcmp( (char *)lexum->array->data, "AVG") == 0 ||
			strcmp( (char *)lexum->array->data, "MAX") == 0 ||
			strcmp( (char *)lexum->array->data, "MIN") == 0 ||
			strcmp( (char *)lexum->array->data, "SUM") == 0
			)
	{
		if ( ! strbufAppend( dp->tsql, (char *)lexum->array->data ) )
		{
			*errcode = ERR_OUT_OF_MEM;
			return false;
		}
		if ( ! strbufAppendChar( dp->tsql, '(' ) )
		{
			*errcode = ERR_OUT_OF_MEM;
			return false;
		}
		if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
		{
			*errcode = ERR_SYNTAX;
			return false;
		}
		if ( TOK_LPAR != *token )
		{
			*errcode = ERR_EXP_LPAR;
			return false;
		}
		if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
		{
			*errcode = ERR_SYNTAX;
			return false;
		}
		if ( ! parseField(dp, pos, lineno, token, lexum, errcode, false) )
		{
			return false;
		}
		if ( ! strbufAppendChar( dp->tsql, ')' ) )
		{
			*errcode = ERR_OUT_OF_MEM;
			return false;
		}
	}
	else
	{
		*errcode = ERR_UNKNOWN_FN;
		return false;
	}
	if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
	{
		*errcode = ERR_SYNTAX;
		return false;
	}
	if ( TOK_RPAR != *token )
	{
		*errcode = ERR_EXP_RPAR;
		return false;
	}
	if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
	{
		*errcode = ERR_SYNTAX;
		return false;
	}
	return true;
}

static bool parseFieldFnList
( 
	sqldef *dp, 
	int *pos, 
	int *lineno, 
	sqltoken *token, 
	strbuf *lexum, 
	sqlparseerror *errcode 
)
{
	if ( TOK_ID != *token || TOK_FUNC != *token )
	{
		*errcode = ERR_EXP_FIELDS;
		return false;
	}
	while ( TOK_ID == *token || TOK_FUNC == *token )
	{
		if ( TOK_ID == *token )
		{
			if ( ! parseField(dp, pos, lineno, token, lexum, errcode, false) )
			{
				return false;
			}
		}
		else if ( TOK_FUNC == *token )
		{
			if ( ! parseFunction(dp, pos, lineno, token, lexum, errcode) )
			{
				return false;
			}
		}
		if ( TOK_COMMA == *token )
		{
			if ( ! strbufAppendChar( dp->tsql, ',' ) )
			{
				*errcode = ERR_OUT_OF_MEM;
				return false;
			}
			if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
			{
				*errcode = ERR_SYNTAX;
				return false;
			}
			if ( TOK_ID != *token || TOK_FUNC != *token )
			{
				*errcode = ERR_EXP_FIELDS;
				return false;
			}			
		}
	}
	return true;
}

static bool parseGroupBy
( 
	sqldef *dp, 
	int *pos, 
	int *lineno, 
	sqltoken *token, 
	strbuf *lexum, 
	sqlparseerror *errcode 
)
{
	if ( TOK_GROUP != *token )
	{
		return true;
	}
	if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
	{
		*errcode = ERR_SYNTAX;
		return false;
	}
	if ( TOK_BY != *token )
	{
		*errcode = ERR_EXP_ORDERBY;
		return false;
	}
	if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
	{
		*errcode = ERR_SYNTAX;
		return false;
	}
	if ( ! strbufAppend( dp->tsql, " GROUP BY " ) )
	{
		*errcode = ERR_OUT_OF_MEM;
		return false;
	}
	return parseFieldFnList( dp, pos, lineno, token, lexum, errcode );
}

static bool parseOrderBy
( 
	sqldef *dp, 
	int *pos, 
	int *lineno, 
	sqltoken *token, 
	strbuf *lexum, 
	sqlparseerror *errcode 
)
{
	if ( TOK_ORDER != *token )
	{
		return true;
	}
	if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
	{
		*errcode = ERR_SYNTAX;
		return false;
	}
	if ( TOK_BY != *token )
	{
		*errcode = ERR_EXP_ORDERBY;
		return false;
	}
	if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
	{
		*errcode = ERR_SYNTAX;
		return false;
	}
	if ( ! strbufAppend( dp->tsql, " ORDER BY " ) )
	{
		*errcode = ERR_OUT_OF_MEM;
		return false;
	}
	if ( TOK_ID != *token )
	{
		*errcode = ERR_EXP_FIELDS;
		return false;
	}
	while ( TOK_ID == *token || TOK_FUNC == *token )
	{
		if ( TOK_ID == *token )
		{
			if ( ! parseField(dp, pos, lineno, token, lexum, errcode, false) )
			{
				return false;
			}
		}
		else
		{
			if ( ! parseFunction(dp, pos, lineno, token, lexum, errcode) )
			{
				return false;
			}
		}
		if ( TOK_DESC == *token )
		{
			if ( ! strbufAppend( dp->tsql, " DESC " ) )
			{
				*errcode = ERR_OUT_OF_MEM;
				return false;
			}
			if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
			{
				*errcode = ERR_SYNTAX;
				return false;
			}
		}
		else if ( TOK_ASC == *token )
		{
			if ( ! strbufAppend( dp->tsql, " ASC " ) )
			{
				*errcode = ERR_OUT_OF_MEM;
				return false;
			}
			if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
			{
				*errcode = ERR_SYNTAX;
				return false;
			}
		}
		else
		{
			if ( ! strbufAppend( dp->tsql, " DESC " ) )
			{
				*errcode = ERR_OUT_OF_MEM;
				return false;
			}
		}
		if ( TOK_COMMA == *token )
		{
			if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
			{
				*errcode = ERR_SYNTAX;
				return false;
			}
			if ( ! strbufAppendChar( dp->tsql, ',' ) )
			{
				*errcode = ERR_OUT_OF_MEM;
				return false;
			}
			if ( TOK_ID != *token && TOK_FUNC != *token )
			{
				*errcode = ERR_EXP_FIELDS;
				return false;
			}
		}
	}
	return true;
}

static bool parseInList
( 
	sqldef *dp, 
	int *pos, 
	int *lineno, 
	sqltoken *token, 
	strbuf *lexum, 
	sqlparseerror *errcode 
)
{
	if ( TOK_IN != *token )
	{
		return true;
	}
	if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
	{
		*errcode = ERR_SYNTAX;
		return false;
	}
	if ( TOK_LPAR != *token )
	{
		*errcode = ERR_EXP_LPAR;
		return false;
	}
	if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
	{
		*errcode = ERR_SYNTAX;
		return false;
	}
	if ( ! strbufAppend( dp->tsql, " IN (" ) )
	{
		*errcode = ERR_OUT_OF_MEM;
		return false;
	}
	if ( TOK_SELECT == *token )
	{
		if ( ! parseSelect( dp, pos, lineno, token, lexum, errcode, true ) )
		{
			return false;
		}
	}
	else
	{
		while( TOK_NUM == *token || TOK_STRLIT == *token )
		{
			if ( ! parseLit( dp, pos, lineno, token, lexum, errcode, false ) )
			{
				return false;
			}
			if ( TOK_COMMA == *token )
			{
				if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
				{
					*errcode = ERR_SYNTAX;
					return false;
				}
				if ( ! strbufAppendChar( dp->tsql, ',' ) )
				{
					*errcode = ERR_OUT_OF_MEM;
					return false;
				}
				if ( ! (TOK_NUM == *token || TOK_STRLIT == *token) )
				{
					*errcode = ERR_SYNTAX;
					return false;
				}
			}
		}
	}
	if ( TOK_RPAR != *token )
	{
		*errcode = ERR_EXP_RPAR;
		return false;
	}
	if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
	{
		*errcode = ERR_SYNTAX;
		return false;
	}
	if ( ! strbufAppendChar( dp->tsql, ')' ) )
	{
		*errcode = ERR_OUT_OF_MEM;
		return false;
	}

	return true;
}

static bool setPrmIdx( sqlparam *prm, int *prmidxpos )
{
	int x;
	for ( x = 0; x < PRMIDXLEN; x++ )
	{
		if ( prm->idxs[x] == -1 )
		{
			prm->idxs[x] = *prmidxpos;
			(*prmidxpos)++;
			return true;
		}
	}
	return false;
}

static bool parseConditions
( 
	sqldef *dp, 
	int *pos, 
	int *lineno, 
	sqltoken *token, 
	strbuf *lexum, 
	sqlparseerror *errcode,
	int *prmidxpos,
	bool allowFunctions,
	bool ignoreTrailingRpar
);

static bool parseCase
( 
	sqldef *dp, 
	int *pos, 
	int *lineno, 
	sqltoken *token, 
	strbuf *lexum, 
	sqlparseerror *errcode,
	int *prmidxpos,
	bool allowFunctions
)
{
	int elsecount = 0;

	ASSERT( TOK_CASE == *token );
	
	if ( ! strbufAppend( dp->tsql, " CASE " ) )
	{
		*errcode = ERR_OUT_OF_MEM;
		return false;
	}
	if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
	{
		*errcode = ERR_SYNTAX;
		return false;
	}
	if ( TOK_WHEN != *token )
	{
		*errcode = ERR_EXP_WHEN;
		return false;
	}
	while ( TOK_WHEN == *token || TOK_ELSE == *token )
	{
		if ( ! strbufAppendChar( dp->tsql, ' ' ) )
		{
			*errcode = ERR_OUT_OF_MEM;
			return false;
		}
		if ( ! strbufAppend( dp->tsql, (char *)lexum->array->data ) )
		{
			*errcode = ERR_OUT_OF_MEM;
			return false;
		}
		if ( TOK_ELSE == *token )
		{
			if ( ++elsecount > 1 )
			{
				*errcode = ERR_ONLY_ONE_ELSE;
				return false;
			}
			if ( ! strbufAppendChar( dp->tsql, ' ' ) )
			{
				*errcode = ERR_OUT_OF_MEM;
				return false;
			}
			if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
			{
				*errcode = ERR_SYNTAX;
				return false;
			}
			if ( ! parseLit( dp, pos, lineno, token, lexum, errcode, false ) )
			{
				return false;
			}
			continue;
		}
		if ( ! strbufAppendChar( dp->tsql, ' ' ) )
		{
			*errcode = ERR_OUT_OF_MEM;
			return false;
		}
		if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
		{
			*errcode = ERR_SYNTAX;
			return false;
		}
		if ( ! parseConditions(dp, pos, lineno, token, lexum, errcode, prmidxpos, false, false) )
		{
			return false;
		}
		if ( TOK_THEN != *token )
		{
			*errcode = ERR_EXP_THEN;
			return false;
		}
		if ( ! strbufAppend( dp->tsql, " THEN " ) )
		{
			*errcode = ERR_OUT_OF_MEM;
			return false;
		}
		if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
		{
			*errcode = ERR_SYNTAX;
			return false;
		}
		if ( TOK_STRLIT == *token || TOK_NUM == *token )
		{
			if ( ! parseLit( dp, pos, lineno, token, lexum, errcode, false ) )
			{
				return false;
			}
		}
		else if ( TOK_FUNC == *token )
		{
			if ( ! parseFunction(dp, pos, lineno, token, lexum, errcode) )
			{
				return false;
			}
		}
		else
		{
			*errcode = ERR_EXP_VALUE;
			return false;
		}
	}
	if ( TOK_END != *token )
	{
		*errcode = ERR_EXP_END;
		return false;
	}
	if ( ! strbufAppend( dp->tsql, " END " ) )
	{
		*errcode = ERR_OUT_OF_MEM;
		return false;
	}
	if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
	{
		*errcode = ERR_SYNTAX;
		return false;
	}
	return true;
}
	
static bool parseConditions
( 
	sqldef *dp, 
	int *pos, 
	int *lineno, 
	sqltoken *token, 
	strbuf *lexum, 
	sqlparseerror *errcode,
	int *prmidxpos,
	bool allowFunctions,
	bool ignoreTrailingRpar
)
{
	int paradepth = 0;
	bool inlike;
	sqltoken expect = TOK_EOS;

	while ( TOK_ID == *token || TOK_LPAR == *token )
	{
		inlike = false;
		if ( TOK_LPAR == *token )
		{
			if ( ! strbufAppendChar( dp->tsql, '(' ) )
			{
				*errcode = ERR_OUT_OF_MEM;
				return false;
			}
			if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
			{
				*errcode = ERR_SYNTAX;
				return false;
			}
			paradepth++;
		}
		if ( ! parseField( dp, pos, lineno, token, lexum, errcode, false ) )
		{
			return false;
		}
		switch ( *token )
		{
		case TOK_EQ:
			if ( ! strbufAppend( dp->tsql, " = " ) )
			{
				*errcode = ERR_OUT_OF_MEM;
				return false;
			}
			break;
		case TOK_GT:
			if ( ! strbufAppend( dp->tsql, " > " ) )
			{
				*errcode = ERR_OUT_OF_MEM;
				return false;
			}
			break;
		case TOK_LT:
			if ( ! strbufAppend( dp->tsql, " < " ) )
			{
				*errcode = ERR_OUT_OF_MEM;
				return false;
			}
			break;
		case TOK_GTEQ:
			if ( ! strbufAppend( dp->tsql, " >= " ) )
			{
				*errcode = ERR_OUT_OF_MEM;
				return false;
			}
			break;
		case TOK_LTEQ:
			if ( ! strbufAppend( dp->tsql, " <= " ) )
			{
				*errcode = ERR_OUT_OF_MEM;
				return false;
			}
			break;
		case TOK_NEQ:
			if ( ! strbufAppend( dp->tsql, " <> " ) )
			{
				*errcode = ERR_OUT_OF_MEM;
				return false;
			}
			break;
		case TOK_LIKE:
			if ( ! strbufAppend( dp->tsql, " LIKE " ) )
			{
				*errcode = ERR_OUT_OF_MEM;
				return false;
			}
			inlike = true;
			break;
		case TOK_IS:
			if ( ! strbufAppend( dp->tsql, " IS " ) )
			{
				*errcode = ERR_OUT_OF_MEM;
				return false;
			}
			expect = TOK_NULL;
			break;
		case TOK_NOT:
		case TOK_IN:
			if ( TOK_NOT == *token )
			{
				if ( ! strbufAppend( dp->tsql, " NOT " ) )
				{
					*errcode = ERR_OUT_OF_MEM;
					return false;
				}
				if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
				{
					*errcode = ERR_SYNTAX;
					return false;
				}
			}
			if ( ! parseInList(dp, pos, lineno, token, lexum, errcode) )
			{
				return false;
			}
			continue;
		default:
			*errcode = ERR_EXP_OP;
			return false;
		}
		if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
		{
			*errcode = ERR_SYNTAX;
			return false;
		}
		if ( TOK_EOS != expect )
		{
			if ( *token != expect )
			{
				if ( TOK_NULL == expect && TOK_NOT == *token )
				{
					if ( ! strbufAppend( dp->tsql, "NOT " ) )
					{
						*errcode = ERR_OUT_OF_MEM;
						return false;
					}
					if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
					{
						*errcode = ERR_SYNTAX;
						return false;
					}
				}
				else
				{
					*errcode = ERR_SYNTAX;
					return false;
				}
			}
		}
		if ( TOK_VAR == *token )
		{
			sqlparam *prm = (sqlparam *)hashtableGetChar(dp->uparamsLookup, (char *)lexum->array->data, (int)strlen((char *)lexum->array->data));
			if ( NULL == prm )
			{
				*errcode = ERR_EXP_UNDEFPARAM;
				return false;
			}
			if ( inlike )
			{
				prm->likepattern = true;
			}
			if ( ! setPrmIdx( prm, prmidxpos ) )
			{
				*errcode = ERR_OUT_OF_MEM;
				return false;
			}
			if ( ! strbufAppend( dp->tsql, ":p" ) )
			{
				*errcode = ERR_OUT_OF_MEM;
				return false;
			}
			if ( ! strbufAppend( dp->tsql, &((char *)lexum->array->data)[1] ) )
			{
				*errcode = ERR_OUT_OF_MEM;
				return false;
			}
			if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
			{
				*errcode = ERR_SYNTAX;
				return false;
			}
		}
		else if ( TOK_STRLIT == *token || TOK_NUM == *token )
		{
			if ( ! parseLit( dp, pos, lineno, token, lexum, errcode, inlike ) )
			{
				return false;
			}
		}
		else if ( TOK_ID == *token )
		{
			if ( ! parseField( dp, pos, lineno, token, lexum, errcode, false ) )
			{
				return false;
			}
		}
		else if ( TOK_FUNC == *token && allowFunctions )
		{
			if ( ! parseFunction(dp, pos, lineno, token, lexum, errcode) )
			{
				return false;
			}
		}
		else if ( TOK_NULL == *token )
		{
			if ( ! strbufAppend( dp->tsql, " NULL " ) )
			{
				*errcode = ERR_OUT_OF_MEM;
				return false;
			}
			if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
			{
				*errcode = ERR_SYNTAX;
				return false;
			}
		}
		else
		{
			*errcode = ERR_EXP_COND;
			return false;
		}
		if ( inlike )
		{
			if ( ! strbufAppend( dp->tsql, " TERMINATE \"`\" " ) )
			{
				*errcode = ERR_OUT_OF_MEM;
				return false;
			}
		}
		if ( ! strbufAppendChar( dp->tsql, ' ' ) )
		{
			*errcode = ERR_OUT_OF_MEM;
			return false;
		}
		if ( TOK_RPAR == *token )
		{
			if ( paradepth <= 0 )
			{
				if ( ! (paradepth == 0 && ignoreTrailingRpar) )
				{
					*errcode = ERR_UNEXP_RPAR;
					return false;
				}
				return true;
			}
			paradepth--;
			if ( ! strbufAppend( dp->tsql, ") " ) )
			{
				*errcode = ERR_OUT_OF_MEM;
				return false;
			}
			if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
			{
				*errcode = ERR_SYNTAX;
				return false;
			}
		}
		/* logic ops */
		switch ( *token )
		{
		case TOK_AND:
		case TOK_OR:
			if ( ! strbufAppend( dp->tsql, (char *)lexum->array->data ) )
			{
				*errcode = ERR_OUT_OF_MEM;
				return false;
			}
			break;
		default:
			/* no more where statements */
			return true;
		}
		if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
		{
			*errcode = ERR_SYNTAX;
			return false;
		}
		if ( ! strbufAppendChar( dp->tsql, ' ' ) )
		{
			*errcode = ERR_OUT_OF_MEM;
			return false;
		}
	}
	if ( paradepth != 0 )
	{
		*errcode = ERR_EXP_RPAR;
		return false;
	}
	return true;
}

static bool parseWhere
( 
	sqldef *dp, 
	int *pos, 
	int *lineno, 
	sqltoken *token, 
	strbuf *lexum, 
	sqlparseerror *errcode,
	int *prmidxpos,
	bool ignoreTrailingRpar
)
{
	if ( TOK_WHERE != *token )
	{
		return true;
	}
	if ( ! strbufAppend( dp->tsql, " WHERE " ) )
	{
		*errcode = ERR_OUT_OF_MEM;
		return false;
	}
	if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
	{
		*errcode = ERR_SYNTAX;
		return false;
	}

	return parseConditions(dp, pos, lineno, token, lexum, errcode, prmidxpos, false, ignoreTrailingRpar);
}

static bool parseHaving
( 
	sqldef *dp, 
	int *pos, 
	int *lineno, 
	sqltoken *token, 
	strbuf *lexum, 
	sqlparseerror *errcode,
	int *prmidxpos
)
{
	if ( TOK_HAVING != *token )
	{
		return true;
	}
	if ( ! strbufAppend( dp->tsql, " HAVING " ) )
	{
		*errcode = ERR_OUT_OF_MEM;
		return false;
	}
	if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
	{
		*errcode = ERR_SYNTAX;
		return false;
	}

	return parseConditions(dp, pos, lineno, token, lexum, errcode, prmidxpos, true, false);
}

static bool parseJoinLhs
( 
	sqldef *dp, 
	int *pos, 
	int *lineno, 
	sqltoken *token, 
	strbuf *lexum, 
	sqlparseerror *errcode,
	int *prmidxpos
)
{
	if ( ! strbufAppend( dp->tsql, (char *)lexum->array->data ) )
	{
		*errcode = ERR_OUT_OF_MEM;
		return false;
	}
	if ( ! strbufAppend( dp->tsql, " JOIN " ) )
	{
		*errcode = ERR_OUT_OF_MEM;
		return false;
	}
	if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
	{
		*errcode = ERR_SYNTAX;
		return false;
	}
	if ( TOK_JOIN != *token )
	{
		*errcode = ERR_EXP_JOIN;
		return false;
	}
	if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
	{
		*errcode = ERR_SYNTAX;
		return false;
	}
	if ( TOK_ID != *token )
	{
		*errcode = ERR_EXP_TABLE;
		return false;
	}
	if ( '=' != ((char *)lexum->array->data)[0] )
	{
		strbufAppendChar( dp->tsql, '=' );
	}
	if ( ! strbufAppend( dp->tsql, (char *)lexum->array->data ) )
	{
		*errcode = ERR_OUT_OF_MEM;
		return false;
	}
	/* alias */
	if ( ! strbufAppendChar( dp->tsql, ' ' ) )
	{
		*errcode = ERR_OUT_OF_MEM;
		return false;
	}
	if ( '=' == ((char *)lexum->array->data)[0] )
	{
		if ( ! strbufAppend( dp->tsql, &((char *)lexum->array->data)[1] ) )
		{
			*errcode = ERR_OUT_OF_MEM;
			return false;
		}
	}
	else
	{
		if ( ! strbufAppend( dp->tsql, (char *)lexum->array->data ) )
		{
			*errcode = ERR_OUT_OF_MEM;
			return false;
		}
	}
	if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
	{
		*errcode = ERR_SYNTAX;
		return false;
	}
	if ( TOK_ON != *token )
	{
		*errcode = ERR_EXP_ON;
		return false;
	}
	if ( ! strbufAppend( dp->tsql, " ON (" ) )
	{
		*errcode = ERR_OUT_OF_MEM;
		return false;
	}
	if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
	{
		*errcode = ERR_SYNTAX;
		return false;
	}
	if ( ! parseConditions(dp, pos, lineno, token, lexum, errcode, prmidxpos, false, false) )
	{
		return false;
	}
	if ( ! strbufAppend( dp->tsql, ") " ) )
	{
		*errcode = ERR_OUT_OF_MEM;
		return false;
	}
	return true;
}

static bool parseFrom
( 
	sqldef *dp, 
	int *pos, 
	int *lineno, 
	sqltoken *token, 
	strbuf *lexum, 
	sqlparseerror *errcode,
	int *prmidxpos
)
{
	if ( ! strbufAppend( dp->tsql, "FROM " ) )
	{
		*errcode = ERR_OUT_OF_MEM;
		return false;
	}
	if ( TOK_FROM != *token )
	{
		*errcode = ERR_EXP_FROM;
		return false;
	}
	if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
	{
		*errcode = ERR_SYNTAX;
		return false;
	}
	if ( TOK_ID != *token )
	{
		*errcode = ERR_EXP_TABLE;
		return false;
	}
	while ( TOK_ID == *token )
	{
		if (  ((char *)lexum->array->data)[0] != '=' )
		{
			/* add = for the define, only defines are allowed */
			if ( ! strbufAppendChar( dp->tsql, '=' ) )
			{
				*errcode = ERR_OUT_OF_MEM;
				return false;
			}
		}
		if ( ! strbufAppend( dp->tsql, (char *)lexum->array->data ) )
		{
			*errcode = ERR_OUT_OF_MEM;
			return false;
		}
		if ( ! strbufAppendChar( dp->tsql, ' ' ) )
		{
			*errcode = ERR_OUT_OF_MEM;
			return false;
		}
		/* add a table alias for the define */
		if (  ((char *)lexum->array->data)[0] == '=' )
		{
			if ( ! strbufAppend( dp->tsql, &((char *)lexum->array->data)[1] ) )
			{
				*errcode = ERR_OUT_OF_MEM;
				return false;
			}
		}
		else
		{
			if ( ! strbufAppend( dp->tsql, (char *)lexum->array->data ) )
			{
				*errcode = ERR_OUT_OF_MEM;
				return false;
			}
		}
		if ( ! strbufAppendChar( dp->tsql, ' ' ) )
		{
			*errcode = ERR_OUT_OF_MEM;
			return false;
		}
		/* end alias */
		if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
		{
			*errcode = ERR_SYNTAX;
			return false;
		}
		if ( TOK_COMMA == *token )
		{
			/* old join */
			if ( ! strbufAppendChar( dp->tsql, ',' ) )
			{
				*errcode = ERR_OUT_OF_MEM;
				return false;
			}
			if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
			{
				*errcode = ERR_SYNTAX;
				return false;
			}
			if ( TOK_ID != *token )
			{
				*errcode = ERR_EXP_TABLE;
				return false;
			}
		}
		else if ( TOK_INNER == *token || TOK_LEFT == *token || TOK_RIGHT == *token )
		{
			if ( ! parseJoinLhs(dp, pos, lineno, token, lexum, errcode, prmidxpos) )
			{
				return false;
			}
		}
		else
		{
			break;
		}
	}
	while ( TOK_INNER == *token || TOK_LEFT == *token || TOK_RIGHT == *token )
	{
		if ( ! parseJoinLhs(dp, pos, lineno, token, lexum, errcode, prmidxpos) )
		{
			return false;
		}
	}
	return true;
}

static bool parseSelect
( 
	sqldef *dp, 
	int *pos, 
	int *lineno, 
	sqltoken *token, 
	strbuf *lexum, 
	sqlparseerror *errcode,
	bool ignoreTrailingRpar
)
{
	bool addedfieldname;
	int prmidxpos = 0;
	dp->stmttype = SQLOP_SELECT;
	
	if ( ! strbufAppend(dp->tsql, "SELECT ") )
	{
		*errcode = ERR_OUT_OF_MEM;
		return false;
	}
	if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
	{
		*errcode = ERR_SYNTAX;
		return false;
	}
	/* DISTINCT */
	if ( TOK_DISTINCT == *token )
	{
		if ( ! strbufAppend(dp->tsql, "DISTINCT ") )
		{
			*errcode = ERR_OUT_OF_MEM;
			return false;
		}
		if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
		{
			*errcode = ERR_SYNTAX;
			return false;
		}
	}
	/* 1. field list or splat */
	if ( TOK_SPLAT != *token && TOK_ID != *token && TOK_FUNC != *token )
	{
		*errcode = ERR_EXP_FIELDS;
		return false;		
	}
	if ( TOK_SPLAT == *token )
	{
		if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
		{
			*errcode = ERR_SYNTAX;
			return false;
		}
		if ( ! strbufAppend(dp->tsql, "* ") )
		{
			*errcode = ERR_OUT_OF_MEM;
			return false;
		}
	}
	else if ( TOK_ID == *token || TOK_FUNC == *token )
	{
		/* field list */
		while ( TOK_ID == *token || TOK_FUNC == *token || TOK_STRLIT == *token || TOK_NUM == *token || TOK_CASE == *token )
		{
			addedfieldname = false;
			if ( TOK_FUNC == *token )
			{
				if ( ! parseFunction(dp, pos, lineno, token, lexum, errcode) )
				{
					return false;
				}
			}
			else if ( TOK_CASE == *token )
			{
				if ( ! parseCase( dp, pos, lineno, token, lexum, errcode, &prmidxpos, false ) )
				{
					return false;
				}
			}
			else if ( TOK_STRLIT == *token )
			{
				if ( ! strbufAppendChar( dp->tsql, '\'' ) )
				{
					*errcode = ERR_OUT_OF_MEM;
					return false;
				}
				if ( ! strbufAppend( dp->tsql, (char *)lexum->array->data ) )
				{
					*errcode = ERR_OUT_OF_MEM;
					return false;
				}
				if ( ! strbufAppendChar( dp->tsql, '\'' ) )
				{
					*errcode = ERR_OUT_OF_MEM;
					return false;
				}
				if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
				{
					*errcode = ERR_SYNTAX;
					return false;
				}
			}
			else
			{
				if ( TOK_ID == *token )
				{
					addedfieldname = true;
					if ( ! parseField( dp, pos, lineno, token, lexum, errcode, true ) )
					{
						return false;
					}
				}
				else
				{
					if ( ! parseLit( dp, pos, lineno, token, lexum, errcode, false ) ) 
					{
						return false;
					}
				}
			}
			if ( TOK_AS == *token )
			{
				string *fieldname;
				addedfieldname = true;
				if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
				{
					*errcode = ERR_SYNTAX;
					return false;
				}
				if ( TOK_ID != *token )
				{
					*errcode = ERR_EXP_FIELDS;
					return false;
				}
				if ( ! (fieldname = stringConstructChar( (char *)lexum->array->data )) )
				{
					*errcode = ERR_OUT_OF_MEM;
					return false;
				}
				if ( !vectorAdd( dp->ufields, &fieldname ) )
				{
					*errcode = ERR_OUT_OF_MEM;
					stringDestruct( fieldname );
					return false;
				}
				if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
				{
					*errcode = ERR_SYNTAX;
					return false;
				}
			}
			if ( ! addedfieldname )
			{
				string *fieldname;
				char buf[5];
				if ( ! (fieldname = stringConstructChar( "FIELD" )) )
				{
					*errcode = ERR_OUT_OF_MEM;
					return false;
				}
				sprintf(buf, "%d", vectorCount(dp->ufields) );
				if ( ! stringAppendChar( fieldname, buf ) )
				{
					*errcode = ERR_OUT_OF_MEM;
					return false;
				}
				if ( !vectorAdd( dp->ufields, &fieldname ) )
				{
					*errcode = ERR_OUT_OF_MEM;
					stringDestruct( fieldname );
					return false;
				}
			}
			if ( TOK_COMMA == *token )
			{
				if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
				{
					*errcode = ERR_SYNTAX;
					return false;
				}
				if ( ! strbufAppend( dp->tsql, ", " ) )
				{
					*errcode = ERR_OUT_OF_MEM;
					return false;
				}
			}
			if ( ! strbufAppendChar( dp->tsql, ' ' ) )
			{
				*errcode = ERR_OUT_OF_MEM;
				return false;
			}
		}
	}

	/* 2. from clause */
	if ( ! parseFrom(dp, pos, lineno, token, lexum, errcode, &prmidxpos) )
	{
		return false;
	}

	/* 3.  where clause */
	if ( ! parseWhere(dp, pos, lineno, token, lexum, errcode, &prmidxpos, true) )
	{
		return false;
	}

	/* 3.a groub by */
	if ( ! parseGroupBy(dp, pos, lineno, token, lexum, errcode) )
	{
		return false;
	}

	/* 3.b having */
	if ( ! parseHaving(dp, pos, lineno, token, lexum, errcode, &prmidxpos) )
	{
		return false;
	}

	/* 4.  order by clause */
	if ( ! parseOrderBy(dp, pos, lineno, token, lexum, errcode) )
	{
		return false;
	}
	if ( ! strbufAppend( dp->tsql, " FOR BROWSE ACCESS " ) )
	{
		*errcode = ERR_OUT_OF_MEM;
		return false;
	}
	/* 5.  UNION */
	if ( ! ignoreTrailingRpar && TOK_UNION == *token )
	{
		if ( ! strbufAppend( dp->tsql, " UNION " ) )
		{
			*errcode = ERR_OUT_OF_MEM;
			return false;
		}
		if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
		{
			*errcode = ERR_SYNTAX;
			return false;
		}
		return parseSelect( dp, pos, lineno, token, lexum, errcode, ignoreTrailingRpar );
	}

	return true;
}

static bool parseInsert
( 
	sqldef *dp, 
	int *pos, 
	int *lineno, 
	sqltoken *token, 
	strbuf *lexum, 
	sqlparseerror *errcode 
)
{
	dp->stmttype = SQLOP_INSERT;

	if ( TOK_INSERT != *token )
	{
		*errcode = ERR_SYNTAX;
		return false;
	}
	if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
	{
		*errcode = ERR_SYNTAX;
		return false;
	}
	if ( TOK_INTO != *token )
	{
		*errcode = ERR_EXP_INTO;
		return false;
	}
	if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
	{
		*errcode = ERR_SYNTAX;
		return false;
	}
	if ( TOK_FUNC != *token )
	{
		*errcode = ERR_EXP_TABLE;
		return false;
	}
	if ( ! strbufAppend( dp->tsql, " INSERT INTO " ) )
	{
		*errcode = ERR_OUT_OF_MEM;
		return false;
	}
	if ( '=' != ((char *)lexum->array->data)[0] )
	{
		if ( ! strbufAppendChar( dp->tsql, '=' ) )
		{
			*errcode = ERR_OUT_OF_MEM;
			return false;
		}
	}
	if ( ! strbufAppend( dp->tsql, (char *)lexum->array->data ) )
	{
		*errcode = ERR_OUT_OF_MEM;
		return false;
	}
	if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
	{
		*errcode = ERR_SYNTAX;
		return false;
	}
	if ( TOK_LPAR != *token )
	{
		*errcode = ERR_EXP_LPAR;
		return false;
	}
	if ( ! strbufAppendChar( dp->tsql, '(' ) )
	{
		*errcode = ERR_OUT_OF_MEM;
		return false;
	}
	if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
	{
		*errcode = ERR_SYNTAX;
		return false;
	}
	while ( TOK_ID == *token )
	{
		if ( ! strbufAppend( dp->tsql, (char *)lexum->array->data ) )
		{
			*errcode = ERR_OUT_OF_MEM;
			return false;
		}
		if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
		{
			*errcode = ERR_SYNTAX;
			return false;
		}
		if ( TOK_COMMA == *token )
		{
			if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
			{
				*errcode = ERR_SYNTAX;
				return false;
			}
			if ( TOK_ID != *token )
			{
				*errcode = ERR_EXP_FIELDS;
				return false;
			}
			if ( ! strbufAppendChar( dp->tsql, ',' ) )
			{
				*errcode = ERR_OUT_OF_MEM;
				return false;
			}			
		}
	}
	if ( TOK_RPAR != *token )
	{
		*errcode = ERR_EXP_RPAR;
		return false;
	}
	if ( ! strbufAppendChar( dp->tsql, ')' ) )
	{
		*errcode = ERR_OUT_OF_MEM;
		return false;
	}
	if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
	{
		*errcode = ERR_SYNTAX;
		return false;
	}
	if ( TOK_VALUES != *token )
	{
		*errcode = ERR_EXP_VALUES;
		return false;
	}
	if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
	{
		*errcode = ERR_SYNTAX;
		return false;
	}
	if ( TOK_LPAR != *token )
	{
		*errcode = ERR_EXP_LPAR;
		return false;
	}
	if ( ! strbufAppend( dp->tsql, " VALUES (" ) )
	{
		*errcode = ERR_OUT_OF_MEM;
		return false;
	}
	if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
	{
		*errcode = ERR_SYNTAX;
		return false;
	}
	while ( TOK_FUNC == *token || TOK_NUM == *token || TOK_ID == *token || TOK_VAR == *token || TOK_STRLIT == *token )
	{
		if( TOK_VAR == *token )
		{
			if ( ! strbufAppend( dp->tsql, ":p" ) )
			{
				*errcode = ERR_OUT_OF_MEM;
				return false;
			}
		}
		else if ( TOK_STRLIT == *token )
		{
			if ( ! strbufAppendChar( dp->tsql, '\'' ) )
			{
				*errcode = ERR_OUT_OF_MEM;
				return false;
			}			
		}
		if ( TOK_FUNC == *token )
		{
			if ( ! parseFunction(dp, pos, lineno, token, lexum, errcode) )
			{
				return false;
			}
		}
		else
		{
			if ( TOK_VAR == *token )
			{
				if ( ! strbufAppend( dp->tsql, &((char *)lexum->array->data)[1] ) )
				{
					*errcode = ERR_OUT_OF_MEM;
					return false;
				}
			}
			else
			{
				if ( ! strbufAppend( dp->tsql, (char *)lexum->array->data ) )
				{
					*errcode = ERR_OUT_OF_MEM;
					return false;
				}
			}
			if ( TOK_STRLIT == *token )
			{
				if ( ! strbufAppendChar( dp->tsql, '\'' ) )
				{
					*errcode = ERR_OUT_OF_MEM;
					return false;
				}			
			}
			if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
			{
				*errcode = ERR_SYNTAX;
				return false;
			}
		}
		if ( TOK_COMMA == *token )
		{
			if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
			{
				*errcode = ERR_SYNTAX;
				return false;
			}
			if ( ! (TOK_FUNC == *token || TOK_NUM == *token || TOK_ID == *token || TOK_VAR == *token || TOK_STRLIT == *token) )
			{
				*errcode = ERR_EXP_FIELDS;
				return false;
			}
			if ( ! strbufAppendChar( dp->tsql, ',' ) )
			{
				*errcode = ERR_OUT_OF_MEM;
				return false;
			}			
		}
	}
	if ( TOK_RPAR != *token )
	{
		*errcode = ERR_EXP_RPAR;
		return false;
	}
	if ( ! strbufAppendChar( dp->tsql, ')' ) )
	{
		*errcode = ERR_OUT_OF_MEM;
		return false;
	}			
	if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
	{
		*errcode = ERR_SYNTAX;
		return false;
	}
	return true;
}

static bool parseUpdate
( 
	sqldef *dp, 
	int *pos, 
	int *lineno, 
	sqltoken *token, 
	strbuf *lexum, 
	sqlparseerror *errcode 
)
{
	int prmidxpos = 0;
	dp->stmttype = SQLOP_UPDATE;

	if ( TOK_UPDATE != *token )
	{
		*errcode = ERR_SYNTAX;
		return false;
	}
	if ( ! strbufAppend( dp->tsql, "UPDATE " ) )
	{
		*errcode = ERR_OUT_OF_MEM;
		return false;
	}
	if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
	{
		*errcode = ERR_SYNTAX;
		return false;
	}
	if ( TOK_ID != *token )
	{
		*errcode = ERR_EXP_TABLE;
		return false;
	}
	if ( '=' != ((char *)lexum->array->data)[0] )
	{
		if ( ! strbufAppendChar( dp->tsql, '=' ) )
		{
			*errcode = ERR_OUT_OF_MEM;
			return false;
		}
	}
	if ( ! strbufAppend( dp->tsql, (char *)lexum->array->data ) )
	{
		*errcode = ERR_OUT_OF_MEM;
		return false;
	}
	if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
	{
		*errcode = ERR_SYNTAX;
		return false;
	}
	if ( TOK_SET != *token )
	{
		*errcode = ERR_EXP_SET;
		return false;
	}
	if ( ! strbufAppend( dp->tsql, " SET " ) )
	{
		*errcode = ERR_OUT_OF_MEM;
		return false;
	}
	if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
	{
		*errcode = ERR_SYNTAX;
		return false;
	}
	while ( TOK_ID == *token )
	{
		if ( ! parseField(dp, pos, lineno, token, lexum, errcode, false) )
		{
			return false;
		}
		if ( TOK_EQ != *token )
		{
			*errcode = ERR_EXP_OP;
			return false;
		}
		if ( ! strbufAppend( dp->tsql, " = " ) )
		{
			*errcode = ERR_OUT_OF_MEM;
			return false;
		}
		if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
		{
			*errcode = ERR_SYNTAX;
			return false;
		}
		if ( TOK_NUM == *token )
		{
			if ( ! strbufAppend( dp->tsql, (char *)lexum->array->data ) )
			{
				*errcode = ERR_OUT_OF_MEM;
				return false;
			}
		}
		else if ( TOK_STRLIT == *token )
		{
			if ( ! strbufAppendChar( dp->tsql, '\'' ) )
			{
				*errcode = ERR_OUT_OF_MEM;
				return false;
			}
			if ( ! strbufAppend( dp->tsql, (char *)lexum->array->data ) )
			{
				*errcode = ERR_OUT_OF_MEM;
				return false;
			}
			if ( ! strbufAppendChar( dp->tsql, '\'' ) )
			{
				*errcode = ERR_OUT_OF_MEM;
				return false;
			}
		}
		else if ( TOK_VAR == *token )
		{
			if ( ! strbufAppend( dp->tsql, ":p" ) )
			{
				*errcode = ERR_OUT_OF_MEM;
				return false;
			}
			if ( ! strbufAppend( dp->tsql, &((char *)lexum->array->data)[1] ) )
			{
				*errcode = ERR_OUT_OF_MEM;
				return false;
			}
		}
		else if ( TOK_FUNC == *token )
		{
			if ( ! parseFunction(dp, pos, lineno, token, lexum, errcode) )
			{
				return false;
			}
		}
		else
		{
			*errcode = ERR_SYNTAX;
			return false;
		}
		if ( ! strbufAppendChar( dp->tsql, ' ' ) )
		{
			*errcode = ERR_OUT_OF_MEM;
			return false;
		}
		if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
		{
			*errcode = ERR_SYNTAX;
			return false;
		}
		if ( TOK_COMMA == *token )
		{
			if ( ! strbufAppendChar( dp->tsql, ',' ) )
			{
				*errcode = ERR_OUT_OF_MEM;
				return false;
			}
			if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
			{
				*errcode = ERR_SYNTAX;
				return false;
			}
			if ( TOK_ID != *token )
			{
				*errcode = ERR_EXP_FIELDS;
				return false;
			}
		}
	}
	return parseWhere(dp, pos, lineno, token, lexum, errcode, &prmidxpos, false);
}

static bool parseDelete
( 
	sqldef *dp, 
	int *pos, 
	int *lineno, 
	sqltoken *token, 
	strbuf *lexum, 
	sqlparseerror *errcode 
)
{
	int prmidxpos = 0;
	dp->stmttype = SQLOP_DELETE;

	if ( TOK_DELETE != *token )
	{
		*errcode = ERR_SYNTAX;
		return false;
	}
	if ( ! sqltokenizer( dp->src, pos, lineno, token, lexum ) )
	{
		*errcode = ERR_SYNTAX;
		return false;
	}
	if ( ! strbufAppend( dp->tsql, "DELETE " ) )
	{
		*errcode = ERR_OUT_OF_MEM;
		return false;
	}
	if ( ! parseFrom(dp, pos, lineno, token, lexum, errcode, &prmidxpos) )
	{
		return false;
	}
	if ( ! parseWhere(dp, pos, lineno, token, lexum, errcode, &prmidxpos, false) )
	{
		return false;
	}
	return true;
}

sqldef *parseSQL( char *src, sqlparseerror *errcode, int *lineno )
{
	int pos = 0;
	sqltoken token;
	strbuf *lexum;
	sqldef *dp = (sqldef *)malloc(sizeof(sqldef));
	if ( NULL == dp )
	{
		*errcode = ERR_OUT_OF_MEM;
		return NULL;
	}
	*lineno = 1;
	memset( dp, 0, sizeof(sqldef) );
	if ( NULL == (dp->uparamsLookup = hashtableConstruct()) )
	{
		*errcode = ERR_OUT_OF_MEM;
		free(dp);
		return NULL;
	}
	if ( NULL == (dp->src = stringConstructChar( src )) )
	{
		*errcode = ERR_OUT_OF_MEM;
		sqldefDestruct( dp );
		return NULL;
	}
	if ( NULL == (dp->tsql = strbufConstructBuf(40)) )
	{
		*errcode = ERR_OUT_OF_MEM;
		sqldefDestruct( dp );
		return NULL;
	}
	if ( NULL == (dp->ufields = vectorConstruct( sizeof(string *) )) )
	{
		*errcode = ERR_OUT_OF_MEM;
		sqldefDestruct( dp );
		return NULL;
	}
	if ( NULL == (lexum = strbufConstructBuf( 20 )) )
	{
		*errcode = ERR_OUT_OF_MEM;
		sqldefDestruct( dp );
		return NULL;
	}
	if ( ! sqltokenizer( dp->src, &pos, lineno, &token, lexum ) )
	{
		strbufDestruct( lexum );
		*errcode = ERR_SYNTAX;
		sqldefDestruct( dp );
		return NULL;
	}
	if ( TOK_CREATE != token )
	{
		strbufDestruct( lexum );
		*errcode = ERR_EXP_CREATE;
		sqldefDestruct( dp );
		return NULL;
	}
	if ( ! sqltokenizer( dp->src, &pos, lineno, &token, lexum ) )
	{
		strbufDestruct( lexum );
		*errcode = ERR_SYNTAX;
		sqldefDestruct( dp );
		return NULL;
	}
	if ( TOK_PROC != token )
	{
		strbufDestruct( lexum );
		*errcode = ERR_EXP_PROC;
		sqldefDestruct( dp );
		return NULL;
	}
	if ( ! sqltokenizer( dp->src, &pos, lineno, &token, lexum ) )
	{
		strbufDestruct( lexum );
		*errcode = ERR_SYNTAX;
		sqldefDestruct( dp );
		return NULL;
	}
	if ( TOK_ID != token )
	{
		strbufDestruct( lexum );
		*errcode = ERR_EXP_PROC;
		sqldefDestruct( dp );
		return NULL;
	}
	if ( NULL == (dp->sys = strbufToString( lexum )) )
	{
		strbufDestruct( lexum );
		*errcode = ERR_OUT_OF_MEM;
		sqldefDestruct( dp );
		return NULL;
	}
	stringUCase(dp->sys);
	if ( ! sqltokenizer( dp->src, &pos, lineno, &token, lexum ) )
	{
		strbufDestruct( lexum );
		*errcode = ERR_SYNTAX;
		sqldefDestruct( dp );
		return NULL;
	}
	if ( TOK_DOT != token )
	{
		strbufDestruct( lexum );
		*errcode = ERR_EXP_DOT;
		sqldefDestruct( dp );
		return NULL;
	}
	if ( ! sqltokenizer( dp->src, &pos, lineno, &token, lexum ) )
	{
		strbufDestruct( lexum );
		*errcode = ERR_SYNTAX;
		sqldefDestruct( dp );
		return NULL;
	}
	if ( TOK_FUNC != token && TOK_ID != token )
	{
		strbufDestruct( lexum );
		*errcode = ERR_EXP_PROC;
		sqldefDestruct( dp );
		return NULL;
	}
	if ( NULL == (dp->name = strbufToString( lexum )) )
	{
		strbufDestruct( lexum );
		*errcode = ERR_OUT_OF_MEM;
		sqldefDestruct( dp );
		return NULL;
	}
	stringUCase( dp->name );
	if ( ! parseParamList( dp, &pos, lineno, &token, lexum, errcode ) )
	{
		strbufDestruct( lexum );
		sqldefDestruct( dp );
		return NULL;
	}

	/* actual SQL code */
	dp->codepos = pos;
	if ( TOK_EOS != token )
	{
		if ( TOK_SELECT == token )
		{
			if ( ! parseSelect( dp, &pos, lineno, &token, lexum, errcode, false ) )
			{
				strbufDestruct( lexum );
				sqldefDestruct( dp );
				return NULL;
			}
		}
		else if ( TOK_INSERT == token )
		{
			if ( ! parseInsert( dp, &pos, lineno, &token, lexum, errcode ) )
			{
				strbufDestruct( lexum );
				sqldefDestruct( dp );
				return NULL;
			}
		}
		else if ( TOK_UPDATE == token )
		{
			if ( ! parseUpdate( dp, &pos, lineno, &token, lexum, errcode ) )
			{
				strbufDestruct( lexum );
				sqldefDestruct( dp );
				return NULL;
			}
		}
		else if ( TOK_DELETE == token )
		{
			if ( ! parseDelete( dp, &pos, lineno, &token, lexum, errcode ) )
			{
				strbufDestruct( lexum );
				sqldefDestruct( dp );
				return NULL;
			}
		}
		else
		{
			*errcode = ERR_SYNTAX;
			strbufDestruct( lexum );
			sqldefDestruct( dp );
			return NULL;
		}
		if ( TOK_SEMI == token )
		{
			sqltokenizer( dp->src, &pos, lineno, &token, lexum );
		}
		if ( TOK_EOS != token )
		{
			strbufDestruct( lexum );
			sqldefDestruct( dp );
			*errcode = ERR_SYNTAX;
			return NULL;
		}
	}
	strbufDestruct( lexum );
	return dp;
}

static void skipws( string *str, int *pos, int *linenum )
{
	while ( *pos < str->len && iswspace( str->cstr[*pos] ) )
	{
		if ( str->cstr[*pos] == '\n' )
		{
			(*linenum)++;
		}
		(*pos)++;
	}
}

bool scanstr( string *src, int *pos, strbuf *lexum )
{
	while ( *pos < src->len && src->cstr[*pos] != '\'' )
	{
		if ( '\n' == src->cstr[*pos] )
		{
			return false;
		}
		if ( ! strbufAppendChar( lexum, src->cstr[(*pos)++] ) )
		{
			return false;
		}
	}
	if ( src->cstr[*pos] == '\'' )
	{
		(*pos)++;
		return true;
	}
	else
	{
		return false;
	}
}

bool scantows( string *src, int *pos, strbuf *lexum )
{
	while ( *pos < src->len && !iswspace(src->cstr[*pos]) && (isalnum(src->cstr[*pos]) || '_' == src->cstr[*pos] ) )
	{
		if ( ! strbufAppendChar( lexum, src->cstr[(*pos)++] ) )
		{
			return false;
		}
	}
	return true;
}

bool sqltokenizer( string *src, int *pos, int *linenum, sqltoken *token, strbuf *lexum )
{
	skipws( src, pos, linenum );

	if ( *pos >= src->len )
	{
		*token = TOK_EOS;
		return true;
	}
	strbufSetLen( lexum, 0 );
	strbufAppendChar( lexum, src->cstr[*pos] );

	if ( isdigit(src->cstr[*pos]) || '-' == src->cstr[*pos] )
	{
		char ch = src->cstr[++(*pos)];
		while ( isdigit( ch ) )
		{
			strbufAppendChar( lexum, ch );
			ch = src->cstr[++(*pos)];
		}
		*token = TOK_NUM;
	}
	else
	{
		switch( src->cstr[(*pos)++] )
		{
		case '\'':
			*token = TOK_STRLIT;
			strbufSetLen( lexum, 0 );
			return scanstr( src, pos, lexum );
		case '(':
			*token = TOK_LPAR;
			break;
		case ')':
			*token = TOK_RPAR;
			break;
		case '.':
			*token = TOK_DOT;
			break;
		case '>':
			if ( '=' == src->cstr[*pos] )
			{
				(*pos)++;
				*token = TOK_GTEQ;
			}
			else
			{
				*token = TOK_GT;
			}
			break;
		case '<':
			if ( '=' == src->cstr[*pos] )
			{
				(*pos)++;
				*token = TOK_LTEQ;
			}
			if ( '>' == src->cstr[*pos] )
			{
				(*pos)++;
				*token = TOK_NEQ;
			}
			else
			{
				*token = TOK_LT;
			}
			break;
		case '*':
			*token = TOK_SPLAT;
			break;
		case ',':
			*token = TOK_COMMA;
			break;
		case ';':
			*token = TOK_SEMI;
			break;
		case '=':
			if ( isspace(src->cstr[*pos]) )
			{
				*token = TOK_EQ;
				break;
			}
		default:
			if ( ! scantows( src, pos, lexum ) )
			{
				return false;
			}
			else
			{
				string *ll = strbufToString( lexum );
				if ( NULL == ll )
				{
					return false;
				}
				stringUCase( ll );
				if ( 0 == stringCmpChar( ll, "SELECT" ) )
				{
					*token = TOK_SELECT;
				}
				else if ( 0 == stringCmpChar( ll, "FROM" ) )
				{
					*token = TOK_FROM;
				}
				else if ( 0 == stringCmpChar( ll, "AS" ) )
				{
					*token = TOK_AS;
				}
				else if ( 0 == stringCmpChar( ll, "CREATE" ) )
				{
					*token = TOK_CREATE;
				}
				else if ( 0 == stringCmpChar( ll, "PROCEDURE" ) )
				{
					*token = TOK_PROC;
				}
				else if ( 0 == stringCmpChar( ll, "INT" ) )
				{
					*token = TOK_INT;
				}
				else if ( 0 == stringCmpChar( ll, "SMALLINT" ) )
				{
					*token = TOK_SMALLINT;
				}
				else if ( 0 == stringCmpChar( ll, "BIGINT" ) )
				{
					*token = TOK_BIGINT;
				}
				else if ( 0 == stringCmpChar( ll, "CHAR" ) )
				{
					*token = TOK_CHAR;
				}
				else if ( 0 == stringCmpChar( ll, "WHERE" ) )
				{
					*token = TOK_WHERE;
				}
				else if ( 0 == stringCmpChar( ll, "AND" ) )
				{
					*token = TOK_AND;
				}
				else if ( 0 == stringCmpChar( ll, "OR" ) )
				{
					*token = TOK_OR;
				}
				else if ( 0 == stringCmpChar( ll, "INNER" ) )
				{
					*token = TOK_INNER;
				}
				else if ( 0 == stringCmpChar( ll, "JOIN" ) )
				{
					*token = TOK_JOIN;
				}
				else if ( 0 == stringCmpChar( ll, "LEFT" ) )
				{
					*token = TOK_LEFT;
				}
				else if ( 0 == stringCmpChar( ll, "RIGHT" ) )
				{
					*token = TOK_RIGHT;
				}
				else if ( 0 == stringCmpChar( ll, "ON" ) )
				{
					*token = TOK_ON;
				}
				else if ( 0 == stringCmpChar( ll, "INSERT" ) )
				{
					*token = TOK_INSERT;
				}
				else if ( 0 == stringCmpChar( ll, "INTO" ) )
				{
					*token = TOK_INTO;
				}
				else if ( 0 == stringCmpChar( ll, "VALUES" ) )
				{
					*token = TOK_VALUES;
				}
				else if ( 0 == stringCmpChar( ll, "UPDATE" ) )
				{
					*token = TOK_UPDATE;
				}
				else if ( 0 == stringCmpChar( ll, "SET" ) )
				{
					*token = TOK_SET;
				}
				else if ( 0 == stringCmpChar( ll, "ORDER" ) )
				{
					*token = TOK_ORDER;
				}
				else if ( 0 == stringCmpChar( ll, "BY" ) )
				{
					*token = TOK_BY;
				}
				else if ( 0 == stringCmpChar( ll, "DESC" ) )
				{
					*token = TOK_DESC;
				}
				else if ( 0 == stringCmpChar( ll, "ASC" ) )
				{
					*token = TOK_ASC;
				}
				else if ( 0 == stringCmpChar( ll, "IN" ) )
				{
					*token = TOK_IN;
				}
				else if ( 0 == stringCmpChar( ll, "DELETE" ) )
				{
					*token = TOK_DELETE;
				}
				else if ( 0 == stringCmpChar( ll, "GROUP" ) )
				{
					*token = TOK_GROUP;
				}
				else if ( 0 == stringCmpChar( ll, "HAVING" ) )
				{
					*token = TOK_HAVING;
				}
				else if ( 0 == stringCmpChar( ll, "LIKE" ) )
				{
					*token = TOK_LIKE;
				}
				else if ( 0 == stringCmpChar( ll, "IS" ) )
				{
					*token = TOK_IS;
				}
				else if ( 0 == stringCmpChar( ll, "NULL" ) )
				{
					*token = TOK_NULL;
				}
				else if ( 0 == stringCmpChar( ll, "NOT" ) )
				{
					*token = TOK_NOT;
				}
				else if ( 0 == stringCmpChar( ll, "UNION" ) )
				{
					*token = TOK_UNION;
				}
				else if ( 0 == stringCmpChar( ll, "CASE" ) )
				{
					*token = TOK_CASE;
				}
				else if ( 0 == stringCmpChar( ll, "WHEN" ) )
				{
					*token = TOK_WHEN;
				}
				else if ( 0 == stringCmpChar( ll, "THEN" ) )
				{
					*token = TOK_THEN;
				}
				else if ( 0 == stringCmpChar( ll, "ELSE" ) )
				{
					*token = TOK_ELSE;
				}
				else if ( 0 == stringCmpChar( ll, "END" ) )
				{
					*token = TOK_END;
				}
				else if ( 0 == stringCmpChar( ll, "DISTINCT" ) )
				{
					*token = TOK_DISTINCT;
				}
				else if ( '@' == ll->cstr[0] )
				{
					*token = TOK_VAR;
				}
				else if ( ll->len == 1 && ll->cstr[0] == '=' )
				{
					*token = TOK_EQ;
				}
				else
				{
					skipws(src, pos, linenum);
					if ( '(' == src->cstr[(*pos)] )
					{
						*token = TOK_FUNC;
					}
					else
					{
						*token = TOK_ID;
					}
				}
				stringDestruct( ll );
			}
			break;
		}
	}
	return true;
}

char *sqlparseerrormsgs[] =
{
	"No error",
	"Syntax error",
	"Out of memory",
	"Expected CREATE",
	"Expected PROCEDURE",
	"Expected DOT",
	"Expected ')'",
	"Expected '('",
	"Expected AS",
	"Expected fields",
	"Expected table",
	"Expected ';'",
	"Expected type",
	"Expected type length",
	"Expected operator",
	"Expected condition",
	"Undefined parameter",
	"Expected field after DOT",
	"Expected JOIN",
	"Expected ON",
	"Expected INTO",
	"Expected VALUES",
	"Expected SET",
	"Expected ORDER BY",
	"Unknown function",
	"Expected WHEN",
	"CASE can have only one ELSE",
	"Expected THEN",
	"Expected CASE value",
	"Expected END",
	"Unexpected ')'"
};

char *datatypename[] = {
	"INT",
	"FLOAT4",
	"FLOAT8",
	"CHAR",
	"LONG",
	"SHORT",
	"VARCHAR"
};
