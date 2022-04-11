#include "errors.h"
#include "sqlparam.h"
#include <spl/integer.h>

static const char *g_ctypes[] = {
	"int32",
	"double",
	"int64",
	"char"
};

sqlparam* sqlparamNew(sqllex* lex, int* errorcode)
{
	sqlparam* p;
	stringbuffer* buf;
	
	*errorcode = SQLE_OUTOFMEM;
	
	if (NULL == (p = (sqlparam *)malloc(sizeof(sqlparam))))
	{
		return NULL;
	}
	
	memset(p, 0, sizeof(sqlparam));
	
	if (NULL == (buf = stringbufferNew()))
	{
		free(p);
		return NULL;
	}
	
	if (lex->m_token != SQL_ID)
	{
		*errorcode = SQLE_SYNTAX;
		stringbufferDelete(buf);
		free(p);
		return NULL;
	}
	
	if (NULL == (p->m_name = stringDup(lex->m_lexum)))
	{
		stringbufferDelete(buf);
		free(p);
		return NULL;
	}
	
	if (! sqllexNext(lex))
	{
		*errorcode = SQLE_SYNTAX;
		stringbufferDelete(buf);
		free(p);
		return NULL;
	}
	
	/* type name */
	if (! stringbufferAppendString(buf, lex->m_lexum))
	{
		stringbufferDelete(buf);
		free(p);
		return NULL;
	}

	if (NULL == (p->m_sqltype = stringDup(lex->m_lexum)))
	{
		stringbufferDelete(buf);
		free(p);
		return NULL;
	}

	if (! sqllexNext(lex))
	{
		*errorcode = SQLE_SYNTAX;
		stringbufferDelete(buf);
		sqlparamDelete(p);
		return NULL;
	}
	
	if (lex->m_token == SQL_LPAR)
	{
		stringbufferAppendCh(buf, '(');

		if (! sqllexNext(lex))
		{
			*errorcode = SQLE_SYNTAX;
			stringbufferDelete(buf);
			sqlparamDelete(p);
			return NULL;
		}

		stringbufferAppendString(buf, lex->m_lexum);
		
		if (! stringIsInt(lex->m_lexum))
		{
			*errorcode = SQLE_SYNTAX;
			stringbufferDelete(buf);
			sqlparamDelete(p);
			return NULL;
		}
		
		p->m_length = int32Parse(lex->m_lexum->m_cstr, lex->m_lexum->m_len, 10);
		
		if (! sqllexNext(lex))
		{
			*errorcode = SQLE_SYNTAX;
			stringbufferDelete(buf);
			sqlparamDelete(p);
			return NULL;
		}
	
		if (lex->m_token == SQL_COMMA)
		{
			stringbufferAppendString(buf, lex->m_lexum);
			
			if (! sqllexNext(lex))
			{
				*errorcode = SQLE_SYNTAX;
				stringbufferDelete(buf);
				sqlparamDelete(p);
				return NULL;
			}
			
			p->m_scale = int32Parse(lex->m_lexum->m_cstr, lex->m_lexum->m_len, 10);
	
			stringbufferAppendString(buf, lex->m_lexum);
			
			if (! sqllexNext(lex))
			{
				*errorcode = SQLE_SYNTAX;
				stringbufferDelete(buf);
				sqlparamDelete(p);
				return NULL;
			}			
		}
		
		if (lex->m_token != SQL_RPAR)
		{
			*errorcode = SQLE_SYNTAX;
			stringbufferDelete(buf);
			sqlparamDelete(p);
			return NULL;
		}

		sqllexNext(lex);
		
		stringbufferAppendCh(buf, ')');
	}
	
	p->m_sqltyperaw = stringbufferToString(buf);
	stringbufferDelete(buf);
	
	if (stringIsEqual(lex->m_lexum, "OUT", true))
	{
		p->m_isout = true;
		
		sqllexNext(lex);
	}

	return p;
}

void sqlparamDelete(sqlparam* p)
{
	if (p->m_name != NULL)
	{
		stringDelete(p->m_name);
	}
	if (p->m_sqltyperaw != NULL)
	{
		stringDelete(p->m_sqltyperaw);
	}
	if (p->m_sqltype != NULL)
	{
		stringDelete(p->m_sqltype);
	}
	if (p->m_indicator != NULL)
	{
		stringDelete(p->m_indicator);
	}
	free(p);
}

char sqlparamPacketType(sqlparam* param)
{
	if (stringIsEqual(param->m_sqltype, "INTEGER", true) || stringIsEqual(param->m_sqltype, "INT", true))
	{
		return 'I';
	}
	if (stringIsEqual(param->m_sqltype, "DOUBLE", true) || stringIsEqual(param->m_sqltype, "REAL", true) || stringIsEqual(param->m_sqltype, "FLOAT", true))
	{
		return 'd';
	}
	if (stringIsEqual(param->m_sqltype, "DECIMAL", true))
	{
		if (param->m_scale == 0)
		{
			if (param->m_length < 11)
			{
				return 'I';
			}
			return 'L';
		}
		return 'd';
	}
	return 'S';
}

const char* sqlparamCType(sqlparam* param)
{
	char type = sqlparamPacketType(param);
	if (type == 'I')
	{
		return g_ctypes[0];
	}
	if (type == 'd')
	{
		return g_ctypes[1];
	}
	if (type == 'L')
	{
		return g_ctypes[2];
	}
	else
	{
		return g_ctypes[3];
	}
}

#ifdef DEBUG
void sqlparamValidateMem(sqlparam* p)
{
	ASSERT_MEM(p, sizeof(sqlparam));
	stringValidateMem(p->m_name);
	stringValidateMem(p->m_sqltyperaw);
	stringValidateMem(p->m_sqltype);
	if (p->m_indicator != NULL)
	{
		stringValidateMem(p->m_indicator);
	}
}
#endif
