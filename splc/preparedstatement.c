#include <spl/io/file.h>

#include "conf.h"
#include "preparedstatement.h"
#include "stats.h"

#include <stdio.h>
#include <stdlib.h>

extern bool _writeError(stringbuffer* xml, const char* cp1, const char* cp2);
extern bool _writeErrorCode(stringbuffer* xml, const char* cp1, const int i);

#ifndef __TANDEM
static char SQLDA_EYE_CATCHER[] = {'A', 'B'};
#endif

const char* g_dbName = "SQLSPROC.db";

#define SIZEOF_SQLDA(d) (sizeof(struct SQLDA_TYPE) + ((d - 1) * sizeof(struct SQLVAR_TYPE)))

static bool _preparedstatementSetInput(preparedstatement* stmt, request* req, stringbuffer* xml);
static void _preparedstatementWriteRow(struct SQLDA_TYPE* sqlda, char*nb, stringbuffer* xml, bool writeMeta);

#ifdef __TANDEM	
#include <cextdecs.h(SQLCADISPLAY, SQLSADISPLAY)>

EXEC SQL BEGIN DECLARE SECTION;
#endif

short sqlcode = 0;
#ifdef __TANDEM	
EXEC SQL INCLUDE SQLSA;
EXEC SQL INCLUDE SQLCA;
#else
struct SQLSA_TYPE sqlsa;
short sqlca;
#define SQLCADISPLAY(d)
#define SQLSADISPLAY(s)
#endif

char stmtHostvar[PSTMT_HOSTVAR_LEN];
char sqlHostvar[8096];
struct SQLDA_TYPE* input_sqlda;
struct SQLDA_TYPE* output_sqlda;
arrayptr cname_i;
arrayptr cname_o;
char c[PSTMT_HOSTVAR_LEN];
struct SQLDA_TYPE* sqldaIn;
struct SQLDA_TYPE* sqldaOut;
	
#ifdef __TANDEM	
EXEC SQL END DECLARE SECTION;
#endif

/* ********************************************************** */
/* FUNCTION get_dtname */
/* This function places the name of a given data type into */
/* the array datatype_name */
/* */
/* Return: pointer to array datatype_name */
/* (array is null terminated) */
/* ********************************************************** */
const char* get_dtname (short datatype)
{
	switch (datatype) 
	{
	case _SQLDT_ASCII_F : /* CHAR data type */
		return "CHAR";

	case _SQLDT_ASCII_F_UP:
		return "CHAR UPSHIFT";

	case _SQLDT_ASCII_V : /* VARCHAR data type */
		return "VARCHAR";

	case _SQLDT_ASCII_V_UP:
		return "VARCHAR UPSHIFT";

	case _SQLDT_16BIT_S : /* 16 bit signed binary */
		return "SIGNED 16BIT NUMERIC";
		
	case _SQLDT_16BIT_U : /* 16 bit unsigned binary */
		return "UNSIGNED 16BIT NUMERIC";

	case _SQLDT_32BIT_S : /* 32 bit signed binary */
		return "SIGNED 32BIT NUMERIC";

	case _SQLDT_32BIT_U : /* 32 bit unsigned binary */
		return "UNSIGNED 32BIT NUMERIC";

	case _SQLDT_64BIT_S : /* 64 bit signed binary */
		return "SIGNED 64BIT NUMERIC";

	case _SQLDT_DEC_U : /* DECIMAL datatype: unsi */
		return "UNSIGNED DECIMAL";

	case _SQLDT_DEC_LSS : /* DECIMAL datatype: LSS */
		return "LEADING SIGN SEPARATE DECIMAL";

	case _SQLDT_DEC_LSE : /* DECIMAL datatype: LSE */
		return "LEADING SIGN EMBEDDED DECIMAL";

	case _SQLDT_DEC_TSS : /* DECIMAL datatype: TSS */
		return "TRAILING SIGN SEPARATE DECIMAL";

	case _SQLDT_DEC_TSE : /* DECIMAL datatype: TSE */
		return "TRAILING SIGN EMBEDDED DECIMAL";
	
	case _SQLDT_REAL:
		return "REAL";
		
	case _SQLDT_DOUBLE_V:
		return "DOUBLE";
		
	case _SQLDT_DATETIME:
		return "DATETIME";
	
	case _SQLDT_INT_Y_Y:
		return "INTERVAL YEAR TO YEAR";
		
	case _SQLDT_INT_MO_MO:
		return "MONTH TO MONTH";
	case _SQLDT_INT_Y_MO:
		return "YEAR TO MONTH";
	case _SQLDT_INT_D_D:
		return "DAY TO DAY";
	case _SQLDT_INT_H_H:
		return "HOUR TO HOUR";
	case _SQLDT_INT_D_H:
		return "DAY TO HOUR";
	case _SQLDT_INT_MI_MI:
		return "MINUTE TO MINUTE";
	case _SQLDT_INT_H_MI:
		return "HOUR TO MINUTE";
	case _SQLDT_INT_D_MI:
		return "DAY TO MINUTE";
	case _SQLDT_INT_S_S:
		return "SECOND TO SECOND";
	case _SQLDT_INT_MI_S:
		return "MINUTE TO SECOND";
	case _SQLDT_INT_H_S:
		return "HOUR TO SECOND";
	case _SQLDT_INT_D_S:
		return "DAY TO SECOND";
	case _SQLDT_INT_F_F:
		return "FRACTION TO FRACTION";
	case _SQLDT_INT_S_F:
		return "SECOND TO FRACTION";
	case _SQLDT_INT_MI_F:
		return "MINUTE TO FRACTION";
	case _SQLDT_INT_H_F:
		return "HOUR TO FRACTION";
	case _SQLDT_INT_D_F:
		return "DAY TO FRACTION";
	default:
		printf("not found dt %d\n", (int)datatype);
		return "not found";
	}
}

