#include "sqlprocedure.h"
#include <spl/stringbuffer.h>
#include <ctype.h>

static int _sqlprocScanForBody(string* text);
static bool _sqlprocParseParameters(sqlproc* sp, string* head, int* errorcode, int* elinenum);
static bool _sqlprocParseBody(sqlproc* sp, string* body, int* errorcode);

static string* _sqlprocConvert(string* name, bool special);

sqlproc* sqlprocNew(string* sql)
{
	bool ok;
	int pos;
	int errorcode;
	int elinenum;	/* error line number */
	string* stmp;
	sqlproc* sp = (sqlproc*)malloc(sizeof(sqlproc));
	
	if (sp == NULL)
	{
		return NULL;
	}
	
	memset(sp, 0, sizeof(sqlproc));
	
	if (NULL == (sp->m_text = stringDup(sql)))
	{
		sqlprocDelete(sp);
		return NULL;
	}
	
	if (0 > (pos = _sqlprocScanForBody(sp->m_text)))
	{
		sqlprocDelete(sp);
		return NULL;
	}
	
	if (NULL == (stmp = stringSubstring(sp->m_text, 0, pos + 1)))
	{
		sqlprocDelete(sp);
		return NULL;
	}
	
	if (NULL == (sp->m_prms = vectorNew()))
	{
		sqlprocDelete(sp);
		return NULL;
	}
	
	ok = _sqlprocParseParameters(sp, stmp, &errorcode, &elinenum); 
	stringDelete(stmp);
	
	if (ok)
	{
		if (NULL == (stmp = stringSubstringFrom(sp->m_text, pos + 1)))
		{
			sqlprocDelete(sp);
			return NULL;
		}
		
		if (! _sqlprocParseBody(sp, stmp, &errorcode))
		{
			sqlprocDelete(sp);
			sp = NULL;
		}

		stringDelete(stmp);
		
		return sp;
	}
	else
	{
		sqlprocDelete(sp);
	}
	
	return NULL;
}

void sqlprocDelete(sqlproc* sp)
{
	if (sp->m_name != NULL)
	{
		stringDelete(sp->m_name);
	}
	if (sp->m_text != NULL)
	{
		stringDelete(sp->m_text);
	}
	if (sp->m_body != NULL)
	{
		stringDelete(sp->m_body);
	}
	if (sp->m_prms != NULL)
	{
		int x;
		for (x = 0; x < vectorCount(sp->m_prms); x++)
		{
			sqlparamDelete((sqlparam *)vectorElementAt(sp->m_prms, x));
		}
		vectorDelete(sp->m_prms);
	}
	free(sp);
}

sqlparam* sqlprocFindParam(sqlproc* sp, const char* cp)
{
	int x;
	
	for (x = 0; x < vectorCount(sp->m_prms); x++)
	{
		sqlparam* p = (sqlparam *)vectorElementAt(sp->m_prms, x);
		if (stringIndexOf(p->m_name, cp, 0, true) > -1)
		{
			return p;
		}
	}
	
	return NULL;
}

sqlparam* sqlprocFindParam2(sqlproc* sp, const char* cp1, const char* cp2)
{
	int x;
	
	for (x = 0; x < vectorCount(sp->m_prms); x++)
	{
		sqlparam* p = (sqlparam *)vectorElementAt(sp->m_prms, x);
		if 
		(
			stringIndexOf(p->m_name, cp1, 0, true) > -1 &&
			stringIndexOf(p->m_name, cp2, 0, true) > -1
		)
		{
			return p;
		}
	}
	
	return NULL;
}

static int _sqlprocScanForBody(string* text)
{
	bool lastWasNum = false;
	int x;
	
	for (x = 0; x < text->m_len; x++)
	{
		if (text->m_cstr[x] == ')' && !lastWasNum)
		{
			return x + 1;
		}
		lastWasNum = isdigit(text->m_cstr[x]);
	}

	return -1;
}

