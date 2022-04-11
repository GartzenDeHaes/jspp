#ifndef _request_h
#define _request_h

#include <spl/types.h>
#include <spl/debug.h>
#include <spl/stringbuffer.h>

typedef struct _requestItem
{
	string* m_name;
	char m_type;
	struct _requestItem* m_next;
	
	union
	{
		int m_int;
		int64 m_long;
		double m_real;
		string* m_str;
	} m_data;
} requestItem;

typedef struct _request
{
	string* m_name;
	int64 m_transid;
	string* m_userId;
	string* m_password;
	requestItem* m_items;
	int m_itemCount;
	int m_bytes;	
} request;

request* requestNew(stringbuffer* buf);
request* requestNewFromExecSql(string* sql);
void requestDelete(request* req);

requestItem* requestGetItem(request* req, const char* name);
requestItem* requestGetItemAt(request* req, int idx);
#define requestHasItem(req, name) (NULL != requestGetItem(req, name))

string* requestItemToString(requestItem* item);
void requestItemToCharBuf(requestItem* item, char *buf, int buflen);
int32 requestItemToInt(requestItem* item);
int64 requestItemToLong(requestItem* item);
float64 requestItemToDouble(requestItem* item);

#ifdef DEBUG
void requestValidateMem(request* req);
#else
#define requestValidateMem(r)
#endif

#endif