/* *************************************************** */
/* FUNCTION setupvarbuffers */
/* This function takes an SQLDA as a parameter and, */
/* for sqlda.num_entries, allocates the data buffers */
/* for appropriate lengths. For each sqlvar, */
/* sqlda.sqlvar[i].var_ptr is set to point to that */
/* buffer */
/* */
/* The sqlda is also changed by altering unsupported */
/* data types to the nearest equivalent data types */
/* and by setting scale information to 0 */
/* */
/* sqlda.num_entries is assumed to have a valid value. */
/* */
/* Return: 0 if successful */
/* -1 if failure */
/* */
static bool _setupvarbuffers (struct SQLDA_TYPE* sqlda)
{
	int num_entries;
	int mem_reqd; /* buffer size */
	int i;

	/* --------------------------------------------------- */
	/* Handle unsupported types; set scale information to 0. */
	/* --------------------------------------------------- */
	/*_adjust_sqlda_scale_types(sqlda);*/
	
	num_entries = sqlda->num_entries;
	
	for (i = 0; i < num_entries; i++)
	{
		sqlda->sqlvar[i].var_ptr = 0;
	}
		
	for (i = 0; i < num_entries; i++)
	{
		mem_reqd = 0;
		
		switch (sqlda->sqlvar[i].data_type) 
		{
		case _SQLDT_ASCII_F_UP:     /* CHAR datatype, UPSHIFTed         */
		case _SQLDT_ASCII_F :		/* CHAR datatype */
			mem_reqd = sqlda->sqlvar[i].data_len;
			break;

		case _SQLDT_ASCII_V_UP:	/* VARCHAR datatype, UPSHIFted      */
		case _SQLDT_ASCII_V :	/* VARCHAR datatype */
			mem_reqd = sqlda->sqlvar[i].data_len + 2;
			break;

		case _SQLDT_16BIT_S : /* SMALLINT */
		case _SQLDT_16BIT_U : /* UNSIGNED SMALLINT */
		case _SQLDT_32BIT_S : /* INTEGER */
		case _SQLDT_32BIT_U : /* UNSIGNED INTEGER */
			/*-------------------------------------------- */
			/* NOTE ON SCALE INFORMATION */
			/*-------------------------------------------- */
			/* Bits 0 through 7 of sqlda->sqlvar[i].data_len */
			/* have the scale information for the numeric */
			/* data types. Either remember this scale */
			/* information and later use the values in the */
			/* host variables appropriately or set the */
			/* scale information to 0 (which can lead to */
			/* truncated values on retrievals and inability */
			/* to provide scaled values through input */
			/* parameters) */
			/*-------------------------------------------- */
			/* Set scale information to 0 (see note above) */
			/*-------------------------------------------- */
			
			/* this code is wrong, maybe it works for decimal? */
			/*
			printf("sqlda->sqlvar[i].data_len==%d\n", (int)sqlda->sqlvar[i].data_len);
			sqlda->sqlvar[i].data_len = (short)(sqlda->sqlvar[i].data_len & 0xFF);
			*/
			/*--------------------------------------------*/
			/* Extract length from bits 8:15 */
			/*--------------------------------------------*/
			mem_reqd = sqlda->sqlvar[i].data_len & 0xFF;
			break;

		case _SQLDT_DEC_LSS:    /* DECIMAL datatype:                */
                                /* leading sign is separate         */
		case _SQLDT_DEC_LSE:    /* DECIMAL datatype:                */
                                /* leading sign is embedded         */
		case _SQLDT_DEC_TSS:    /* DECIMAL datatype:                */
                                /* trailing sign separate           */
		case _SQLDT_DEC_TSE:    /* DECIMAL datatype:                */
                                /* trailing sign embedded           */
		case _SQLDT_DEC_U:      /* DECIMAL datatype: unsigned       */
			mem_reqd = sqlda->sqlvar[i].data_len & 0xFF;
			break;
			
		case _SQLDT_64BIT_S:       /* 64 bit signed binary number      */
                                /* LARGEINT, NUMERIC(10-18,<scale>) */
			mem_reqd = sqlda->sqlvar[i].data_len & 0xFF;
			break;
			
		case _SQLDT_DATETIME:      /* DATETIME datatype                */
			mem_reqd = sqlda->sqlvar[i].data_len & 0xFF;
			break;
			
		case _SQLDT_DOUBLE_F:        /* DOUBLE CHAR datatype             */
		case _SQLDT_DOUBLE_V:     /* DOUBLE VARCHAR datatype          */
		case _SQLDT_REAL:          /* 32 bit FLOAT                     */
		case _SQLDT_DOUBLE:        /* 64 bit FLOAT                     */
			printf("REAL/DOUBLE len==%d len>>8==%d len&0xFF=%d\n", (int)sqlda->sqlvar[i].data_len, (int)sqlda->sqlvar[i].data_len >> 8, (int)sqlda->sqlvar[i].data_len & 0xFF);

		case _SQLDT_INT_Y_Y:         /* YEAR TO YEAR         */
		case _SQLDT_INT_MO_MO:       /* MONTH TO MONTH       */
		case _SQLDT_INT_Y_MO:        /* YEAR TO MONTH        */
		case _SQLDT_INT_D_D:         /* DAY TO DAY           */
		case _SQLDT_INT_H_H:         /* HOUR TO HOUR         */
		case _SQLDT_INT_D_H:         /* DAY TO HOUR          */
		case _SQLDT_INT_MI_MI:       /* MINUTE TO MINUTE     */
		case _SQLDT_INT_H_MI:        /* HOUR TO MINUTE       */
		case _SQLDT_INT_D_MI:        /* DAY TO MINUTE        */
		case _SQLDT_INT_S_S:         /* SECOND TO SECOND     */
		case _SQLDT_INT_MI_S:        /* MINUTE TO SECOND     */
		case _SQLDT_INT_H_S:         /* HOUR TO SECOND       */
		case _SQLDT_INT_D_S:         /* DAY TO SECOND        */
		case _SQLDT_INT_F_F:         /* FRACTION TO FRACTION */
		case _SQLDT_INT_S_F:         /* SECOND TO FRACTION   */
		case _SQLDT_INT_MI_F:        /* MINUTE TO FRACTION   */
		case _SQLDT_INT_H_F:         /* HOUR TO FRACTION     */
		case _SQLDT_INT_D_F:		/* DAY TO FRACTION      */
			mem_reqd = sqlda->sqlvar[i].data_len;
			break;		
		default:
			printf("\n**** Error: Unsupported Datatype: %d %s prec == %d scale == %d\n", (int)sqlda->sqlvar[i].data_type, get_dtname(sqlda->sqlvar[i].data_type), (int)sqlda->sqlvar[i].data_len & 0xFF, (int)sqlda->sqlvar[i].data_len >> 8);
			return false;
		}

		/* ---------------------------------------------- */
		/* Allocate memory for the data buffer and assign */
		/* byte address of the data buffer to var_ptr of */
		/* sqlvar[i]: */
		/* ---------------------------------------------- */
		if (mem_reqd == 0)
		{
			sqlda->sqlvar[i].var_ptr = 0;
		}
		else
		{
			sqlda->sqlvar[i].var_ptr = (long)malloc(mem_reqd);
		}

	} /* for loop */

	return true;
}

static struct SQLDA_TYPE* allocate_sqlda(short num_entries)
{
	struct SQLDA_TYPE* sqlda;
	int i;

	if ((sqlda = (struct SQLDA_TYPE*)malloc(SIZEOF_SQLDA(num_entries))) == NULL)
	{
		return NULL;
	}

	/* Initialize sqlda; constant sqlda_eye_catcher is defined */
	/* by the C compiler and is always 2 characters: */
	strncpy(sqlda->eye_catcher, SQLDA_EYE_CATCHER, 2);

	sqlda->num_entries = num_entries;

	/* Initialize ind_ptr to NULL. ind_ptr must always be */
	/* initialized, even when the program does not handle null */
	/* values */
	for (i = 0; i < num_entries; i++)
	{
		ASSERT_PTR(&sqlda->sqlvar[i]);
		memset(&sqlda->sqlvar[i], 0, sizeof(struct SQLVAR_TYPE));
	}
	
	return sqlda;
}

static void free_sqlda(struct SQLDA_TYPE* sqlda)
{
	int i;
	char *buf_ptr;

	if (sqlda == NULL)
	{
		return;
	}
	
	ASSERT_PTR(sqlda);
	
	for (i = 0; i < sqlda->num_entries; i++)
	{
		if ((buf_ptr = (char *)sqlda->sqlvar[i].var_ptr) != NULL)
		{
			free(buf_ptr);
		}
	}
	
	free (sqlda);
}

static void _padbuf(char *buf, int len, char ch)
{
	int pos = 0;
	
	while (buf[pos] != '\0')
	{
		pos++;
	}
	
	while (pos < len)
	{
		buf[pos++] = ch;
	}
}

