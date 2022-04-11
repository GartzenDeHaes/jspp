#include "sqllex.h"

#define ISEOF(l) (l->m_token == SQL_EOF || l->m_pos >= l->m_text->m_len)

sqllex* sqllexNew(string* sql)
{
	sqllex* lex = (sqllex *)malloc(sizeof(sqllex));
	if (lex == NULL)
	{
		return NULL;
	}
	
	lex->m_token = SQL_BOF;
	lex->m_lexum = NULL;
	lex->m_pos = 0;
	lex->m_lineNum = 1;
	
	if (NULL == (lex->m_text = stringDup(sql)))
	{
		free(lex);
		return NULL;
	}
	
	if (NULL == (lex->m_buf = stringbufferNew()))
	{
		stringDelete(lex->m_text);
		free(lex);
		return NULL;
	}
	
	sqllexNext(lex);
	
	return lex;
}

void sqllexDelete(sqllex* lex)
{
	stringDelete(lex->m_text);
	stringbufferDelete(lex->m_buf);
	
	if (lex->m_lexum != NULL)
	{
		stringDelete(lex->m_lexum);
	}
	
	free(lex);
}

static void _sqllexRead(sqllex* lex)
{
	char ch = lex->m_text->m_cstr[lex->m_pos];
	
	stringbufferClear(lex->m_buf);
	
	while (! ISEOF(lex) && !(isalnum(ch) || ispunct(ch)))
	{
		if (ch == '\n')
		{
			lex->m_lineNum++;
		}
		ch = lex->m_text->m_cstr[++lex->m_pos];
	}
	
	if (ISEOF(lex))
	{
		return;
	}
	
	if (ch == '\'' || ch == '"')
	{
		char exitch = ch;

		ch = lex->m_text->m_cstr[++lex->m_pos];
		while (ch != exitch)
		{
			stringbufferAppendCh(lex->m_buf, ch);
			ch = lex->m_text->m_cstr[++lex->m_pos];
		}
		++lex->m_pos;
		
		lex->m_token = SQL_STRLIT;
	}
	else if (isalnum(ch) || ch == '_')
	{
		while (! ISEOF(lex) && (isalnum(ch) || ch == '_'))
		{
			stringbufferAppendCh(lex->m_buf, ch);
			ch = lex->m_text->m_cstr[++lex->m_pos];
		}
	}
	else
	{
		while (! ISEOF(lex) && ispunct(ch))
		{
			stringbufferAppendCh(lex->m_buf, ch);
			ch = lex->m_text->m_cstr[++lex->m_pos];
			if (ch == ')')
			{
				break;
			}
		}
	}
}

bool sqllexMatch(sqllex* lex, const char* cp)
{
	if (stringIsEqual(lex->m_lexum, cp, true))
	{
		return sqllexNext(lex);
	}
	
	return false;
}

bool sqllexMatchToken(sqllex* lex, const int tok)
{
	if (lex->m_token == tok)
	{
		return sqllexNext(lex);
	}
	
	return false;
}

