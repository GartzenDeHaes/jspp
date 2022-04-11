#include "request.h"
#include <spl/integer.h>
#include <spl/net/packet.h>
#include <spl/real.h>

#include "sqllex.h"

static request* _requestNew()
{
	request* req = (request *)malloc(sizeof(request));
	
	if (req == NULL)
	{
		return NULL;
	}
	
	req->m_name = NULL;
	req->m_items = NULL;
	req->m_transid = 0;
	req->m_bytes = 0;
	req->m_userId = NULL;
	req->m_password = NULL;
	req->m_itemCount = 0;
	
	if (NULL == (req->m_items = (requestItem*)malloc(sizeof(requestItem))))
	{
		requestDelete(req);
		return NULL;		
	}
	req->m_items->m_name = NULL;
	req->m_items->m_next = NULL;
	req->m_items->m_data.m_str = NULL;
	req->m_items->m_type = 0;
	
	return req;
}

request* requestNew(stringbuffer* buf)
{
	packet* pkt;
	requestItem* item;
	request* req = _requestNew();
	
	if (req == NULL)
	{
		return NULL;
	}
	
	if (NULL == (pkt = packetNew(buf)))
	{
		requestDelete(req);
		return NULL;
	}
	
	req->m_bytes = pkt->m_buf->m_used;
	
	if (packetNextType(pkt) != 'S')
	{
		packetDelete(pkt);
		requestDelete(req);
		return NULL;		
	}
	
	if (NULL == (req->m_name = packetReadString(pkt)))
	{
		packetDelete(pkt);
		requestDelete(req);
		return NULL;		
	}
	
	if (! packetIsValidNext(pkt))
	{
		packetDelete(pkt);
		requestDelete(req);
		return NULL;
	}
	
	if (NULL == (req->m_userId = packetReadString(pkt)))
	{
		packetDelete(pkt);
		requestDelete(req);
		return NULL;		
	}
	
	if (NULL == (req->m_password = packetReadString(pkt)))
	{
		packetDelete(pkt);
		requestDelete(req);
		return NULL;		
	}
	
	while (! packetIsEofOrError(pkt))
	{
		req->m_itemCount++;
		
		item = (requestItem*)malloc(sizeof(requestItem));
		if (item == NULL)
		{
			packetDelete(pkt);
			requestDelete(req);
			return NULL;
		}

		item->m_next = NULL;
		item->m_data.m_str = NULL;
		
		if (! packetIsValidNext(pkt))
		{
			packetDelete(pkt);
			requestDelete(req);
			return NULL;
		}
		
		if (NULL == (item->m_name = packetReadString(pkt)))
		{
			packetDelete(pkt);
			requestDelete(req);
			return NULL;
		}
		
		item->m_type = packetNextType(pkt);
		
		if (item->m_type == 'S')
		{
			item->m_data.m_str = packetReadString(pkt);
		}
		else if (item->m_type == 'd')
		{
			item->m_data.m_real = packetReadDouble(pkt);
		}
		else if (item->m_type == 'I')
		{
			item->m_data.m_int = packetReadInt(pkt);
		}
		else if (item->m_type == 'L')
		{
			item->m_data.m_long = packetReadLong(pkt);
		}
		else
		{
			packetDelete(pkt);
			requestDelete(req);
			return NULL;
		}
		
		item->m_next = req->m_items;
		req->m_items = item;
	}
	
	packetDelete(pkt);
	
	if (NULL != (item = requestGetItem(req, "%%TRANSACTION-ID%%")))
	{
		ASSERT (item->m_type == 'L');
		req->m_transid = item->m_data.m_long;
	}
	
	return req;
}

