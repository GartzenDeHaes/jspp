#include "conf.h"
#include "stmtmgr.h"

extern msgline g_status;

extern bool _writeError(stringbuffer* xml, const char* cp1, const char* cp2);
extern bool _writeErrorCode(stringbuffer* xml, const char* cp1, const int i);

static void begintransRequest(stringbuffer* xml);
static void aborttransRequest(int64 transId, stringbuffer* xml);
static void comittransRequest(int64 transId, stringbuffer* xml);
static bool resumetrans(int64 transId, stringbuffer* xml);
static bool suspendtrans(int64 transId, stringbuffer* xml);

stmtmgr* stmtmgrNew()
{
	stmtmgr* mgr;
	
	mgr = (stmtmgr *)malloc(sizeof(stmtmgr));
	if (mgr == NULL)
	{
		return NULL;
	}
	
	if (NULL == (mgr->m_stmts = hashtableNew()))
	{
		free(mgr);
		return NULL;
	}
	mgr->m_stmts->m_keyIgnoreCase = true;
	
	if (NULL == (mgr->m_users = usersNew()))
	{
		hashtableDelete(mgr->m_stmts, false);
		free(mgr);
		return NULL;
	}
	
	/* Load on use instead of startup */
	/*if (! loadAllProcedures(mgr->m_stmts))
	{
		hashtableDelete(mgr->m_stmts, false);
		free(mgr);
		return NULL;
	}*/
	
	return mgr;
}

void stmtmgrDelete(stmtmgr* mgr)
{
	hashitem* hi;
	int x;

	for (x = 0; x < mgr->m_stmts->m_size; x++)
	{
		hi = mgr->m_stmts->m_table[x];
		
		while (hi != NULL)
		{
			preparedstatementDelete((preparedstatement *)hi->m_value);
			hi->m_value = NULL;
			hi = hi->m_next;
		}
	}	

	usersDelete(mgr->m_users);
	hashtableDelete(mgr->m_stmts, false);
	free(mgr);
}