static bool _preparedstatementSqlInner
(
	preparedstatement* stmt, 
	bool prepare, 
	int* errorcode,
	request* req,
	stringbuffer* xml
)
{
	if (prepare)
	{
		int outputnameslen;
		
		if (stmt->m_procDef->m_body->m_len >= 8096)
		{
			*errorcode = SQLE_TOOLONG;
			return false;
		}
		
		StrCpyLen(sqlHostvar, stmt->m_procDef->m_body->m_cstr, 8096);
		StrReplaceChLen(sqlHostvar, '\n', ' ', 8096);
		StrReplaceChLen(sqlHostvar, '\r', ' ', 8096);
		StrReplaceChLen(sqlHostvar, '\t', ' ', 8096);
		_padbuf(sqlHostvar, sizeof(sqlHostvar), ' ');
		
#ifdef __TANDEM
		EXEC SQL PREPARE :stmtHostvar FROM :sqlHostvar;
#else
		sqlsa.u.prepare.input_num = vectorCount(stmt->m_procDef->m_prms);
		sqlsa.u.prepare.output_num = 1;
		sqlsa.u.prepare.input_names_len = 10;
		sqlsa.u.prepare.output_names_len = 10;
#endif

		SQLCADISPLAY ((short *)&sqlca);
		printf("**********\n");
		SQLSADISPLAY((short *)&sqlsa, (short *)&sqlca, -1, NULL);
		
		if (sqlcode != 0)
		{
			*errorcode = sqlcode;
			return false;
		}
		
		strcpy(stmt->m_stmtHostvar, stmtHostvar);
		
		outputnameslen = sqlsa.u.prepare.output_names_len;
		
		stmt->m_numInputs = sqlsa.u.prepare.input_num;
		if (NULL == (stmt->m_sqldaIn = allocate_sqlda(sqlsa.u.prepare.input_num)))
		{
			*errorcode = SQLE_OUTOFMEM;
			return false;
		}
		
		stmt->m_numOutputs = sqlsa.u.prepare.output_num;
		if (NULL == (stmt->m_sqldaOut = allocate_sqlda(sqlsa.u.prepare.output_num)))
		{
			*errorcode = SQLE_OUTOFMEM;
			return false;
		}
		
		input_sqlda = stmt->m_sqldaIn;
		if (sqlsa.u.prepare.input_names_len > 0)
		{
			if (NULL == (stmt->m_namesIn = (arrayptr)malloc(sqlsa.u.prepare.input_names_len)))
			{
				*errorcode = SQLE_OUTOFMEM;
				return false;
			}
			cname_i = stmt->m_namesIn;
			
#ifdef __TANDEM
			EXEC SQL DESCRIBE INPUT :stmtHostvar INTO :*input_sqlda NAMES INTO :*cname_i;
#else
			memset(stmt->m_namesIn, 0, sizeof(sqlsa.u.prepare.input_names_len));
#endif
		}
		
		if (sqlcode != 0)
		{
			*errorcode = sqlcode;
			return false;
		}
		
		output_sqlda = stmt->m_sqldaOut;
		if (outputnameslen > 0)
		{
			if (NULL == (stmt->m_namesOut = (arrayptr)malloc(outputnameslen)))
			{
				*errorcode = SQLE_OUTOFMEM;
				return false;
			}
			
			cname_o = stmt->m_namesOut;

#ifdef __TANDEM	
			EXEC SQL DESCRIBE :stmtHostvar INTO :*output_sqlda NAMES INTO :*cname_o;
#endif
		}
		
		if (sqlcode != 0)
		{
			stmt->m_sqldaOut->num_entries = 0;
			printf("EXEC SQL DESCRIBE error %d\n", (int)sqlcode);
			*errorcode = sqlcode;
			return false;
		}
		
#ifndef __TANDEM
		output_sqlda->sqlvar[0].data_type = _SQLDT_ASCII_F;
		output_sqlda->sqlvar[0].data_len = 1;
#endif
		
		if (! _setupvarbuffers(input_sqlda))
		{
			printf("_setupvarbuffers input error\n");
			*errorcode = SQLE_VARBUFS;
			return false;
		}
		if (! _setupvarbuffers(output_sqlda))
		{
			printf("_setupvarbuffers output error\n");
			*errorcode = SQLE_VARBUFS;
			return false;
		}
		
		preparedstatementValidateMem(stmt);
		
		printf("PREPARED %s\n", stmt->m_fqname->m_cstr);
		
		return true;
	}

	/* end of prepare */
	
	strcpy(c, stmt->m_stmtHostvar);
	strcpy(stmtHostvar, stmt->m_stmtHostvar);

	if (! _preparedstatementSetInput(stmt, req, xml))
	{
		return false;
	}
	
	sqlcode = 0;
	
	sqldaIn = stmt->m_sqldaIn;
	sqldaOut = stmt->m_sqldaOut;

	if (! stmt->m_procDef->m_isCursor)
	{
		/*****************************************************/
		/* Not a SELECT statement. Perform EXECUTE with */
		/* USING DESCRIPTOR if there are input variables; */
		/* otherwise, perform EXECUTE */
		/*****************************************************/
#ifdef __TANDEM
		if (stmt->m_numInputs > 0)
		{
			EXEC SQL EXECUTE :stmtHostvar USING DESCRIPTOR :*sqldaIn;
		}
		else
		{
			EXEC SQL EXECUTE :stmtHostvar;
		}
#endif

		writeStats(xml, &sqlsa);

		if (sqlcode != 0)
		{
			SQLCADISPLAY ((short *)&sqlca);
			_writeErrorCode(xml, "EXEC SQL EXECUTE failed", sqlcode);
		}		
	}
	else
	{
		clearStatsAcc();

		_preparedstatementWriteRow(stmt->m_sqldaOut, (char *)stmt->m_namesOut, xml, true);
		
		/*************************************************/
		/* SELECT statement */
		/*************************************************/

		/* --------------------------------------------- */
		/* Define a cursor name for the statement S1, to be */
		/* used later in OPEN, FETCH and CLOSE statements */
		/* --------------------------------------------- */
#ifdef __TANDEM	
		EXEC SQL DECLARE :c CURSOR FOR :stmtHostvar;
#endif

		if (sqlcode != 0)
		{
			printf("cname=%s; stmtname=%s\n", c, stmtHostvar);
			SQLCADISPLAY ((short *)&sqlca);
			_writeErrorCode(xml, "EXEC SQL DECLARE failed", sqlcode);
			return false;
		}

		statsAcc(&sqlsa);
		
		/************************************************ */
		/* Open the cursor. By this point, all input */
		/* parameters must have valid values */
		/************************************************ */
#ifdef __TANDEM	
		if (stmt->m_numInputs > 0)
		{
			EXEC SQL OPEN :c USING DESCRIPTOR :*sqldaIn;
		}
		else
		{
			EXEC SQL OPEN :c;
		}
#endif

		statsAcc(&sqlsa);
		
		if (sqlcode != 0)
		{
			_writeErrorCode(xml, "EXEC SQL OPEN failed", sqlcode);
			return false;
		}

		while (sqlcode >= 0) 
		{
#ifdef __TANDEM	
			EXEC SQL FETCH :c USING DESCRIPTOR :*sqldaOut;
#else
			sqlcode = 100;
#endif
			statsAcc(&sqlsa);
			
			if (sqlcode == 100)
			{ 
				break;
			}

			if (sqlcode >= 0)
			{
				_preparedstatementWriteRow(stmt->m_sqldaOut, (char *)stmt->m_namesOut, xml, false);
			}
		}

		if (sqlcode < 0)
		{
			_writeErrorCode(xml, "SQL ERROR", sqlcode);
		}

#ifdef __TANDEM	
		EXEC SQL CLOSE :c;
#endif		
		writeStatsAcc(xml, &sqlsa);
	}
	
	return true;
}

