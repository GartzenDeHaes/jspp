#ifndef _preparedstatement_h
#define _preparedstatement_h

#include <spl/types.h>
#include <spl/debug.h>
#include <spl/collection/hashtable.h>
#include <spl/collection/vector.h>
#include <spl/string.h>
#include <spl/stringbuffer.h>

#ifdef __TANDEM
#include <sql.h>
EXEC SQL INCLUDE STRUCTURES ALL VERSION 315;
EXEC SQL INCLUDE sqlda (dummy_da, 1, dummy_names, 1);
#else

struct SQLVAR_TYPE
{
	short data_type;
	short data_len;
	long var_ptr;
	void* ind_ptr;
};

struct SQLDA_TYPE
{
	short num_entries;
	char eye_catcher[2];
	struct SQLVAR_TYPE sqlvar[1];
};

struct SQLSA_TYPE
{
	char eye_catcher[2];
	short version;
	union
	{
		struct DML_TYPE
		{
			short num_tables;
			struct STATS_TYPE
			{
				char table_name[24];
				long records_accessed;
				long records_used;
				long disc_reads;
				long messages;
				long message_bytes;
				short waits;
				short escalations;
				char sqlsa_reserved[4];
			} stats[16];
		} dml;
		struct PREPARE_TYPE
		{
			short input_num;
			short input_names_len;
			short output_num;
			short output_names_len;
			short name_map_len;
			short sql_statement_type;
			long output_collations_len;
		} prepare;
	} u;
} sqlsa;

#endif

#include "errors.h"
#include "request.h"
#include "sqlprocedure.h"

#define PSTMT_HOSTVAR_LEN 11

typedef char (*arrayptr)[4000];

typedef struct _preparedstatement
{
	int m_id;
	string* m_system;
	string* m_name;
	string* m_fqname;
	struct SQLDA_TYPE* m_sqldaIn;
	struct SQLDA_TYPE* m_sqldaOut;
	sqlproc* m_procDef;
	char m_stmtHostvar[PSTMT_HOSTVAR_LEN];
	arrayptr m_namesIn;
	arrayptr m_namesOut;
	int m_numInputs;
	int m_numOutputs;
} preparedstatement;

preparedstatement* preparedstatementNew(string* system, string* sql, int count, int* errorcode);
preparedstatement* preparedstatementNewImmediate(string* system, string* sql, int* errorcode);
void preparedstatementDelete(preparedstatement* stmt);

void preparedstatementExec(preparedstatement* stmt, request* req, stringbuffer* xml);

#ifdef DEBUG
void preparedstatementValidateMem(preparedstatement* stmt);
#else
#define preparedstatementValidateMem(r)
#endif

bool createProcedureTable();
bool loadAllProcedures(hashtable* stmts);
bool updateProcedure(hashtable* stmts, string* system, string* name);
bool saveProcedure(string* system, string* name, string* sql);
bool deleteProcedure(string* system, string* name);
vector* loadAllSystems();

#endif