void stmtmgrProcessSql
(
	stmtmgr* mgr, 
	string* system, 
	string* userId,
	string* sql, 
	int64 transId,
	stringbuffer* xml
)
{
	preparedstatement* stmt;
	int errorcode;
	
	if (stringIsEqual(sql, "BEGIN TRANSACTION", true))
	{
		if (transId != 0)
		{
			_writeError(xml, "Nested transactions not supported", "");
			return;
		}
		begintransRequest(xml);
	}
	else if (stringIsEqual(sql, "COMMIT TRANSACTION", true))
	{
		if (transId == 0)
		{
			_writeError(xml, "Transaction not in progress", "");
			return;
		}
		comittransRequest(transId, xml);
	}
	else if (stringIsEqual(sql, "ABORT TRANSACTION", true))
	{
		if (transId == 0)
		{
			_writeError(xml, "Transaction not in progress", "");
			return;
		}
		aborttransRequest(transId, xml);
	}
	else if (stringStartsWith(sql, "GRANT ", true))
	{
		if (! usersParseGrant(mgr->m_users, system, sql))
		{
			_writeError(xml, "GRANT failed", "");
		}
		
		stringbufferAppend(xml, "<row message='OK' />\r\n");
		return;
	}
	else if (stringStartsWith(sql, "CREATE LOGON", true))
	{
		if (! usersParseCreateLogon(mgr->m_users, sql))
		{
			_writeError(xml, "CREATE LOGON failed", "");
		}
		
		stringbufferAppend(xml, "<row message='OK' />\r\n");
		return;
	}
	else if (stringStartsWith(sql, "EXEC ", true))
	{
		request* req;
		
		if (transId != 0)
		{
			if (! resumetrans(transId, xml))
			{
				return;
			}
		}

		if (NULL == (req = requestNewFromExecSql(sql)))
		{
			_writeError(xml, "syntax error or out of memory", "");
			return;
		}
		
		if (NULL == (req->m_userId = stringDup(userId)))
		{
			requestDelete(req);
			_writeError(xml, "syntax error or out of memory", "");
			return;
		}
		
		stmtmgrExec(mgr, system, req, xml);
		
		requestDelete(req);
		
		if (transId != 0)
		{
			suspendtrans(transId, xml);
		}

		return;
	}
	else if 
	(
		stringStartsWith(sql, "CREATE CURSOR", true) || 
		stringStartsWith(sql, "CREATE PROCEDURE", true) || 
		stringStartsWith(sql, "CREATE PROC", true)
	)
	{
		if (! usersIsAdmin(mgr->m_users, userId))
		{
			_writeError(xml, "unauthorized", "");
			return;
		}
		
		if (NULL == (stmt = preparedstatementNew(system, sql, hashtableCount(mgr->m_stmts), &errorcode)))
		{
			_writeErrorCode(xml, "prepared statement failed", errorcode);
			return;
		}
		
		if (hashtableContainsKey(mgr->m_stmts, stmt->m_fqname->m_cstr))
		{
			preparedstatementDelete((preparedstatement*)hashtableGet(mgr->m_stmts, stmt->m_fqname->m_cstr));
			deleteProcedure(system, stmt->m_name);
		}
		
		if (! hashtablePut(mgr->m_stmts, stmt->m_fqname->m_cstr, stmt, false))
		{
			_writeError(xml, "out of memory", "");
			preparedstatementDelete(stmt);
			return;
		}
		
		if (! saveProcedure(system, stmt->m_name, sql))
		{
			_writeError(xml, "error writing procedure to local database", "");
			preparedstatementDelete(stmt);
			return;
		}
		
		stringbufferAppend(xml, "<row message='OK' />\r\n");
	}
	else if 
	(
		stringStartsWith(sql, "DROP PROCEDURE", true) || 
		stringStartsWith(sql, "DROP PROC", true)
	)
	{
		preparedstatement* stmt;
		string* fqname;
		string* system;
		string* name = stringSubstringFrom(sql, stringIndexOfCh(sql, ' ', 6) + 1);
		int pos = stringIndexOf(name, ".", 0, false);

		if (! usersIsAdmin(mgr->m_users, userId))
		{
			_writeError(xml, "unauthorized", "");
			return;
		}
		
		if (pos < 0)
		{
			_writeError(xml, "procedure name must be in the format SYSTEM.PROCNAME", "");
			return;
		}

		fqname = stringDup(name);
		system = stringSubstring(fqname, 0, pos);
		stringDelete(name);
		name = stringSubstringFrom(fqname, pos + 1);

		stmt = (preparedstatement*)hashtableGet(mgr->m_stmts, fqname->m_cstr);
		
		if (stmt == NULL)
		{
			_writeError(xml, "procedure not found ", fqname->m_cstr);
			stringDelete(fqname);
			stringDelete(name);
			stringDelete(system);
			return;
		}

		hashtablePut(mgr->m_stmts, fqname->m_cstr, NULL, false);

		if (! deleteProcedure(system, name))
		{
			_writeError(xml, "couldn't delete in sqlite, system in inconsistant state ", fqname->m_cstr);
			stringDelete(fqname);
			stringDelete(name);
			stringDelete(system);
			return;
		}

		stringDelete(fqname);
		stringDelete(name);
		stringDelete(system);

		preparedstatementDelete(stmt);
		
		stringbufferAppend(xml, "<row message='OK' />\r\n");
	}
	else
	{
		request* req;
		string* dummySql;
		
		if (! usersIsAdmin(mgr->m_users, userId))
		{
			_writeError(xml, "unauthorized", "");
			return;
		}
		
		if (NULL == (stmt = preparedstatementNewImmediate(system, sql, &errorcode)))
		{
			_writeErrorCode(xml, "prepared statement failed", errorcode);
			return;
		}

		if (NULL == (dummySql = stringNew("EXEC IMM")))
		{
			preparedstatementDelete(stmt);
			_writeError(xml, "out of memeory", "");
			return;
		}

		req = requestNewFromExecSql(dummySql);
		stringDelete(dummySql);
		
		if (NULL == req)
		{
			preparedstatementDelete(stmt);
			_writeError(xml, "out of memeory", "");
			return;
		}

		if (transId != 0)
		{
			if (resumetrans(transId, xml))
			{
				preparedstatementDelete(stmt);
				requestDelete(req);
				return;
			}
		}

		preparedstatementExec(stmt, req, xml);
		
		if (transId != 0)
		{
			suspendtrans(transId, xml);
		}

		preparedstatementDelete(stmt);
		requestDelete(req);
	}
}

static preparedstatement* stmtmgrGetStmt
(
	hashtable* stmts,
	string* system, 
	string* name, 
	string* fqname
)
{
	preparedstatement* stmt = (preparedstatement*)hashtableGet(stmts, fqname->m_cstr);

	if (stmt == NULL)
	{
		int pos = stringIndexOf(name, ".", 0, false);
		
		if (pos > -1)
		{
			string *tmp = stringNew(&name->m_cstr[pos+1]);
			if (updateProcedure(stmts, system, tmp))
			{
				stmt = (preparedstatement*)hashtableGet(stmts, fqname->m_cstr);
			}
			stringDelete(tmp);
		}
		else
		{
			if (updateProcedure(stmts, system, name))
			{
				stmt = (preparedstatement*)hashtableGet(stmts, fqname->m_cstr);
			}
		}
	}
	
	return stmt;
}

