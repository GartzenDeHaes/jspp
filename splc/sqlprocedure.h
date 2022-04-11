#ifndef _sqlprocedure_h
#define _sqlprocedure_h

#include <spl/types.h>
#include <spl/debug.h>
#include <spl/string.h>
#include <spl/collection/vector.h>

#include "errors.h"
#include "sqlparam.h"

typedef struct _sqlproc
{
	string* m_name;
	bool m_isCursor;
	string* m_text;
	string* m_body;
	vector* m_prms;
} sqlproc;

sqlproc* sqlprocNew(string* sql);
void sqlprocDelete(sqlproc* sp);

sqlparam* sqlprocFindParam(sqlproc* sp, const char* cp);
sqlparam* sqlprocFindParam2(sqlproc* sp, const char* cp1, const char* cp2);

#ifdef DEBUG
void sqlprocValidateMem(sqlproc* stmt);
#else
#define sqlprocValidateMem(r)
#endif

#endif
