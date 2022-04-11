#ifndef _sqliteconn_h
#define _sqliteconn_h

#include <spl/types.h>
#include <spl/debug.h>
#include <spl/string.h>

#include <spl/data/sqlitecommand.h>

typedef struct _sqliteconnection
{
	string* m_filename;
	sqlite3 *m_db;
	int m_lasterror;
} sqliteconnection;

sqliteconnection* sqliteconnNew(const char* filename);
void sqliteconnDelete(sqliteconnection *conn);

bool sqliteconnOpen(sqliteconnection *conn);
void sqliteconnClose(sqliteconnection *conn);

sqlitecommand* sqliteconnCreateCommand(sqliteconnection *conn, const char* sql);

#endif