bool stmtmgrExec
(
	stmtmgr* mgr, 
	string* _system, 
	request* req,
	stringbuffer* xml
)
{
	preparedstatement* stmt;
	string* stmp;
	string* fqname;
	string* name;
	string* system;
	string* userId = req->m_userId;
	int pos = stringIndexOf(req->m_name, ".", 0, false);
	
	if (pos < 0)
	{
		system = stringDup(_system);
		name = stringDup(req->m_name);
		
		if (NULL == (stmp = stringCat(system, ".", 1)))
		{
			_writeError(xml, "out of memory", "");
			return true;
		}
		if (NULL == (fqname = stringCat(stmp, name->m_cstr, name->m_len)))
		{
			stringDelete(stmp);
			_writeError(xml, "out of memory", "");
			return true;
		}
		
		stringDelete(stmp);
	}
	else
	{
		fqname = stringDup(req->m_name);
		system = stringSubstring(fqname, 0, pos);
		name = stringSubstringFrom(fqname, pos + 1);
	}

	if (! usersHasPerm(mgr->m_users, userId, fqname))
	{
		stringDelete(name);
		stringDelete(fqname);
		stringDelete(system);
		_writeError(xml, "permission denied", "");
		return true;		
	}

	stmt = stmtmgrGetStmt(mgr->m_stmts, system, name, fqname);
	
	stringDelete(name);
	stringDelete(system);

	if (stmt == NULL)
	{
		_writeError(xml, "procedure not found ", fqname->m_cstr);
		stringDelete(fqname);
		return false;
	}

	stringDelete(fqname);

	if (req->m_transid != 0)
	{
		if (! resumetrans(req->m_transid, xml))
		{
			return false;
		}
	}

	preparedstatementExec(stmt, req, xml);

	if (req->m_transid != 0)
	{
		suspendtrans(req->m_transid, xml);
	}
	
	return true;
}

vector* stmtmgrListSystems(stmtmgr* mgr)
{
	hashtable* ht;
	vector* vec;
	hashitem* hi;
	int x;

	if (NULL == (vec = vectorNew()))
	{
		return NULL;
	}
	
	if (NULL == (ht = hashtableNew()))
	{
		vectorDelete(vec);
		return NULL;
	}
	ht->m_keyIgnoreCase = true;
	
	for (x = 0; x < mgr->m_stmts->m_size; x++)
	{
		hi = mgr->m_stmts->m_table[x];
		
		while (hi != NULL)
		{
			preparedstatement *stmt = (preparedstatement *)hi->m_value;
			
			if (! hashtableContainsKey(ht, stmt->m_system->m_cstr))
			{
				hashtablePut(ht, stmt->m_system->m_cstr, NULL, false);
				vectorAdd(vec, stringDup(stmt->m_system));
			}
						
			hi = hi->m_next;
		}
	}
	
	hashtableDelete(ht, false);
	
	return vec;
}

vector* stmtmgrGetProcedures(stmtmgr* mgr)
{
	vector* vec;
	hashitem* hi;
	int x;

	if (NULL == (vec = vectorNew()))
	{
		return NULL;
	}
	
	if (hashtableCount(mgr->m_stmts) == 0)
	{
		if (! loadAllProcedures(mgr->m_stmts))
		{
			return NULL;
		}		
	}
	
	for (x = 0; x < mgr->m_stmts->m_size; x++)
	{
		hi = mgr->m_stmts->m_table[x];
		
		while (hi != NULL)
		{
			vectorAdd(vec, (preparedstatement *)hi->m_value);						
			hi = hi->m_next;
		}
	}
		
	return vec;
}

preparedstatement* stmtmgrGetProcedure(stmtmgr* mgr, string* system, string* name)
{
	string* stmp;
	string* stmp2;
	preparedstatement* stmt;

	if (NULL == (stmp = stringCat(system, ".", 1)))
	{
		return NULL;
	}
	if (NULL == (stmp2 = stringCat(stmp, name->m_cstr, name->m_len)))
	{
		stringDelete(stmp);
		return NULL;
	}
	
	stringDelete(stmp);

	stmt = (preparedstatement*)hashtableGet(mgr->m_stmts, stmp2->m_cstr);

	stringDelete(stmp2);
	
	return stmt;
}