request* requestNewFromExecSql(string* sql)
{
	request* req = _requestNew();
	sqllex* lex;
	
	if (req == NULL)
	{
		return NULL;
	}
	
	if (NULL == (lex = sqllexNew(sql)))
	{
		requestDelete(req);
		return NULL;
	}
	
	if (! sqllexMatchToken(lex, SQL_EXEC))
	{
		sqllexDelete(lex);
		requestDelete(req);
		return NULL;
	}
	
	if (NULL == (req->m_name = stringDup(lex->m_lexum)))
	{
		sqllexDelete(lex);
		requestDelete(req);
		return NULL;
	}
	
	if (! sqllexMatchToken(lex, SQL_ID))
	{
		sqllexDelete(lex);
		requestDelete(req);
		return NULL;
	}

	if (lex->m_token == SQL_DOT)
	{
		string* str = req->m_name;
		req->m_name = stringCat(str, ".", 1);
		stringDelete(str);
		sqllexMatchToken(lex, SQL_DOT);
		
		str = req->m_name;
		req->m_name = stringCat(req->m_name, lex->m_lexum->m_cstr, lex->m_lexum->m_len);
		stringDelete(str);
		
		if (! sqllexMatchToken(lex, SQL_ID))
		{
			sqllexDelete(lex);
			requestDelete(req);
			return NULL;
		}
	}

	while (lex->m_token != SQL_EOF)
	{
		requestItem* item;
		
		req->m_itemCount++;
		
		if (lex->m_token == SQL_COMMA)
		{
			if (! sqllexNext(lex))
			{
				sqllexDelete(lex);
				requestDelete(req);
				return NULL;
			}
		}
		
		item = (requestItem*)malloc(sizeof(requestItem));
		if (item == NULL)
		{
			sqllexDelete(lex);
			requestDelete(req);
			return NULL;
		}

		item->m_name = NULL;
		item->m_next = NULL;
		item->m_data.m_str = NULL;
		
		if (lex->m_token == SQL_STRLIT)
		{
			item->m_type = 'S';
			
			ASSERT(lex->m_lexum->m_cstr[0] != '\'' && lex->m_lexum->m_cstr[0] != '"');
			
			/*lex->m_lexum->m_cstr[lex->m_lexum->m_len - 1] = '\0';*/
			item->m_data.m_str = stringNew(lex->m_lexum->m_cstr);
		}
		else if (lex->m_token == SQL_NUM)
		{
			if (stringIsFloat(lex->m_lexum))
			{
				item->m_type = 'd';
				item->m_data.m_real = float64Parse(lex->m_lexum->m_cstr, lex->m_lexum->m_len);
			}
			else
			{
				item->m_type = 'I';
				item->m_data.m_int = int32Parse(lex->m_lexum->m_cstr, lex->m_lexum->m_len, 10);
			}
		}
		else
		{
			sqllexDelete(lex);
			requestDelete(req);
			return NULL;
		}
		
		item->m_next = req->m_items;
		req->m_items = item;
		
		sqllexNext(lex);
	}

	sqllexDelete(lex);
	
	return req;
}

void requestDelete(request* req)
{
	requestItem* item = req->m_items;

	requestValidateMem(req);
	
	while (item != NULL)
	{
		requestItem* next = item->m_next;
		if (item->m_name != NULL)
		{
			stringDelete(item->m_name);
		}
		if (item->m_type == 'S')
		{
			stringDelete(item->m_data.m_str);
		}
		free(item);
		item = next;
	}
	
	if (req->m_name != NULL)
	{
		stringDelete(req->m_name);
	}
	
	if (req->m_userId != NULL)
	{
		stringDelete(req->m_userId);
	}
	
	if (req->m_password != NULL)
	{
		stringDelete(req->m_password);
	}
		
	free(req);
}

requestItem* requestGetItem(request* req, const char* name)
{
	requestItem* item = req->m_items;
	
	while (item != NULL)
	{
		requestItem* next = item->m_next;
		if (item->m_name == NULL)
		{
			break;
		}
		if (stringIsEqual(item->m_name, name, false))
		{
			return item;
		}
		item = next;
	}
	
	return NULL;
}

