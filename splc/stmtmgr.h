#ifndef _stmtmgr_h
#define _stmtmgr_h

#include <spl/types.h>
#include <spl/debug.h>
#include <spl/collection/hashtable.h>
#include <spl/string.h>
#include <spl/stringbuffer.h>
#include <spl/collection/vector.h>

#include "preparedstatement.h"
#include "request.h"
#include "users.h"

typedef struct _stmtmgr
{
	hashtable* m_stmts;
	users* m_users;
} stmtmgr;

stmtmgr* stmtmgrNew();
void stmtmgrDelete(stmtmgr* mgr);

void stmtmgrProcessSql(stmtmgr* mgr, string* system, string* userId, string* sql, int64 transId, stringbuffer* xml);
bool stmtmgrExec(stmtmgr* mgr, string* system, request* req, stringbuffer* xml);

vector* stmtmgrListSystems(stmtmgr* mgr);
vector* stmtmgrGetProcedures(stmtmgr* mgr);
preparedstatement* stmtmgrGetProcedure(stmtmgr* mgr, string* system, string* name);

#ifdef DEBUG
void stmtmgrValidateMem(stmtmgr* mgr);
#else
#define stmtmgrValidateMem(m);
#endif

#endif
