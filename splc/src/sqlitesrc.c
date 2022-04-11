#include <spl/data/sqliteconnection.h>
#include <spl/data/sqlitecommand.h>
#include <spl/data/sqlitereader.h>

static sqlitecommand* sqlitecommandNew(sqliteconnection* conn, const char* sql);
static bool sqlitecommandPrepare(sqlitecommand* cmd);

sqliteconnection* sqliteconnNew(const char* filename)
{
	sqliteconnection* conn = (sqliteconnection*)malloc(sizeof(sqliteconnection));
	
	if (conn == NULL)
	{
		return NULL;
	}
	
	conn->m_db = NULL;
	
	if (NULL == (conn->m_filename = stringNew(filename)))
	{
		free(conn);
		return NULL;
	}
	
	return conn;
}

void sqliteconnDelete(sqliteconnection *conn)
{
	if (conn->m_db != NULL)
	{
		sqliteconnClose(conn);
	}
	stringDelete(conn->m_filename);
	free(conn);
}

bool sqliteconnOpen(sqliteconnection *conn)
{
	if (conn->m_db != NULL)
	{
		return false;
	}
	return SQLITE_OK == sqlite3_open(conn->m_filename->m_cstr, &conn->m_db);
}

void sqliteconnClose(sqliteconnection *conn)
{
	if (conn->m_db == NULL)
	{
		return;
	}
	sqlite3_close(conn->m_db);
	conn->m_db = NULL;
}

sqlitecommand* sqliteconnCreateCommand(sqliteconnection *conn, const char* sql)
{
	sqlitecommand* cmd = sqlitecommandNew(conn, sql);
	if (cmd == NULL)
	{
		return NULL;
	}
		
	return cmd;
}

static sqlitecommand* sqlitecommandNew(sqliteconnection* conn, const char* sql)
{
	sqlitecommand* cmd = (sqlitecommand*)malloc(sizeof(sqlitecommand));
	if (cmd == NULL)
	{
		return NULL;
	}
	
	if (NULL == (cmd->m_sql = stringNew(sql)))
	{
		free(cmd);
		return NULL;
	}
	
	cmd->m_stmt = NULL;
	cmd->m_conn = conn;
	
	if (! sqlitecommandPrepare(cmd))
	{
		sqlitecommandDelete(cmd);
		return NULL;
	}
	
	return cmd;
}

void sqlitecommandDelete(sqlitecommand* cmd)
{
	stringDelete(cmd->m_sql);
	
	if (cmd->m_stmt != NULL)
	{
		sqlite3_finalize(cmd->m_stmt);
	}
	free(cmd);
}

bool sqlitecommandPrepare(sqlitecommand* cmd)
{
	((sqliteconnection*)cmd->m_conn)->m_lasterror = 
		sqlite3_prepare_v2
		(
			((sqliteconnection*)cmd->m_conn)->m_db, 
			cmd->m_sql->m_cstr, 
			cmd->m_sql->m_len, 
			&cmd->m_stmt, 
			NULL
		);
	
	return SQLITE_OK == ((sqliteconnection*)cmd->m_conn)->m_lasterror;
}

int sqlitecommandExecuteNonQuery(sqlitecommand* cmd)
{
	((sqliteconnection *)cmd->m_conn)->m_lasterror = sqlite3_step(cmd->m_stmt);
	
	if (SQLITE_DONE != ((sqliteconnection *)cmd->m_conn)->m_lasterror)
	{
		return -1;
	}
	
	return 1;
}

sqlitereader* sqlitecommandExecuteQuery(sqlitecommand* cmd)
{
	return sqlitereaderNew(cmd);
}

bool sqlitecommandResetParameters(sqlitecommand* cmd)
{
	int rc = sqlite3_clear_bindings(cmd->m_stmt);
	((sqliteconnection *)cmd->m_conn)->m_lasterror = rc;

	if (SQLITE_OK == rc)
	{
		rc = sqlite3_reset(cmd->m_stmt);
		((sqliteconnection *)cmd->m_conn)->m_lasterror = rc;
	}
	
	return SQLITE_OK == rc;
}