static bool _preparedstatementPrepare(preparedstatement* stmt, int* errorcode)
{
	return _preparedstatementSqlInner(stmt, true, errorcode, NULL, NULL);
}

preparedstatement* preparedstatementNew
(
	string* system, 
	string* sql, 
	int count,
	int* errorcode
)
{
	string* stmp;
	preparedstatement* stmt = (preparedstatement*)malloc(sizeof(preparedstatement));
	
	*errorcode = SQLE_OUTOFMEM;
	
	if (stmt == NULL)
	{
		return NULL;
	}

	memset(stmt, 0, sizeof(preparedstatement));
	
	stmt->m_id = count;
	
	if (NULL == (stmt->m_procDef = sqlprocNew(sql)))
	{
		*errorcode = SQLE_SYNTAX;
		preparedstatementDelete(stmt);
		return NULL;
	}
	
	if (NULL == (stmt->m_system = stringDup(system)))
	{
		preparedstatementDelete(stmt);
		return NULL;
	}
	
	if (NULL == (stmt->m_name = stringDup(stmt->m_procDef->m_name)))
	{
		preparedstatementDelete(stmt);
		return NULL;
	}
		
	if (NULL == (stmp = stringCat(stmt->m_system, ".", 1)))
	{
		preparedstatementDelete(stmt);
		return NULL;
	}
	if (NULL == (stmt->m_fqname = stringCat(stmp, stmt->m_name->m_cstr, stmt->m_name->m_len)))
	{
		preparedstatementDelete(stmt);
		return NULL;
	}
	stringDelete(stmp);
	
	if (! _preparedstatementPrepare(stmt, errorcode))
	{
		preparedstatementDelete(stmt);
		return NULL;
	}

	return stmt;
}

preparedstatement* preparedstatementNewImmediate
(
	string* system, 
	string* sql, 
	int* errorcode
)
{
	stringbuffer* buf;
	preparedstatement* stmt;
	
	*errorcode = SQLE_OUTOFMEM;
	
	if (NULL == (buf = stringbufferNew(120)))
	{
		return NULL;	
	}
	
	stringbufferAppend(buf, "CREATE CURSOR ZZTMP () ");
	stringbufferAppend(buf, sql->m_cstr);
	
	sql = stringbufferToString(buf);
	stringbufferDelete(buf);
	
	stmt = preparedstatementNew(system, sql, -1, errorcode);
	
	stringDelete(sql);
	
	return stmt;
}

void preparedstatementDelete(preparedstatement* stmt)
{
	preparedstatementValidateMem(stmt);
	
	if (stmt->m_system != NULL)
	{
		stringDelete(stmt->m_system);
	}
	
	if (stmt->m_name != NULL)
	{
		stringDelete(stmt->m_name);
	}
	
	if (stmt->m_fqname != NULL)
	{
		stringDelete(stmt->m_fqname);
	}
	
	if (stmt->m_sqldaIn != NULL)
	{
		free_sqlda(stmt->m_sqldaIn);
	}
	
	if (stmt->m_sqldaOut != NULL)
	{
		free_sqlda(stmt->m_sqldaOut);
	}
	
	if (stmt->m_procDef != NULL)
	{
		sqlprocDelete(stmt->m_procDef);
	}
	
	if (stmt->m_namesIn != NULL)
	{
		free(stmt->m_namesIn);
	}
	
	if (stmt->m_namesOut != NULL)
	{
		free(stmt->m_namesOut);
	}
	
	free(stmt);
}

static bool _preparedstatementSetInput(preparedstatement* stmt, request* req, stringbuffer* xml)
{
	short *len_ptr; /* int ptr to get the length from the names buffer */
					/* and write len prefix to varchar data buffers */
					
	short name_len;		/* num bytes in a name */
	int i;

	short data_len;		/* #bytes of input data needed*/
	
	char name_array[31];/* buffer to contain null terminated name of the */
						/* input param (without the leading '?' ) */
	char *lastchar;
	char *nb = (char *)stmt->m_namesIn;
	char *cp;
	
	requestItem* item;
	
	for (i = 0; i < stmt->m_sqldaIn->num_entries; i++)
	{
		len_ptr = (short *) nb; /* get to length prefix */
		name_len = *len_ptr;
		nb += 2; /* advance nb to skip the 2-byte length prefix */
	
		if (name_len > 30)
		{
			_writeError(xml, "internal name buffer too short", "");
			return false;
		}
		
		/* get null terminated param name in name_array */
		if (name_len == 0)
		{
			name_array[0] = '\0'; /* unnamed param */
		}
		else
		{
			lastchar = nb + (name_len - 1);
			if (*lastchar == ' ')
			{ 
				strncpy( name_array, nb, name_len - 1);
				name_array[name_len -1] = '\0';
			}
			else
			{ 
				strncpy(name_array, nb, name_len);
				name_array[name_len] = '\0';
			}
			
			/* advance nb to the next name */
			nb = lastchar + 1;
		}
		
		if (NULL == (item = requestGetItem(req, name_array)))
		{
			if (NULL == (item = requestGetItemAt(req, i)))
			{
				_writeError(xml, "parameter not found ", name_array);
				return false;
			}
			if (! (item->m_name == NULL || item->m_name->m_len == 0))
			{
				_writeError(xml, "parameter not found ", name_array);
				return false;
			}
		}
		
		switch (stmt->m_sqldaIn->sqlvar[i].data_type) 
		{
		case _SQLDT_DATETIME:   /* DATETIME datatype                */			
		case _SQLDT_DEC_LSS:    /* DECIMAL datatype:                */
                                /* leading sign is separate         */
		case _SQLDT_DEC_LSE:    /* DECIMAL datatype:                */
                                /* leading sign is embedded         */
		case _SQLDT_DEC_TSS:    /* DECIMAL datatype:                */
                                /* trailing sign separate           */
		case _SQLDT_DEC_TSE:    /* DECIMAL datatype:                */
                                /* trailing sign embedded           */
		case _SQLDT_DEC_U:      /* DECIMAL datatype: unsigned       */
		case _SQLDT_DOUBLE_F:        /* DOUBLE CHAR datatype             */
		case _SQLDT_ASCII_F_UP:
		case _SQLDT_ASCII_F:
			/* CHAR data type */
			
			data_len = stmt->m_sqldaIn->sqlvar[i].data_len;

			requestItemToCharBuf(item, (char *)stmt->m_sqldaIn->sqlvar[i].var_ptr, data_len);
			
			_padbuf((char *)stmt->m_sqldaIn->sqlvar[i].var_ptr, data_len, ' ');

			break;
		
		case _SQLDT_DOUBLE_V:     /* DOUBLE VARCHAR datatype          */
		case _SQLDT_ASCII_V_UP:
		case _SQLDT_ASCII_V:
			/* VARCHAR data type */
			cp = (char *)(stmt->m_sqldaIn->sqlvar[i].var_ptr + 2);
			data_len = stmt->m_sqldaIn->sqlvar[i].data_len;

			requestItemToCharBuf(item, cp, data_len);

			_padbuf(cp, data_len, ' ');

			len_ptr = (short *)stmt->m_sqldaIn->sqlvar[i].var_ptr;
			*len_ptr = (short)strlen((char *)stmt->m_sqldaIn->sqlvar[i].var_ptr); /* #chars for the varchar buffer */

			break;
			
		case _SQLDT_16BIT_S : /* 16 bit signed numeric */
			*(short *)stmt->m_sqldaIn->sqlvar[i].var_ptr = (short)requestItemToInt(item);
			break;
		case _SQLDT_16BIT_U : /* 16 bit unsigned numeric */
			*(unsigned short *)stmt->m_sqldaIn->sqlvar[i].var_ptr = (unsigned short)requestItemToInt(item);
			break;
		case _SQLDT_32BIT_S : /* 32 bit signed numeric */
			*(int32 *)stmt->m_sqldaIn->sqlvar[i].var_ptr = requestItemToInt(item);
			break;
		case _SQLDT_32BIT_U : /* 16 bit unsigned numeric */
			*(uint32 *)stmt->m_sqldaIn->sqlvar[i].var_ptr = (uint32)requestItemToInt(item);
			break;
		
		case _SQLDT_64BIT_S:
			*(int64 *)stmt->m_sqldaIn->sqlvar[i].var_ptr = requestItemToLong(item);
			break;
			
		case _SQLDT_DOUBLE:        /* 64 bit FLOAT                     */
			*(float64 *)stmt->m_sqldaIn->sqlvar[i].var_ptr = requestItemToDouble(item);
			break;
			
		case _SQLDT_REAL:          /* 32 bit FLOAT                     */
			*(float32 *)stmt->m_sqldaIn->sqlvar[i].var_ptr = (float32)requestItemToDouble(item);
			break;
		
		default:
			_writeErrorCode(xml, "unsupported date type ", stmt->m_sqldaIn->sqlvar[i].data_type);
			return false;		
		}
	}
	
	return true;
}