requestItem* requestGetItemAt(request* req, int idx)
{
	requestItem* item = req->m_items;
	int x = 0;
	
	while (x < req->m_itemCount && x < idx)
	{
		item = item->m_next;
		if (item == NULL)
		{
			break;
		}
	}
	
	if (x < req->m_itemCount)
	{
		return item;
	}
	
	return NULL;
}

string* requestItemToString(requestItem* item)
{
	switch(item->m_type)
	{
		case 'S':
			return stringDup(item->m_data.m_str);
		case 'd':
			return float64ToString(item->m_data.m_real);
		case 'I':
			return int32ToString(item->m_data.m_int);
		case 'L':
			return int64ToString(item->m_data.m_long);
		default:
			return stringNew("unknown");
	}
}

void requestItemToCharBuf(requestItem* item, char *buf, int buflen)
{
	string* s;
	
	switch(item->m_type)
	{
		case 'S':
			StrCpyLen(buf, item->m_data.m_str->m_cstr, buflen);
			break;
		case 'd':
			s = float64ToString(item->m_data.m_real);
			StrCpyLen(buf, s->m_cstr, buflen);
			stringDelete(s);
			break;
		case 'I':
			s = int32ToString(item->m_data.m_int);
			StrCpyLen(buf, s->m_cstr, buflen);
			stringDelete(s);
			break;
		case 'L':
			s = int64ToString(item->m_data.m_long);
			StrCpyLen(buf, s->m_cstr, buflen);
			stringDelete(s);
			break;
	}
}

int32 requestItemToInt(requestItem* item)
{
	switch(item->m_type)
	{
		case 'S':
			if (stringIsInt(item->m_data.m_str))
			{
				return int32Parse(item->m_data.m_str->m_cstr, item->m_data.m_str->m_len, 10);
			}
			if (stringIsFloat(item->m_data.m_str))
			{
				return (int32)float64Parse(item->m_data.m_str->m_cstr, item->m_data.m_str->m_len);
			}
			return 0;
		case 'd':
			return (int32)item->m_data.m_real;
		case 'I':
			return item->m_data.m_int;
		case 'L':
			return (int32)item->m_data.m_long;
		default:
			return 0;
	}
}

int64 requestItemToLong(requestItem* item)
{
	switch(item->m_type)
	{
		case 'S':
			if (stringIsInt(item->m_data.m_str))
			{
				return int32Parse(item->m_data.m_str->m_cstr, item->m_data.m_str->m_len, 10);
			}
			if (stringIsFloat(item->m_data.m_str))
			{
				return (int64)float64Parse(item->m_data.m_str->m_cstr, item->m_data.m_str->m_len);
			}
			return 0;
		case 'd':
			return (int64)item->m_data.m_real;
		case 'I':
			return (int64)item->m_data.m_int;
		case 'L':
			return item->m_data.m_long;
		default:
			return 0;
	}
}

float64 requestItemToDouble(requestItem* item)
{
	switch(item->m_type)
	{
		case 'S':
			if (stringIsFloat(item->m_data.m_str))
			{
				return float64Parse(item->m_data.m_str->m_cstr, item->m_data.m_str->m_len);
			}
			return 0;
		case 'd':
			return item->m_data.m_real;
		case 'I':
			return (float64)item->m_data.m_int;
		case 'L':
			return (float64)item->m_data.m_long;
		default:
			return 0;
	}
}

#ifdef DEBUG

void requestValidateMem(request* req)
{
	requestItem* item;
	
	ASSERT_MEM(req, sizeof(request));
	
	item = req->m_items;
	
	while (item != NULL)
	{
		requestItem* next = item->m_next;
		ASSERT_MEM(item, sizeof(requestItem));
		
		if (item->m_name != NULL)
		{
			stringValidateMem(item->m_name);
		}
		if (item->m_type == 'S')
		{
			stringValidateMem(item->m_data.m_str);
		}
		item = next;
	}
	
	if (req->m_name != NULL)
	{
		stringValidateMem(req->m_name);
	}	
}

#endif