bool sqllexNext(sqllex* lex)
{
	if (lex->m_token == SQL_EOF)
	{
		return false;
	}
	
	if (lex->m_lexum != NULL)
	{
		stringDelete(lex->m_lexum);
		lex->m_lexum = NULL;
	}
	
	if (lex->m_pos >= lex->m_text->m_len)
	{
		lex->m_token = SQL_EOF;
		return true;
	}
	
	lex->m_token = SQL_UNKNOWN;
	
	_sqllexRead(lex);
	
	if (lex->m_buf->m_used == 0)
	{
		return false;
	}
	
	lex->m_lexum = stringbufferToString(lex->m_buf);
	
	if (lex->m_token == SQL_STRLIT)
	{
		return true;
	}
	
	if (stringIsEqual(lex->m_lexum, "(", false))
	{
		lex->m_token = SQL_LPAR;
	}
	else if (stringIsEqual(lex->m_lexum, ")", false))
	{
		lex->m_token = SQL_RPAR;
	}
	else if (stringIsEqual(lex->m_lexum, ">", false))
	{
		lex->m_token = SQL_GT;
	}
	else if (stringIsEqual(lex->m_lexum, ">=", false))
	{
		lex->m_token = SQL_GTEQ;
	}
	else if (stringIsEqual(lex->m_lexum, "<", false))
	{
		lex->m_token = SQL_LT;
	}
	else if (stringIsEqual(lex->m_lexum, "<=", false))
	{
		lex->m_token = SQL_LTEQ;
	}
	else if (stringIsEqual(lex->m_lexum, "<>", false))
	{
		lex->m_token = SQL_NEQ;
	}
	else if (stringIsEqual(lex->m_lexum, "*", false))
	{
		lex->m_token = SQL_SPLAT;
	}
	else if (stringIsEqual(lex->m_lexum, ":", false))
	{
		lex->m_token = SQL_COLON;
	}
	else if (stringIsEqual(lex->m_lexum, "?", false))
	{
		/* has the same effect as colon in this program */
		lex->m_token = SQL_COLON;
	}
	else if (stringIsEqual(lex->m_lexum, ";", false))
	{
		lex->m_token = SQL_SEMI;
	}
	else if (stringIsEqual(lex->m_lexum, "=", false))
	{
		lex->m_token = SQL_EQ;
	}
	else if (stringIsEqual(lex->m_lexum, ".", false))
	{
		lex->m_token = SQL_DOT;
	}
	else if (stringIsEqual(lex->m_lexum, ",", false))
	{
		lex->m_token = SQL_COMMA;
	}
	else if (stringIsEqual(lex->m_lexum, "CREATE", true))
	{
		lex->m_token = SQL_CREATE;
	}
	else if (stringIsEqual(lex->m_lexum, "PROCEDURE", true))
	{
		lex->m_token = SQL_PROC;
	}
	else if (stringIsEqual(lex->m_lexum, "SELECT", true))
	{
		lex->m_token = SQL_SELECT;
	}
	else if (stringIsEqual(lex->m_lexum, "FROM", true))
	{
		lex->m_token = SQL_FROM;
	}
	else if (stringIsEqual(lex->m_lexum, "AS", true))
	{
		lex->m_token = SQL_AS;
	}
	else if (stringIsEqual(lex->m_lexum, "INT", true))
	{
		lex->m_token = SQL_INT;
	}
	else if (stringIsEqual(lex->m_lexum, "INTEGER", true))
	{
		lex->m_token = SQL_INT;
	}
	else if (stringIsEqual(lex->m_lexum, "SMALLINT", true))
	{
		lex->m_token = SQL_SMALLINT;
	}
	else if (stringIsEqual(lex->m_lexum, "BIGINT", true))
	{
		lex->m_token = SQL_BIGINT;
	}
	else if (stringIsEqual(lex->m_lexum, "CHAR", true))
	{
		lex->m_token = SQL_CHAR;
	}
	else if (stringIsEqual(lex->m_lexum, "WHERE", true))
	{
		lex->m_token = SQL_WHERE;
	}
	else if (stringIsEqual(lex->m_lexum, "AND", true))
	{
		lex->m_token = SQL_AND;
	}
	else if (stringIsEqual(lex->m_lexum, "OR", true))
	{
		lex->m_token = SQL_OR;
	}
	else if (stringIsEqual(lex->m_lexum, "INNER", true))
	{
		lex->m_token = SQL_INNER;
	}
	else if (stringIsEqual(lex->m_lexum, "JOIN", true))
	{
		lex->m_token = SQL_JOIN;
	}
	else if (stringIsEqual(lex->m_lexum, "LEFT", true))
	{
		lex->m_token = SQL_LEFT;
	}
	else if (stringIsEqual(lex->m_lexum, "RIGHT", true))
	{
		lex->m_token = SQL_RIGHT;
	}
	else if (stringIsEqual(lex->m_lexum, "ON", true))
	{
		lex->m_token = SQL_ON;
	}
	else if (stringIsEqual(lex->m_lexum, "INSERT", true))
	{
		lex->m_token = SQL_INSERT;
	}
	else if (stringIsEqual(lex->m_lexum, "INTO", true))
	{
		lex->m_token = SQL_INTO;
	}
	else if (stringIsEqual(lex->m_lexum, "VALUES", true))
	{
		lex->m_token = SQL_VALUES;
	}
	else if (stringIsEqual(lex->m_lexum, "UPDATE", true))
	{
		lex->m_token = SQL_UPDATE;
	}
	else if (stringIsEqual(lex->m_lexum, "SET", true))
	{
		lex->m_token = SQL_SET;
	}
	else if (stringIsEqual(lex->m_lexum, "ORDER", true))
	{
		lex->m_token = SQL_ORDER;
	}
	else if (stringIsEqual(lex->m_lexum, "BY", true))
	{
		lex->m_token = SQL_BY;
	}
	else if (stringIsEqual(lex->m_lexum, "DESC", true))
	{
		lex->m_token = SQL_DESC;
	}
	else if (stringIsEqual(lex->m_lexum, "ASC", true))
	{
		lex->m_token = SQL_ASC;
	}
	else if (stringIsEqual(lex->m_lexum, "IN", true))
	{
		lex->m_token = SQL_IN;
	}
	else if (stringIsEqual(lex->m_lexum, "DELETE", true))
	{
		lex->m_token = SQL_DELETE;
	}
	else if (stringIsEqual(lex->m_lexum, "GROUP", true))
	{
		lex->m_token = SQL_GROUP;
	}
	else if (stringIsEqual(lex->m_lexum, "HAVING", true))
	{
		lex->m_token = SQL_HAVING;
	}
	else if (stringIsEqual(lex->m_lexum, "LIKE", true))
	{
		lex->m_token = SQL_LIKE;
	}
	else if (stringIsEqual(lex->m_lexum, "IS", true))
	{
		lex->m_token = SQL_IS;
	}
	else if (stringIsEqual(lex->m_lexum, "NULL", true))
	{
		lex->m_token = SQL_NULL;
	}
	else if (stringIsEqual(lex->m_lexum, "NOT", true))
	{
		lex->m_token = SQL_NOT;
	}
	else if (stringIsEqual(lex->m_lexum, "UNION", true))
	{
		lex->m_token = SQL_UNION;
	}
	else if (stringIsEqual(lex->m_lexum, "CASE", true))
	{
		lex->m_token = SQL_CASE;
	}
	else if (stringIsEqual(lex->m_lexum, "WHEN", true))
	{
		lex->m_token = SQL_WHEN;
	}
	else if (stringIsEqual(lex->m_lexum, "THEN", true))
	{
		lex->m_token = SQL_THEN;
	}
	else if (stringIsEqual(lex->m_lexum, "ELSE", true))
	{
		lex->m_token = SQL_ELSE;
	}
	else if (stringIsEqual(lex->m_lexum, "END", true))
	{
		lex->m_token = SQL_END;
	}
	else if (stringIsEqual(lex->m_lexum, "DISTINCT", true))
	{
		lex->m_token = SQL_DISTINCT;
	}
	else if (stringIsEqual(lex->m_lexum, "BEGIN", true))
	{
		lex->m_token = SQL_BEGIN;
	}
	else if (stringIsEqual(lex->m_lexum, "TRANSACTION", true))
	{
		lex->m_token = SQL_TRANS;
	}
	else if (stringIsEqual(lex->m_lexum, "COMMIT", true))
	{
		lex->m_token = SQL_COMMIT;
	}
	else if (stringIsEqual(lex->m_lexum, "ABORT", true))
	{
		lex->m_token = SQL_ABORT;
	}
	else if (stringIsEqual(lex->m_lexum, "INVOKE", true))
	{
		lex->m_token = SQL_INVOKE;
	}
	else if (stringIsEqual(lex->m_lexum, "ALTER", true))
	{
		lex->m_token = SQL_ALTER;
	}
	else if (stringIsEqual(lex->m_lexum, "DECLARE", true))
	{
		lex->m_token = SQL_DECLARE;
	}
	else if (stringIsEqual(lex->m_lexum, "SECTION", true))
	{
		lex->m_token = SQL_SECTION;
	}
	else if (stringIsEqual(lex->m_lexum, "INCLUDE", true))
	{
		lex->m_token = SQL_INCLUDE;
	}
	else if (stringIsEqual(lex->m_lexum, "CURSOR", true))
	{
		lex->m_token = SQL_CURSOR;
	}
	else if (stringIsEqual(lex->m_lexum, "FOR", true))
	{
		lex->m_token = SQL_FOR;
	}
	else if (stringIsEqual(lex->m_lexum, "OPEN", true))
	{
		lex->m_token = SQL_OPEN;
	}
	else if (stringIsEqual(lex->m_lexum, "FETCH", true))
	{
		lex->m_token = SQL_FETCH;
	}
	else if (stringIsEqual(lex->m_lexum, "BETWEEN", true))
	{
		lex->m_token = SQL_BETWEEN;
	}
	else if (stringIsEqual(lex->m_lexum, "CLOSE", true))
	{
		lex->m_token = SQL_CLOSE;
	}
	else if (stringIsEqual(lex->m_lexum, "DATETIME", true))
	{
		lex->m_token = SQL_DATETIME;
	}
	else if (stringIsEqual(lex->m_lexum, "FREE", true))
	{
		lex->m_token = SQL_FREE;
	}
	else if (stringIsEqual(lex->m_lexum, "ROLLBACK", true))
	{
		lex->m_token = SQL_ROLLBACK;
	}
	else if (stringIsEqual(lex->m_lexum, "CONTROL", true))
	{
		lex->m_token = SQL_CONTROL;
	}
	else if (stringIsEqual(lex->m_lexum, "CAST", true))
	{
		lex->m_token = SQL_CAST;
	}
	else if (stringIsEqual(lex->m_lexum, "EXISTS", true))
	{
		lex->m_token = SQL_EXISTS;
	}
	else if (stringIsEqual(lex->m_lexum, "BROWSE", true))
	{
		lex->m_token = SQL_BROWSE;
	}
	else if (stringIsEqual(lex->m_lexum, "STABLE", true))
	{
		lex->m_token = SQL_STABLE;
	}
	else if (stringIsEqual(lex->m_lexum, "INTERVAL", true))
	{
		lex->m_token = SQL_INTERVAL;
	}
	else if (stringIsEqual(lex->m_lexum, "SUBSTRING", true))
	{
		lex->m_token = SQL_SUBSTRING;
	}
	else if (stringIsEqual(lex->m_lexum, "DATEFORMAT", true))
	{
		lex->m_token = SQL_DATEFORMAT;
	}
	else if (stringIsEqual(lex->m_lexum, "POSITION", true))
	{
		lex->m_token = SQL_POSITION;
	}
	else if (stringIsEqual(lex->m_lexum, "TRIM", true))
	{
		lex->m_token = SQL_TRIM;
	}
	else if (stringIsEqual(lex->m_lexum, "EXEC", true))
	{
		lex->m_token = SQL_EXEC;
	}
	else if (stringIsEqual(lex->m_lexum, "EXECUTE", true))
	{
		lex->m_token = SQL_EXEC;
	}
	else if (stringIsNumeric(lex->m_lexum))
	{
		lex->m_token = SQL_NUM;
	}
	else
	{
		lex->m_token = SQL_ID;
	}
	
	return true;
}

#ifdef DEBUG
void sqllexValidateMem(sqllex* lex)
{
	ASSERT_MEM(lex, sizeof(sqllex));
	stringValidateMem(lex->m_text);
	stringbufferValidateMem(lex->m_buf);
	
	if (lex->m_lexum != NULL)
	{
		stringValidateMem(lex->m_lexum);
	}
}
#endif