static bool _psWriteColInt64(stringbuffer* xml, char* cname, int64 data, int scale);

static bool _psWriteColCh(stringbuffer* xml, char* cname, char* data)
{
	return stringbufferAppend(xml, cname) &&
		stringbufferAppend(xml, "='") &&
		stringbufferAppendXmlEncode(xml, data, true) &&
		stringbufferAppend(xml, "' ");
}

static bool _psWriteColInt(stringbuffer* xml, char* cname, int data, int scale)
{
	if (scale != 0)
	{
		return _psWriteColInt64(xml, cname, (int64)data, scale);
	}
	return stringbufferAppend(xml, cname) &&
		stringbufferAppend(xml, "='") &&
		stringbufferAppendInt(xml, data) &&
		stringbufferAppend(xml, "' ");
}

static bool _psWriteColUInt(stringbuffer* xml, char* cname, uint32 data, int scale)
{
	char buf[32];
	
	if (scale != 0)
	{
		return _psWriteColInt64(xml, cname, (int64)data, scale);
	}
	
	sprintf(buf, "%u", data);
	
	return stringbufferAppend(xml, cname) &&
		stringbufferAppend(xml, "='") &&
		stringbufferAppend(xml, buf) &&
		stringbufferAppend(xml, "' ");
}

static bool _psWriteColInt64(stringbuffer* xml, char* cname, int64 data, int scale)
{
	if (! stringbufferAppend(xml, cname))
	{
		return false;
	}
	
	if (! stringbufferAppend(xml, "='"))
	{
		return false;
	}

	if (scale == 0)
	{
		if (! stringbufferAppendLong(xml, data))
		{
			return false;
		}
	}
	else
	{
		char buf[64];
		int x;
		int len;
		bool isneg = data < 0;
		stringbuffer* sbuf = stringbufferNew();
		string* str;
		string* strrev;
		
		sprintf(buf, "%ld", data);
		len = (int)strlen(buf) - 1;
		
		for (x = 0; x < scale; x++)
		{
			if (len < 0 || buf[len] == '-')
			{
				stringbufferAppendCh(sbuf, '0');
			}
			else
			{
				stringbufferAppendCh(sbuf, buf[len--]);
			}
		}
		
		if (scale != 0)
		{
			if (! stringbufferAppend(sbuf, "."))
			{
				return false;
			}
		}
		
		while (len >= 0 && buf[len] != '-')
		{
			if (! stringbufferAppendCh(sbuf, buf[len--]))
			{
				return false;
			}
		}
		
		if (isneg)
		{
			stringbufferAppendCh(sbuf, '-');
		}
		
		str = stringbufferToString(sbuf);
		stringbufferDelete(sbuf);
		
		if (str == NULL)
		{
			return NULL;
		}
		
		strrev = stringReverse(str);
		
		stringDelete(str);
		
		stringbufferAppendString(xml, strrev);
		
		stringDelete(strrev);
	}
	
	return stringbufferAppend(xml, "' ");
}

static bool _psWriteColDec(stringbuffer* xml, char* cname, char* data, int prec, int scale)
{
	int x;
	char ch;
	/*int dotpos = prec - scale;*/
	
	if (! stringbufferAppend(xml, cname))
	{
		return false;
	}

	if (! stringbufferAppend(xml, "='"))
	{
		return false;
	}
	
	for (x = 0; x < prec; x++)
	{
		/*if (x == dotpos)
		{
			if (x == 0 && data[x] == '-')
			{
				dotpos = 1;
			}
			else if (! stringbufferAppendCh(xml, '.'))
			{
				return false;
			}
		}*/
		
		ch = data[x];
		if (ch < ' ' || ch > 127)
		{
			ch = '0';
		}
		if (! stringbufferAppendCh(xml, data[x]))
		{
			return false;
		}
	}
	
	return stringbufferAppend(xml, "' ");
}

static bool _psWriteColDouble(stringbuffer* xml, char* cname, double data)
{
	if (! stringbufferAppend(xml, cname))
	{
		return false;
	}

	if (! stringbufferAppend(xml, "='"))
	{
		return false;
	}

	stringbufferAppendReal(xml, data);

	return stringbufferAppend(xml, "' ");
}

