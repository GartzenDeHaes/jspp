#ifndef _sqlitecommand_h
#define _sqlitecommand_h

#include <spl/types.h>
#include <spl/debug.h>
#include <spl/string.h>

#include "sqlite3.h"

#include <spl/data/sqlitereader.h>

typedef struct _sqlitecommand
{
	void *m_conn;
	string* m_sql;
	sqlite3_stmt *m_stmt;
} sqlitecommand;

void sqlitecommandDelete(sqlitecommand* cmd);

int sqlitecommandExecuteNonQuery(sqlitecommand* cmd);
sqlitereader* sqlitecommandExecuteQuery(sqlitecommand* cmd);

bool sqlitecommandResetParameters(sqlitecommand* cmd);

bool sqlitecommandSetParameterInt(sqlitecommand* cmd, const char* paramName, int val);
bool sqlitecommandSetParameterInt64(sqlitecommand* cmd, const char* paramName, int64 val);
bool sqlitecommandSetParameterFloat(sqlitecommand* cmd, const char* paramName, double val);
bool sqlitecommandSetParameterText(sqlitecommand* cmd, const char* paramName, const char* val);
bool sqlitecommandSetParameterNull(sqlitecommand* cmd, const char* paramName);

bool sqlitecommandSetParameterIntAt(sqlitecommand* cmd, int pnum, int val);
bool sqlitecommandSetParameterInt64At(sqlitecommand* cmd, int pnum, int64 val);
bool sqlitecommandSetParameterFloatAt(sqlitecommand* cmd, int pnum, double val);
bool sqlitecommandSetParameterTextAt(sqlitecommand* cmd, int pnum, const char* val);
bool sqlitecommandSetParameterNullAt(sqlitecommand* cmd, int pnum);

#endif
