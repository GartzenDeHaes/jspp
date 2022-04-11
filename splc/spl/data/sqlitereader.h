#ifndef _sqlitereader_h
#define _sqlitereader_h

#include <spl/types.h>
#include <spl/debug.h>
#include <spl/collection/hashtable.h>
#include <spl/string.h>

typedef struct _sqlitereader
{
	void* m_cmd;
	hashtable* m_colIdx;
} sqlitereader;

sqlitereader* sqlitereaderNew(void* cmd);
void sqlitereaderDelete(sqlitereader* reader);

bool sqlitereaderRead(sqlitereader* reader);

int sqlitereaderColumnCount(sqlitereader* reader);

/** Note: this is the column type for the current row */
int sqlitereaderColumnType(sqlitereader* reader, int colNum);
const char* sqlitereaderColumnName(sqlitereader* reader, int colNum);

bool sqlitereaderIsNullAt(sqlitereader* reader, int colNum);
int64 sqlitereaderIntAt(sqlitereader* reader, int colNum);
double sqlitereaderFloatAt(sqlitereader* reader, int colNum);
const char* sqlitereaderTextAt(sqlitereader* reader, int colNum);

bool sqlitereaderIsNull(sqlitereader* reader, const char* colName);
int64 sqlitereaderInt(sqlitereader* reader, const char* colName);
double sqlitereaderFloat(sqlitereader* reader, const char* colName);
const char* sqlitereaderText(sqlitereader* reader, const char* colName);

#endif