static void _preparedstatementWriteRow
(
	struct SQLDA_TYPE* sqlda, 
	char* nb, 
	stringbuffer* xml,
	bool meta
)
{
	short *len_ptr; /* int ptr to get the length */
					/* from the names buffer */
	short name_len; /* num bytes in a name */
	short num_entries; /* number of sqlvar entries */
	short i;
	char data_array[1024]; /* buffer to contain data to  be displayed (null terminated) */
	char *data_ptr; /* ptr to retrieved data */
	short data_len; /* data buffer size */
	char name_array[244];/* buffer to contain null terminated name in a */
						/* <tabname>.<colname> format [ 8 + 1 + 30 chars ] */
	char *lastchar;
	
	char *errormsg = NULL;

	if (meta)
	{
		stringbufferAppend(xml, "<META>");		
	}
	else
	{
		stringbufferAppend(xml, "<row ");
	}
	
	num_entries = sqlda->num_entries;
	
	for (i = 0; i < num_entries; i++)
	{
		len_ptr = (short *)nb; /* get to length prefix */
		name_len = *len_ptr;
		nb += 2; /* advance nb to skip the 2-byte length prefix */

		/* get null terminated name in name_array */
		if (name_len == 0)
		{
			char ibuf[32];
			strcpy(name_array, "EXPR");
			strcat(name_array, itoa(i, ibuf, 10));
		}
		else
		{
			lastchar = nb + (name_len - 1);
			if (*lastchar == ' ') 
			{ 
				/* last character is blank that SQL inserts to make */
				/* length info fall on an even byte boundary */
				/* (the name had an odd number of characters) */
				strncpy( name_array, nb, name_len - 1);
				name_array[name_len - 1] = '\0';
			}
			else
			{ 
				strncpy( name_array, nb, name_len);
				name_array[name_len] = '\0';
			}
		}

		/* advance nb to the next name */
		nb = lastchar + 1;

		if (meta)
		{
			stringbufferAppend(xml, "<");
			stringbufferAppend(xml, name_array);
			stringbufferAppend(xml, " type='");
			stringbufferAppend(xml, get_dtname(sqlda->sqlvar[i].data_type));
			stringbufferAppend(xml, "' precision='");
		}

		switch (sqlda->sqlvar[i].data_type) 
		{
		case _SQLDT_DOUBLE_F:        /* DOUBLE CHAR datatype             */
		case _SQLDT_ASCII_F:
			/* CHAR data type */
			
			if (meta)
			{
				stringbufferAppendInt(xml, (int)sqlda->sqlvar[i].data_len);
				stringbufferAppend(xml, "' scale='0' />");
				break;
			}
			else
			{
				data_ptr = (char *) sqlda->sqlvar[i].var_ptr;
				data_len = sqlda->sqlvar[i].data_len;

				if (data_len < sizeof(data_array))
				{
					strncpy(data_array, data_ptr, data_len);
					data_array[data_len] = '\0';
					
					_psWriteColCh(xml, name_array, data_array);
				}
				else
				{
					errormsg = "data truncation error";
				}
			}
			break;

		case _SQLDT_DOUBLE_V:     /* DOUBLE VARCHAR datatype          */
		case _SQLDT_ASCII_V:
			/* VARCHAR datatype */

			if (meta)
			{
				stringbufferAppendInt(xml, (int)sqlda->sqlvar[i].data_len);
				stringbufferAppend(xml, "' scale='0' />");
				break;
			}
			else
			{
				data_ptr = (char *) sqlda->sqlvar[i].var_ptr;
				len_ptr = (short *) data_ptr; /* length prefix */
				data_ptr += 2; /* skip length prefix */

				if (*len_ptr < sizeof(data_array))
				{ 
					if (*len_ptr != 0) /* filter zero length */
					{
						strncpy(data_array, data_ptr, *len_ptr );
					}
					data_array[*len_ptr] = '\0';
					
					_psWriteColCh(xml, name_array, data_array);
				}
				else
				{
					errormsg = "data truncation error";
				}
			}

			break;

		case _SQLDT_16BIT_S:
			/* 16bit numeric */
			if (meta)
			{
				stringbufferAppendInt(xml, (int)sqlda->sqlvar[i].data_len & 0xFF);
				stringbufferAppend(xml, "' scale='");
				stringbufferAppendInt(xml, (int)sqlda->sqlvar[i].data_len >> 8);
				stringbufferAppend(xml, "' />");
				break;
			}
			else
			{
				_psWriteColInt(xml, name_array, (int32)*(short *)sqlda->sqlvar[i].var_ptr, (int)sqlda->sqlvar[i].data_len >> 8);
			}
			break;

		case _SQLDT_16BIT_U:
			/* 16 bit unsigned numeric */
			if (meta)
			{
				stringbufferAppendInt(xml, (int)sqlda->sqlvar[i].data_len & 0xFF);
				stringbufferAppend(xml, "' scale='");
				stringbufferAppendInt(xml, (int)sqlda->sqlvar[i].data_len >> 8);
				stringbufferAppend(xml, "' />");
				break;
			}
			else
			{
				_psWriteColUInt(xml, name_array, (uint32)*(uint16 *)sqlda->sqlvar[i].var_ptr, (int)sqlda->sqlvar[i].data_len >> 8);
			}
			break;

		case _SQLDT_INT_Y_Y:         /* YEAR TO YEAR         */
		case _SQLDT_INT_MO_MO:       /* MONTH TO MONTH       */
		case _SQLDT_INT_Y_MO:        /* YEAR TO MONTH        */
		case _SQLDT_INT_D_D:         /* DAY TO DAY           */
		case _SQLDT_INT_H_H:         /* HOUR TO HOUR         */
		case _SQLDT_INT_D_H:         /* DAY TO HOUR          */
		case _SQLDT_INT_MI_MI:       /* MINUTE TO MINUTE     */
		case _SQLDT_INT_H_MI:        /* HOUR TO MINUTE       */
		case _SQLDT_INT_D_MI:        /* DAY TO MINUTE        */
		case _SQLDT_INT_S_S:         /* SECOND TO SECOND     */
		case _SQLDT_INT_MI_S:        /* MINUTE TO SECOND     */
		case _SQLDT_INT_H_S:         /* HOUR TO SECOND       */
		case _SQLDT_INT_D_S:         /* DAY TO SECOND        */
		case _SQLDT_INT_F_F:         /* FRACTION TO FRACTION */
		case _SQLDT_INT_S_F:         /* SECOND TO FRACTION   */
		case _SQLDT_INT_MI_F:        /* MINUTE TO FRACTION   */
		case _SQLDT_INT_H_F:         /* HOUR TO FRACTION     */
		case _SQLDT_INT_D_F:		/* DAY TO FRACTION      */
		case _SQLDT_32BIT_S :		/* 32 bit signed numeric */
			if (meta)
			{
				stringbufferAppendInt(xml, (int)sqlda->sqlvar[i].data_len & 0xFF);
				stringbufferAppend(xml, "' scale='");
				stringbufferAppendInt(xml, (int)sqlda->sqlvar[i].data_len >> 8);
				stringbufferAppend(xml, "' />");
				break;
			}
			else
			{
				_psWriteColInt(xml, name_array, (int32)*(int32 *)sqlda->sqlvar[i].var_ptr, (int)sqlda->sqlvar[i].data_len >> 8);
			}
			break;

		case _SQLDT_32BIT_U:
			/* 32 bit unsigned numeric */
			if (meta)
			{
				stringbufferAppendInt(xml, (int)sqlda->sqlvar[i].data_len & 0xFF);
				stringbufferAppend(xml, "' scale='");
				stringbufferAppendInt(xml, (int)sqlda->sqlvar[i].data_len >> 8);
				stringbufferAppend(xml, "' />");
				break;
			}
			else
			{
				_psWriteColUInt(xml, name_array, (uint32)*(uint32 *)sqlda->sqlvar[i].var_ptr, (int)sqlda->sqlvar[i].data_len >> 8);
			}
			break;

		case _SQLDT_DEC_LSS:    /* DECIMAL datatype:                */
                                /* leading sign is separate         */
		case _SQLDT_DEC_LSE:    /* DECIMAL datatype:                */
                                /* leading sign is embedded         */
		case _SQLDT_DEC_TSS:    /* DECIMAL datatype:                */
                                /* trailing sign separate           */
		case _SQLDT_DEC_TSE:    /* DECIMAL datatype:                */
                                /* trailing sign embedded           */
		case _SQLDT_DEC_U:         
			/* DECIMAL datatype: unsigned       */
			if (meta)
			{
				stringbufferAppendInt(xml, (int)sqlda->sqlvar[i].data_len & 0xFF);
				stringbufferAppend(xml, "' scale='");
				stringbufferAppendInt(xml, (int)sqlda->sqlvar[i].data_len >> 8);
				stringbufferAppend(xml, "' />");
				break;
			}
			else
			{
				_psWriteColDec(xml, name_array, (char *)sqlda->sqlvar[i].var_ptr, (int)sqlda->sqlvar[i].data_len & 0xFF, (int)sqlda->sqlvar[i].data_len >> 8);
			}
			break;
				
		case _SQLDT_64BIT_S:       /* 64 bit signed binary number      */
                                /* LARGEINT, NUMERIC(10-18,<scale>) */
			if (meta)
			{
				stringbufferAppendInt(xml, (int)sqlda->sqlvar[i].data_len & 0xFF);
				stringbufferAppend(xml, "' scale='");
				stringbufferAppendInt(xml, (int)sqlda->sqlvar[i].data_len >> 8);
				stringbufferAppend(xml, "' />");
				break;
			}
			else
			{		
				_psWriteColInt64(xml, name_array, (int64)*(int64 *)sqlda->sqlvar[i].var_ptr, (int)sqlda->sqlvar[i].data_len >> 8);
			}
			break;
			
		case _SQLDT_DATETIME:      /* DATETIME datatype                */
			if (meta)
			{
				stringbufferAppendInt(xml, (int)sqlda->sqlvar[i].data_len & 0xFF);
				stringbufferAppend(xml, "' scale='");
				stringbufferAppendInt(xml, (int)sqlda->sqlvar[i].data_len >> 8);
				stringbufferAppend(xml, "' />");
			}
			else
			{
				_psWriteColDec(xml, name_array, (char *)sqlda->sqlvar[i].var_ptr, (int)sqlda->sqlvar[i].data_len & 0xFF, (int)sqlda->sqlvar[i].data_len >> 8);
			}
			break;
					
		case _SQLDT_REAL:          /* 32 bit FLOAT                     */
			if (meta)
			{
				stringbufferAppendInt(xml, (int)sqlda->sqlvar[i].data_len & 0xFF);
				stringbufferAppend(xml, "' scale='");
				stringbufferAppendInt(xml, (int)sqlda->sqlvar[i].data_len >> 8);
				stringbufferAppend(xml, "' />");
			}
			else
			{
				_psWriteColDouble(xml, name_array, (float64)*(float32 *)sqlda->sqlvar[i].var_ptr);
			}
			break;
			
		case _SQLDT_DOUBLE:        /* 64 bit FLOAT                     */
			if (meta)
			{
				stringbufferAppendInt(xml, (int)sqlda->sqlvar[i].data_len & 0xFF);
				stringbufferAppend(xml, "' scale='");
				stringbufferAppendInt(xml, (int)sqlda->sqlvar[i].data_len >> 8);
				stringbufferAppend(xml, "' />");
			}
			else
			{
				_psWriteColDouble(xml, name_array, *(float64 *)sqlda->sqlvar[i].var_ptr);
			}
			break;
		default: 
			stringbufferAppend(xml, "' scale='");
			stringbufferAppendInt(xml, 0);
			stringbufferAppend(xml, "' />");
			printf( "**** Error for %-40s: %s Datatype is unsupported.\n", name_array, get_dtname(sqlda->sqlvar[i].data_type));
			errormsg = "unsupported data type";
			break;
		}
	}
	
	if (meta)
	{
		stringbufferAppend(xml, "</META>\r\n");
	}
	else
	{
		stringbufferAppend(xml, "/>\r\n");
	}
	
	if (errormsg != NULL)
	{
		_writeError(xml, errormsg, "");
	}
}