bool sqlitecommandSetParameterInt(sqlitecommand* cmd, const char* paramName, int val)
{
	int rc = sqlite3_bind_int(cmd->m_stmt, sqlite3_bind_parameter_index(cmd->m_stmt, paramName), val);
	((sqliteconnection *)cmd->m_conn)->m_lasterror = rc;
	return SQLITE_OK == rc;
}

bool sqlitecommandSetParameterInt64(sqlitecommand* cmd, const char* paramName, int64 val)
{
	int rc = sqlite3_bind_int64(cmd->m_stmt, sqlite3_bind_parameter_index(cmd->m_stmt, paramName), val);
	((sqliteconnection *)cmd->m_conn)->m_lasterror = rc;
	return SQLITE_OK == rc;
}

bool sqlitecommandSetParameterFloat(sqlitecommand* cmd, const char* paramName, double val)
{
	int rc = sqlite3_bind_double(cmd->m_stmt, sqlite3_bind_parameter_index(cmd->m_stmt, paramName), val);
	((sqliteconnection *)cmd->m_conn)->m_lasterror = rc;
	return SQLITE_OK == rc;
}

bool sqlitecommandSetParameterText(sqlitecommand* cmd, const char* paramName, const char* val)
{
	int rc = sqlite3_bind_text(cmd->m_stmt, sqlite3_bind_parameter_index(cmd->m_stmt, paramName), val, -1, SQLITE_STATIC);
	((sqliteconnection *)cmd->m_conn)->m_lasterror = rc;
	return SQLITE_OK == rc;
}

bool sqlitecommandSetParameterNull(sqlitecommand* cmd, const char* paramName)
{
	int rc = sqlite3_bind_null(cmd->m_stmt, sqlite3_bind_parameter_index(cmd->m_stmt, paramName));
	((sqliteconnection *)cmd->m_conn)->m_lasterror = rc;
	return SQLITE_OK == rc;
}

bool sqlitecommandSetParameterIntAt(sqlitecommand* cmd, int pnum, int val)
{
	int rc = sqlite3_bind_int(cmd->m_stmt, pnum, val);
	((sqliteconnection *)cmd->m_conn)->m_lasterror = rc;
	return SQLITE_OK == rc;
}

bool sqlitecommandSetParameterInt64At(sqlitecommand* cmd, int pnum, int64 val)
{
	int rc = sqlite3_bind_int64(cmd->m_stmt, pnum, val);
	((sqliteconnection *)cmd->m_conn)->m_lasterror = rc;
	return SQLITE_OK == rc;
}

bool sqlitecommandSetParameterFloatAt(sqlitecommand* cmd, int pnum, double val)
{
	int rc = sqlite3_bind_double(cmd->m_stmt, pnum, val);
	((sqliteconnection *)cmd->m_conn)->m_lasterror = rc;
	return SQLITE_OK == rc;
}

bool sqlitecommandSetParameterTextAt(sqlitecommand* cmd, int pnum, const char* val)
{
	int rc = sqlite3_bind_text(cmd->m_stmt, pnum, val, -1, SQLITE_STATIC);
	((sqliteconnection *)cmd->m_conn)->m_lasterror = rc;
	return SQLITE_OK == rc;
}

bool sqlitecommandSetParameterNullAt(sqlitecommand* cmd, int pnum)
{
	int rc = sqlite3_bind_null(cmd->m_stmt, pnum);
	((sqliteconnection *)cmd->m_conn)->m_lasterror = rc;
	return SQLITE_OK == rc;
}

sqlitereader* sqlitereaderNew(void* cmd)
{
	sqlitereader* reader = (sqlitereader*)malloc(sizeof(sqlitereader));
	
	if (reader == NULL)
	{
		return NULL;
	}
	
	reader->m_colIdx = NULL;
	reader->m_cmd = cmd;
	
	return reader;
}