#ifdef DEBUG
void stmtmgrValidateMem(stmtmgr* mgr)
{
	hashitem* hi;
	int x;

	ASSERT_MEM(mgr, sizeof(stmtmgr));
	hashtableValidateMem(mgr->m_stmts);
	usersValidateMem(mgr->m_users);
	
	for (x = 0; x < mgr->m_stmts->m_size; x++)
	{
		hi = mgr->m_stmts->m_table[x];
		
		while (hi != NULL)
		{
			preparedstatementValidateMem((preparedstatement *)hi->m_value);
			hi = hi->m_next;
		}
	}	
}
#endif

/* name collision in cextdecs.h */
#undef DEBUG

#ifdef __TANDEM
#include <cextdecs.h>
#endif

static bool resumetrans(int64 transId, stringbuffer* xml)
{
#ifdef __TANDEM
	int status = TMF_RESUME_(transId);
	if (status != 0)
	{
		StrCpyLen(g_status.msg, "TMF_RESUME_ error", sizeof(g_status.msg));
		_writeErrorCode(xml, "TMF_RESUME_ error ", status);
		return false;
	}
#endif
	
	return true;
}

static bool suspendtrans(int64 transId, stringbuffer* xml)
{
#ifdef __TANDEM
	int status = TMF_SUSPEND_(&transId);
	if (status != 0)
	{
		StrCpyLen(g_status.msg, "TMF_SUSPEND_ error", sizeof(g_status.msg));
		_writeErrorCode(xml, "TMF_SUSPEND_ error ", status);
		return false;
	}
	
	RESUMETRANSACTION(NULL);
#endif
	
	return true;
}

static void begintransRequest(stringbuffer* xml)
{
#ifdef __TANDEM
	int64 transid;
	long tmftag;
	int status = TMF_TXBEGIN_(15, &tmftag);
	if (status != 0)
	{
		_writeErrorCode(xml, "TMF_TXBEGIN_ error ", status);
	}
	else
	{
		short txhandle;
		int status = TMF_TXHANDLE_FROM_BEGINTAG_(tmftag, &txhandle);
		if (status != 0)
		{
			StrCpyLen(g_status.msg, "TMF_TXHANDLE_FROM_BEGINTAG_ error", sizeof(g_status.msg));
			_writeErrorCode(xml, "TMF_TXHANDLE_FROM_BEGINTAG_ error ", status);
			return;
		}

		status = TMF_GET_TX_ID_(&txhandle, &transid);
		if (status != 0)
		{
			StrCpyLen(g_status.msg, "TMF_GET_TX_ID_ error ", sizeof(g_status.msg));
			_writeErrorCode(xml, "TMF_GET_TX_ID_ error ", status);
			return;
		}
		
		if (suspendtrans(transid, xml))
		{
			stringbufferAppend(xml, "<row message='OK' transid='");
			stringbufferAppendLong(xml, transid);
			stringbufferAppend(xml, "' />\r\n");
		}
	}
	
	RESUMETRANSACTION(NULL);
#else
	stringbufferAppend(xml, "<row message='OK' transid='0' />\r\n");
#endif
}

static void aborttransRequest(int64 transId, stringbuffer* xml)
{
#ifdef __TANDEM
	int status;
	
	if (! resumetrans(transId, xml))
	{
		return;
	}
	
	status = ABORTTRANSACTION();
	if (status != 0)
	{
		StrCpyLen(g_status.msg, "ABORTTRANSACTION error", sizeof(g_status.msg));
		_writeErrorCode(xml, "ABORTTRANSACTION error ", status);
	}
	else
	{		
		stringbufferAppend(xml, "<row message='OK' />\r\n");
	}
	
#else
	stringbufferAppend(xml, "<row message='OK' />\r\n");
#endif
}

static void comittransRequest(int64 transId, stringbuffer* xml)
{
#ifdef __TANDEM
	int status;
	
	if (! resumetrans(transId, xml))
	{
		return;
	}
	
	status = ENDTRANSACTION();
	if (status != 0)
	{
		StrCpyLen(g_status.msg, "ENDTRANSACTION error", sizeof(g_status.msg));
		_writeErrorCode(xml, "ENDTRANSACTION error ", status);
	}
	else
	{
		stringbufferAppend(xml, "<row message='OK' />\r\n");
	}
	
#else
	stringbufferAppend(xml, "<row message='OK' />\r\n");
#endif
}