void preparedstatementExec(preparedstatement* stmt, request* req, stringbuffer* xml)
{
	_preparedstatementSqlInner(stmt, false, NULL, req, xml);
}

#ifdef DEBUG

static void prepstmtValidateSqlDa(struct SQLDA_TYPE* sqlda)
{
	int i;
	
	if (sqlda == NULL)
	{
		return;
	}
		
	for (i = 0; i < sqlda->num_entries; i++)
	{
		if (sqlda->sqlvar[i].var_ptr != 0)
		{
			ASSERT_PTR((void *)sqlda->sqlvar[i].var_ptr);
		}
	}
}

void preparedstatementValidateMem(preparedstatement* stmt)
{
	if (stmt->m_name != NULL)
	{
		stringValidateMem(stmt->m_name);
	}
	if (stmt->m_system != NULL)
	{
		stringValidateMem(stmt->m_system);
	}
	if (stmt->m_fqname)
	{
		stringValidateMem(stmt->m_fqname);
	}
	
	if (stmt->m_sqldaIn != NULL)
	{
		ASSERT_MEM(stmt->m_sqldaIn, SIZEOF_SQLDA(stmt->m_numInputs));
		prepstmtValidateSqlDa(stmt->m_sqldaIn);
	}
	if (stmt->m_sqldaOut != NULL)
	{
		ASSERT_MEM(stmt->m_sqldaOut, SIZEOF_SQLDA(stmt->m_numOutputs));
		prepstmtValidateSqlDa(stmt->m_sqldaOut);
	}
	if (stmt->m_procDef != NULL)
	{
		sqlprocValidateMem(stmt->m_procDef);
	}
}
#endif

#include <spl/data/sqliteconnection.h>
#include <spl/data/sqlitecommand.h>
#include <spl/data/sqlitereader.h>

bool createProcedureTable()
{
	bool ret = true;
	
	if (! fileExists(g_dbName))
	{
		const char* ssql = "CREATE TABLE SPROCS (SYS_CODE CHAR(5), SP_NAME VARCHAR(40), SP_TEXT VARCHAR(8096), PRIMARY KEY (SYS_CODE, SP_NAME));";
		const char* psql = "CREATE TABLE PERMS (UID_NAME VARCHAR(40), PERM_NAME VARCHAR(80), PRIMARY KEY(UID_NAME, PERM_NAME));";
		const char* usql = "CREATE TABLE USERS (UID_NAME VARCHAR(40), PW_TEXT VARCHAR(40), ADMIN_FL INTEGER, PRIMARY KEY(UID_NAME));";

		sqlitecommand* cmd;
		sqliteconnection* conn = sqliteconnNew(g_dbName);
		
		if (conn == NULL)
		{
			return false;
		}
		
		if (! sqliteconnOpen(conn))
		{
			sqliteconnDelete(conn);
			return false;			
		}
		
		if (NULL == (cmd = sqliteconnCreateCommand(conn, ssql)))
		{
			sqliteconnDelete(conn);
			return false;
		}
		ret = sqlitecommandExecuteNonQuery(cmd) > 0;
		sqlitecommandDelete(cmd);
		
		if (NULL == (cmd = sqliteconnCreateCommand(conn, psql)))
		{
			sqliteconnDelete(conn);
			return false;
		}
		ret &= sqlitecommandExecuteNonQuery(cmd) > 0;
		sqlitecommandDelete(cmd);
		
		if (NULL == (cmd = sqliteconnCreateCommand(conn, usql)))
		{
			sqliteconnDelete(conn);
			return false;
		}
		ret &= sqlitecommandExecuteNonQuery(cmd) > 0;
		sqlitecommandDelete(cmd);
		
		if (NULL == (cmd = sqliteconnCreateCommand(conn, "INSERT INTO USERS VALUES ('SA', '1234', 1);")))
		{
			sqliteconnDelete(conn);
			return false;
		}
		ret &= sqlitecommandExecuteNonQuery(cmd) > 0;
		sqlitecommandDelete(cmd);

		sqliteconnClose(conn);
		sqliteconnDelete(conn);
	}
	
	return ret;
}