void sqlitereaderDelete(sqlitereader* reader)
{
	if (reader->m_colIdx != NULL)
	{
		hashtableDelete(reader->m_colIdx, true);
	}
	free(reader);
}

bool sqlitereaderRead(sqlitereader* reader)
{
	int rc = sqlite3_step(((sqlitecommand *)reader->m_cmd)->m_stmt);
	((sqliteconnection*)((sqlitecommand *)reader->m_cmd)->m_conn)->m_lasterror = rc;
	
	if (SQLITE_ROW == rc && reader->m_colIdx == NULL)
	{
		if (NULL == (reader->m_colIdx = hashtableNew()))
		{
			rc = -1;
		}
		else
		{
			int cnt = sqlitereaderColumnCount(reader);
			int x;
			
			for (x = 0; x < cnt; x++)
			{
				int* ip = (int *)malloc(sizeof(int));
				*ip = x;
				hashtablePut(reader->m_colIdx, sqlitereaderColumnName(reader, x), ip, false);
				
			}
		}
	}
	
	return SQLITE_ROW == rc;
}

int sqlitereaderColumnCount(sqlitereader* reader)
{
	return sqlite3_column_count(((sqlitecommand *)reader->m_cmd)->m_stmt);
}

int sqlitereaderColumnType(sqlitereader* reader, int colNum)
{
	return sqlite3_column_type(((sqlitecommand *)reader->m_cmd)->m_stmt, colNum);
}

const char* sqlitereaderColumnName(sqlitereader* reader, int colNum)
{
	return sqlite3_column_name(((sqlitecommand *)reader->m_cmd)->m_stmt, colNum);
}

bool sqlitereaderIsNullAt(sqlitereader* reader, int colNum)
{
	return sqlitereaderColumnType(reader, colNum) == SQLITE_NULL;
}

int64 sqlitereaderIntAt(sqlitereader* reader, int colNum)
{
	return sqlite3_column_int64(((sqlitecommand *)reader->m_cmd)->m_stmt, colNum);
}

double sqlitereaderFloatAt(sqlitereader* reader, int colNum)
{
	return sqlite3_column_double(((sqlitecommand *)reader->m_cmd)->m_stmt, colNum);
}

const char* sqlitereaderTextAt(sqlitereader* reader, int colNum)
{
	return (const char*)sqlite3_column_text(((sqlitecommand *)reader->m_cmd)->m_stmt, colNum);
}

bool sqlitereaderIsNull(sqlitereader* reader, const char* colName)
{
	if (reader->m_colIdx == NULL || !hashtableContainsKey(reader->m_colIdx, colName))
	{
		return false;
	}
	return sqlitereaderIsNullAt(reader, *(int *)hashtableGet(reader->m_colIdx, colName));
}

int64 sqlitereaderInt(sqlitereader* reader, const char* colName)
{
	if (reader->m_colIdx == NULL || !hashtableContainsKey(reader->m_colIdx, colName))
	{
		return -1;
	}
	return sqlitereaderIntAt(reader, *(int *)hashtableGet(reader->m_colIdx, colName));
}

double sqlitereaderFloat(sqlitereader* reader, const char* colName)
{
	if (reader->m_colIdx == NULL || !hashtableContainsKey(reader->m_colIdx, colName))
	{
		return -1;
	}
	return sqlitereaderFloatAt(reader, *(int *)hashtableGet(reader->m_colIdx, colName));
}

const char* sqlitereaderText(sqlitereader* reader, const char* colName)
{
	if (reader->m_colIdx == NULL || !hashtableContainsKey(reader->m_colIdx, colName))
	{
		return NULL;
	}
	return sqlitereaderTextAt(reader, *(int *)hashtableGet(reader->m_colIdx, colName));
}