static bool _sqlprocParseParameters(sqlproc* sp, string* head, int* errorcode, int* elinenum)
{
	sqllex* lex = sqllexNew(head);
		
	*errorcode = SQLE_OK;
	*elinenum = 0;
	
	if (lex == NULL)
	{
		*errorcode = SQLE_OUTOFMEM;
		return false;
	}
	
	if (lex->m_token == SQL_EOF)
	{
		*errorcode = SQLE_EMPTY;
		sqllexDelete(lex);
		return false;
	}
	
	if (lex->m_token != SQL_CREATE)
	{
		*errorcode = SQLE_EXP_CREATE;
		sqllexDelete(lex);
		return false;
	}
	
	if (! sqllexNext(lex))
	{
		*errorcode = SQLE_SYNTAX;
		*elinenum = lex->m_lineNum;
		sqllexDelete(lex);
		return false;
	}

	if (lex->m_token != SQL_PROC)
	{
		if (lex->m_token == SQL_CURSOR)
		{
			sp->m_isCursor = true;
		}
		else
		{
			*errorcode = SQLE_EXP_PROC;
			sqllexDelete(lex);
			return false;
		}
	}
	
	if (! sqllexNext(lex) || lex->m_token != SQL_ID)
	{
		*errorcode = SQLE_SYNTAX;
		*elinenum = lex->m_lineNum;
		sqllexDelete(lex);
		return false;
	}
	
	if (NULL == (sp->m_name = stringDup(lex->m_lexum)))
	{
		*errorcode = SQLE_OUTOFMEM;
		sqllexDelete(lex);
		return false;
	}
	
	if (! sqllexNext(lex) || lex->m_token != SQL_LPAR)
	{
		*errorcode = SQLE_EXP_LPAR;
		*elinenum = lex->m_lineNum;
		sqllexDelete(lex);
		return false;
	}
	
	while (sqllexNext(lex))
	{
		sqlparam* param;

		if (lex->m_token == SQL_RPAR)
		{
			break;
		}		

		if (lex->m_token == SQL_COMMA)
		{
			if (! sqllexNext(lex))
			{
				break;
			}
		}

		param = sqlparamNew(lex, errorcode);
		
		if (param == NULL)
		{
			*elinenum = lex->m_lineNum;
			sqllexDelete(lex);
			return false;
		}
		
		if (! vectorAdd(sp->m_prms, param))
		{
			*errorcode = SQLE_OUTOFMEM;
			sqllexDelete(lex);
			return false;		
		}
	}
	
	sqllexDelete(lex);
	
	return true;
}

static void _sqlprocSkipWs(stringbuffer* buf, string* body, int* pos)
{
	while (*pos < body->m_len && iswhitespace(body->m_cstr[*pos]))
	{
		stringbufferAppendCh(buf, body->m_cstr[(*pos)++]);
	}
}

static void _sqlprocReallySkipWs(string* body, int* pos)
{
	while (*pos < body->m_len && iswhitespace(body->m_cstr[*pos]))
	{
		(*pos)++;
	}
}

static string* _sqlprocReadToWs(string* body, int* pos)
{
	string* ret;
	stringbuffer* buf = stringbufferNew();
	
	if (buf == NULL)
	{
		return NULL;
	}
	
	while 
	(
		*pos < body->m_len && 
		!iswhitespace(body->m_cstr[*pos]) && 
		body->m_cstr[*pos] != ',' && 
		body->m_cstr[*pos] != ')'
	)
	{
		stringbufferAppendCh(buf, body->m_cstr[(*pos)++]);
	}
	
	ret = stringbufferToString(buf);
	stringbufferDelete(buf);
	
	return ret;
}