static bool _loadProcedureInner(hashtable* stmts, sqlitereader* reader)
{
	while (sqlitereaderRead(reader))
	{
		int errorcode;
		preparedstatement* stmt;
		
		const char* cpsys = sqlitereaderText(reader, "SYS_CODE");
		string* system = stringNew(cpsys);
		const char* cpsql = sqlitereaderText(reader, "SP_TEXT");
		string* sql = stringNew(cpsql);

		if (NULL == (stmt = preparedstatementNew(system, sql, hashtableCount(stmts), &errorcode)))
		{
			stringDelete(system);
			stringDelete(sql);
			return false;
		}

		stringDelete(system);
		stringDelete(sql);
		
		if (! hashtablePut(stmts, stmt->m_fqname->m_cstr, stmt, false))
		{
			return false;
		}
		
		preparedstatementValidateMem(stmt);
	}
	
	return true;
}

vector* loadAllSystems()
{
	vector* ret = NULL;
	sqlitereader* reader;
	sqlitecommand* cmd;
	sqliteconnection* conn = sqliteconnNew(g_dbName);
	
	if (conn == NULL)
	{
		return NULL;
	}
	
	if (! sqliteconnOpen(conn))
	{
		sqliteconnDelete(conn);
		return NULL;			
	}
	
	if (NULL == (cmd = sqliteconnCreateCommand(conn, "SELECT DISTINCT SYS_CODE FROM SPROCS;")))
	{
		sqliteconnDelete(conn);
		return false;
	}

	if (NULL != (reader = sqlitecommandExecuteQuery(cmd)))
	{
		if (NULL != (ret = vectorNew()))
		{
			while (sqlitereaderRead(reader))
			{
				const char* cpsys = sqlitereaderText(reader, "SYS_CODE");
				string* system = stringNew(cpsys);
				vectorAdd(ret, system);
			}
			
			sqlitereaderDelete(reader);
		}
	}
	
	sqlitecommandDelete(cmd);
	sqliteconnClose(conn);
	sqliteconnDelete(conn);
	
	return ret;
}

bool loadAllProcedures(hashtable* stmts)
{
	bool ret = true;
	sqlitereader* reader;
	sqlitecommand* cmd;
	sqliteconnection* conn = sqliteconnNew(g_dbName);
	
	if (conn == NULL)
	{
		return false;
	}
	
	if (! sqliteconnOpen(conn))
	{
		sqliteconnDelete(conn);
		return false;			
	}
	
	if (NULL == (cmd = sqliteconnCreateCommand(conn, "SELECT * FROM SPROCS;")))
	{
		sqliteconnDelete(conn);
		return false;
	}

	if (NULL == (reader = sqlitecommandExecuteQuery(cmd)))
	{
		ret = false;
	}
	else
	{
		ret = _loadProcedureInner(stmts, reader);

		sqlitereaderDelete(reader);
	}
	
	sqlitecommandDelete(cmd);
	sqliteconnClose(conn);
	sqliteconnDelete(conn);
	
	return ret;
}

bool saveProcedure(string* system, string* name, string* sql)
{
	bool ret = false;
	sqlitecommand* cmd;
	sqliteconnection* conn = sqliteconnNew(g_dbName);
	
	if (conn == NULL)
	{
		return false;
	}
	
	if (! sqliteconnOpen(conn))
	{
		sqliteconnDelete(conn);
		return false;			
	}
	
	if (NULL == (cmd = sqliteconnCreateCommand(conn, "INSERT INTO SPROCS (SYS_CODE, SP_NAME, SP_TEXT) VALUES (@SYS_CODE, @SP_NAME, @SP_TEXT)")))
	{
		sqliteconnDelete(conn);
		return false;
	}

	if (sqlitecommandSetParameterText(cmd, "@SYS_CODE", system->m_cstr))
	{
		if (sqlitecommandSetParameterText(cmd, "@SP_NAME", name->m_cstr))
		{
			if (sqlitecommandSetParameterText(cmd, "@SP_TEXT", sql->m_cstr))
			{
				ret = sqlitecommandExecuteNonQuery(cmd) > 0;
			}
		}
	}

	sqlitecommandDelete(cmd);
	sqliteconnClose(conn);
	sqliteconnDelete(conn);
	
	return ret;
}

bool deleteProcedure(string* system, string* name)
{
	bool ret = false;
	sqlitecommand* cmd;
	sqliteconnection* conn = sqliteconnNew(g_dbName);
	
	if (conn == NULL)
	{
		return false;
	}
	
	if (! sqliteconnOpen(conn))
	{
		sqliteconnDelete(conn);
		return false;			
	}
	
	if (NULL == (cmd = sqliteconnCreateCommand(conn, "DELETE FROM SPROCS WHERE SYS_CODE = @SYS_CODE AND SP_NAME = @SP_NAME;")))
	{
		sqliteconnDelete(conn);
		return false;
	}

	if (sqlitecommandSetParameterText(cmd, "@SYS_CODE", system->m_cstr))
	{
		if (sqlitecommandSetParameterText(cmd, "@SP_NAME", name->m_cstr))
		{
			ret = sqlitecommandExecuteNonQuery(cmd) > 0;
		}
	}

	sqlitecommandDelete(cmd);
	sqliteconnClose(conn);
	sqliteconnDelete(conn);
	
	return ret;
}

bool updateProcedure(hashtable* stmts, string* system, string* name)
{
	bool ret = true;
	sqlitereader* reader;
	sqlitecommand* cmd;
	sqliteconnection* conn = sqliteconnNew(g_dbName);
	
	if (conn == NULL)
	{
		return false;
	}
	
	if (! sqliteconnOpen(conn))
	{
		sqliteconnDelete(conn);
		return false;			
	}
	
	if (NULL == (cmd = sqliteconnCreateCommand(conn, "SELECT * FROM SPROCS WHERE SYS_CODE = @SYS_CODE AND SP_NAME = @SP_NAME;")))
	{
		sqliteconnDelete(conn);
		return false;
	}

	if (sqlitecommandSetParameterText(cmd, "@SYS_CODE", system->m_cstr))
	{
		if (sqlitecommandSetParameterText(cmd, "@SP_NAME", name->m_cstr))
		{
			if (NULL == (reader = sqlitecommandExecuteQuery(cmd)))
			{
				ret = false;
			}
			else
			{
				ret = _loadProcedureInner(stmts, reader);
				sqlitereaderDelete(reader);
			}
		}
	}
	
	sqlitecommandDelete(cmd);
	sqliteconnClose(conn);
	sqliteconnDelete(conn);
	
	return ret;	
}
