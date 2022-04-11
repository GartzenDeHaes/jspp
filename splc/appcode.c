#include <spl/stringbuffer.h>
#include "request.h"
#include "users.h"

#ifdef __TANDEM
#include <sql.h>
#endif

extern bool _writeError(stringbuffer* xml, const char* cp1, const char* cp2);
extern bool _writeErrorCode(stringbuffer* xml, const char* cp1, const int i);

static bool traSelValidateRequest(request *req, stringbuffer* xml);
static void traSel(request *req, stringbuffer* xml);

bool serviceRequest(users* u, request *req, stringbuffer* xml)
{
	if (! usersIsPerm(u, req->m_name))
	{
		_writeError(xml, "stored procedure not found ", req->m_name->m_cstr);
		return false;
	}
	
	if (! usersHasPerm(u, req->m_userId, req->m_name))
	{
		_writeError(xml, "permission denied", "");
		return false;		
	}
	
	if (stringIsEqual(req->m_name, "traSel", false))
	{
		if (! traSelValidateRequest(req, xml))
		{
			_writeError(xml, "invalid parameter(s)", "");
			return false;
		}
		traSel(req, xml);
		
		return true;
	}
	else
	{
		return false;
	}
}

static bool traSelValidateRequest(request *req, stringbuffer* xml)
{
	return requestHasItem(req, "TRA_ID") &&
		requestGetItem(req, "TRA_ID")->m_type == 'L';
}

static void traSel(request *req, stringbuffer* xml)
{
#ifdef __TANDEM
	EXEC SQL BEGIN DECLARE SECTION;
	
	short sqlcode;
	int64 TRA_ID = requestGetItem(req, "TRA_ID")->m_data.m_long;
	
	EXEC SQL END DECLARE SECTION;

	EXEC SQL DECLARE curse CURSOR FOR
		SELECT TRA_ID FROM =TRAFILE 
		WHERE TRA_ID = :TRA_ID
		FOR BROWSE ACCESS;

	EXEC SQL OPEN curse;

	while (sqlcode == 0)
	{
		EXEC SQL FETCH curse INTO :TRA_ID;
		
		if (sqlcode == 0)
		{
			stringbufferAppend(xml, "<row TRA_ID='");
			stringbufferAppendLong(xml, TRA_ID);
			stringbufferAppend(xml, "' />\r\n");
		}
	}
	
	if (sqlcode != 100)
	{
		_writeErrorCode(xml, "SQL ERROR ", sqlcode);
	}
	
	EXEC SQL CLOSE curse;
	
	/*	
	EXEC SQL 
		SELECT 
			TRA_ID 
		INTO 
			:TRA_ID
		FROM =TRAFILE 
		WHERE 
			TRA_ID = :TRA_ID
		FOR BROWSE ACCESS;

	stringbufferAppend(xml, "<row TRA_ID='");
	stringbufferAppendLong(xml, TRA_ID);
	stringbufferAppend(xml, "' />\r\n");
	*/
	
#else

	stringbufferAppend(xml, "<row TRA_ID='");
	stringbufferAppendLong(xml, requestGetItem(req, "TRA_ID")->m_data.m_long);
	stringbufferAppend(xml, "' />\r\n");	
#endif	
}