static bool _sqlprocParseBody(sqlproc* sp, string* body, int* errorcode)
{
	int pos = 0;
	stringbuffer *buf = stringbufferNew();
	
	if (buf == NULL)
	{
		return false;
	}
	
	while (pos < body->m_len)
	{
		while 
		(
			pos < body->m_len && 
			body->m_cstr[pos] != ':' &&
			body->m_cstr[pos] != '?' &&
			body->m_cstr[pos] != '\'' && 
			body->m_cstr[pos] != '"'
		)
		{
			if (! stringbufferAppendCh(buf, body->m_cstr[pos++]))
			{
				stringbufferDelete(buf);
				return false;
			}
		}
		
		if (pos == body->m_len)
		{
			break;
		}
		
		if (body->m_cstr[pos] == '\'' || body->m_cstr[pos] == '"')
		{
			char exitch = body->m_cstr[pos];
				
			do
			{
				if (! stringbufferAppendCh(buf, body->m_cstr[pos++]))
				{
					stringbufferDelete(buf);
					return false;
				}
			} while (pos < body->m_len && body->m_cstr[pos] != exitch);
			if (! stringbufferAppendCh(buf, body->m_cstr[pos++]))
			{
				stringbufferDelete(buf);
				return false;
			}
		}
		else if (body->m_cstr[pos] == ':' || body->m_cstr[pos] == '?')
		{
			string* name;
			string* stmp;
			sqlparam* prm;
			
			pos++;
			
			_sqlprocReallySkipWs(body, &pos);
			
			if (NULL == (name = _sqlprocReadToWs(body, &pos)))
			{
				stringbufferDelete(buf);
				return false;
			}
			
			stmp = _sqlprocConvert(name, false);
			stringDelete(name);
			
			if (stmp == NULL)
			{
				stringbufferDelete(buf);
				return false;				
			}
			
			name = stmp;
			
			_sqlprocSkipWs(buf, body, &pos);
			
			if
			(
				(body->m_cstr[pos] == 'O' || body->m_cstr[pos] == 'o') &&
				(body->m_cstr[pos + 1] == 'F' || body->m_cstr[pos + 1] == 'f')
			)
			{
				string* name2Raw;
				string* name2;
				
				ASSERT(body->m_cstr[pos + 2] == ' ');
				pos += 3;
				
				_sqlprocReallySkipWs(body, &pos);
				
				if (NULL == (name2Raw = _sqlprocReadToWs(body, &pos)))
				{
					stringDelete(name);
					stringbufferDelete(buf);
					return false;
				}
				
				name2 = _sqlprocConvert(name2Raw, false);
				prm = sqlprocFindParam2(sp, name->m_cstr, name2->m_cstr);
				stringDelete(name2);
				
				if (prm == NULL)
				{
					name2 = _sqlprocConvert(name2Raw, true);
					prm = sqlprocFindParam2(sp, name->m_cstr, name2->m_cstr);
					stringDelete(name2);
				}
				
				stringDelete(name2Raw);				
			}
			else
			{
				prm = sqlprocFindParam(sp, name->m_cstr);
			}
			
			ASSERT(prm != NULL);
			
			stringDelete(name);

			stringbufferAppendCh(buf, '?');
			stringbufferAppendString(buf, prm->m_name);
			stringbufferAppendCh(buf, ' ');
			
			_sqlprocSkipWs(buf, body, &pos);
			
			if (pos >= body->m_len)
			{
				continue;				
			}
			
			if (body->m_cstr[pos] != 'I' && body->m_cstr[pos] != 'i')
			{
				continue;
			}
			
			if (stringIndexOf(body, "INDICATOR", pos, true) > -1)
			{
				stringbufferAppend(buf, "INDICATOR ?");
				pos = stringIndexOfCh(body, ':', pos) + 1;
				if (pos == 0)
				{
					pos = stringIndexOfCh(body, '?', pos) + 1;
				}
				
				stmp = _sqlprocReadToWs(body, &pos);
				name = _sqlprocConvert(stmp, false);
				stringDelete(stmp);
				
				prm->m_indicator = name;
				
				if
				(
					(body->m_cstr[pos] == 'O' || body->m_cstr[pos] == 'o') &&
					(body->m_cstr[pos + 1] == 'F' || body->m_cstr[pos + 1] == 'f')
				)
				{
					ASSERT(body->m_cstr[pos + 2] == ' ');
					
					pos += 3;
					stmp = _sqlprocReadToWs(body, &pos);
					name = _sqlprocConvert(stmp, false);
					stringDelete(stmp);
					
					stmp = prm->m_indicator;
					
					prm->m_indicator = stringCat(stmp, name->m_cstr, name->m_len);
					stringDelete(stmp);
					stringDelete(name);
				}				
			}
		}
	}	
	
	sp->m_body = stringbufferToString(buf);
	
	stringbufferDelete(buf);
	
	if (sp->m_body == NULL)
	{
		*errorcode = SQLE_OUTOFMEM;
		return false;
	}
	
	return true;
}

static string* _sqlprocConvert(string* name, bool special)
{
	string* ret;
	stringbuffer *buf;
	vector* parts;
	int x;
	
	if (special)
	{
		if (stringIndexOfCh(name, '-', 0) < 0)
		{
			return stringCat(name, "_", 1);
		}
		else
		{
			return stringReplace(name, "-", "_");
		}
	}

	if (stringIndexOfCh(name, '-', 0) < 0)
	{
		return stringDup(name);
	}
	
	if (NULL == (parts = stringSplit(name, "-")))
	{
		return NULL;
	}

	if (NULL == (buf = stringbufferNew()))
	{
		stringDeleteSplitVector(parts);
		return NULL;
	}

	for (x = 0; x < vectorCount(parts); x++)
	{
		int y;
		string* s = (string*)vectorElementAt(parts, x);
		
		if (x == 0 && isdigit(stringCharAt(s, 0)))
		{
			stringbufferAppendCh(buf, '_');
		}

		if (((string*)s)->m_len > 0)
		{
			stringbufferAppendCh(buf, (char)toupper(((string*)s)->m_cstr[0]));
		}

		for (y = 1; y < s->m_len; y++)
		{
			stringbufferAppendCh(buf, (char)toupper(s->m_cstr[y]));
		}
	}

	stringDeleteSplitVector(parts);
	ret = stringbufferToString(buf);
	stringbufferDelete(buf);
	
	return ret;
}

#ifdef DEBUG
void sqlprocValidateMem(sqlproc* sp)
{
	ASSERT_MEM(sp, sizeof(sqlproc));
	
	stringValidateMem(sp->m_name);
	stringValidateMem(sp->m_text);
	stringValidateMem(sp->m_body);
	
	if (sp->m_prms != NULL)
	{
		int x;
		
		vectorValidateMem(sp->m_prms);
		
		for (x = 0; x < vectorCount(sp->m_prms); x++)
		{
			sqlparamValidateMem((sqlparam *)vectorElementAt(sp->m_prms, x));
		}
	}	
}
#endif
