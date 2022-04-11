#ifndef _sqlparam_h
#define _sqlparam_h

#include <spl/types.h>
#include <spl/debug.h>
#include <spl/string.h>

#include "sqllex.h"

typedef struct _sqlparam
{
	string* m_name;
	string* m_sqltyperaw;
	string* m_sqltype;
	int m_length;
	int m_scale;
	bool m_isout;
	string* m_indicator;
} sqlparam;

sqlparam* sqlparamNew(sqllex* lex, int* errorcode);
void sqlparamDelete(sqlparam* param);
char sqlparamPacketType(sqlparam* param);
const char* sqlparamCType(sqlparam* param);

#ifdef DEBUG
void sqlparamValidateMem(sqlparam* stmt);
#else
#define sqlparamValidateMem(r)
#endif

#ifndef __TANDEM
/* Help for debugging on win */
#define _SQLDT_ASCII_F      0   /* CHAR datatype                    */
#define _SQLDT_ASCII_F_UP   1   /* CHAR datatype, UPSHIFTed         */
#define _SQLDT_DOUBLE_F     2   /* DOUBLE CHAR datatype             */
#define _SQLDT_ASCII_V     64   /* VARCHAR datatype                 */
#define _SQLDT_ASCII_V_UP  65   /* VARCHAR datatype, UPSHIFted      */
#define _SQLDT_DOUBLE_V    66   /* DOUBLE VARCHAR datatype          */
#define _SQLDT_16BIT_S    130   /* 16 bit signed binary number      */
                                /* SMALLINT, NUMERIC(1-4, <scale>)  */
#define _SQLDT_16BIT_U    131   /* 16 bit unsigned binary number    */
                                /* unsigned NUMERIC(1-4, <scale>)   */
#define _SQLDT_32BIT_S    132   /* 32 bit signed binary number      */
                                /* INTEGER, NUMERIC(5-9, <scale>)   */
#define _SQLDT_32BIT_U    133   /* 32 bit unsigned binary number    */
                                /* unsigned NUMERIC(5-9, <scale>)   */
#define _SQLDT_64BIT_S    134   /* 64 bit signed binary number      */
                                /* LARGEINT, NUMERIC(10-18,<scale>) */
#define _SQLDT_REAL       140   /* 32 bit FLOAT                     */
#define _SQLDT_DOUBLE     141   /* 64 bit FLOAT                     */
#define _SQLDT_DEC_U      150   /* DECIMAL datatype: unsigned       */
#define _SQLDT_DEC_LSS    151   /* DECIMAL datatype:                */
                                /* leading sign is separate         */
#define _SQLDT_DEC_LSE    152   /* DECIMAL datatype:                */
                                /* leading sign is embedded         */
#define _SQLDT_DEC_TSS    153   /* DECIMAL datatype:                */
                                /* trailing sign separate           */
#define _SQLDT_DEC_TSE    154   /* DECIMAL datatype:                */
                                /* trailing sign embedded           */
#define _SQLDT_DATETIME   192   /* DATETIME datatype                */

/*
 * INTERVAL datatype:
 */
#define _SQLDT_INT_Y_Y    195     /* YEAR TO YEAR         */
#define _SQLDT_INT_MO_MO  196     /* MONTH TO MONTH       */
#define _SQLDT_INT_Y_MO   197     /* YEAR TO MONTH        */
#define _SQLDT_INT_D_D    198     /* DAY TO DAY           */
#define _SQLDT_INT_H_H    199     /* HOUR TO HOUR         */
#define _SQLDT_INT_D_H    200     /* DAY TO HOUR          */
#define _SQLDT_INT_MI_MI  201     /* MINUTE TO MINUTE     */
#define _SQLDT_INT_H_MI   202     /* HOUR TO MINUTE       */
#define _SQLDT_INT_D_MI   203     /* DAY TO MINUTE        */
#define _SQLDT_INT_S_S    204     /* SECOND TO SECOND     */
#define _SQLDT_INT_MI_S   205     /* MINUTE TO SECOND     */
#define _SQLDT_INT_H_S    206     /* HOUR TO SECOND       */
#define _SQLDT_INT_D_S    207     /* DAY TO SECOND        */
#define _SQLDT_INT_F_F    208     /* FRACTION TO FRACTION */
#define _SQLDT_INT_S_F    209     /* SECOND TO FRACTION   */
#define _SQLDT_INT_MI_F   210     /* MINUTE TO FRACTION   */
#define _SQLDT_INT_H_F    211     /* HOUR TO FRACTION     */
#define _SQLDT_INT_D_F    212     /* DAY TO FRACTION      */

/*
 * DATETIME OR INTERVAL qualifiers:
 */
#define _SQL_DTINT_QUAL_Y_Y    1    /* YEAR TO YEAR         */
#define _SQL_DTINT_QUAL_MO_MO  2    /* MONTH TO MONTH       */
#define _SQL_DTINT_QUAL_D_D    3    /* DAY TO DAY           */
#define _SQL_DTINT_QUAL_H_H    4    /* HOUR TO HOUR         */
#define _SQL_DTINT_QUAL_MI_MI  5    /* MINUTE TO MINUTE     */
#define _SQL_DTINT_QUAL_S_S    6    /* SECOND TO SECOND     */
#define _SQL_DTINT_QUAL_F_F    7    /* FRACTION TO FRACTION */
#define _SQL_DTINT_QUAL_Y_MO   8    /* YEAR TO MONTH        */
#define _SQL_DTINT_QUAL_Y_D    9    /* YEAR TO DAY          */
#define _SQL_DTINT_QUAL_Y_H   10    /* YEAR TO HOUR         */
#define _SQL_DTINT_QUAL_Y_MI  11    /* YEAR TO MINUTE       */
#define _SQL_DTINT_QUAL_Y_S   12    /* YEAR TO SECOND       */
#define _SQL_DTINT_QUAL_Y_F   13    /* YEAR TO FRACTION     */
#define _SQL_DTINT_QUAL_MO_D  14    /* MONTH TO DAY         */
#define _SQL_DTINT_QUAL_MO_H  15    /* MONTH TO HOUR        */
#define _SQL_DTINT_QUAL_MO_MI 16    /* MONTH TO MINUTE      */
#define _SQL_DTINT_QUAL_MO_S  17    /* MONTH TO SECOND      */
#define _SQL_DTINT_QUAL_MO_F  18    /* MONTH TO FRACTION    */
#define _SQL_DTINT_QUAL_D_H   19    /* DAY TO HOUR          */
#define _SQL_DTINT_QUAL_D_MI  20    /* DAY TO MINUTE        */
#define _SQL_DTINT_QUAL_D_S   21    /* DAY TO SECOND        */
#define _SQL_DTINT_QUAL_D_F   22    /* DAY TO FRACTION      */
#define _SQL_DTINT_QUAL_H_MI  23    /* HOUR TO MINUTE       */
#define _SQL_DTINT_QUAL_H_S   24    /* HOUR TO SECOND       */
#define _SQL_DTINT_QUAL_H_F   25    /* HOUR TO FRACTION     */
#define _SQL_DTINT_QUAL_MI_S  26    /* SECOND TO SECOND     */
#define _SQL_DTINT_QUAL_MI_F  27    /* SECOND TO FRACTION   */
#define _SQL_DTINT_QUAL_S_F   28    /* SECOND TO FRACTION   */

/*
 * Dynamic SQL statement types:
 */
#define _SQL_STATEMENT_SELECT    1  /* select (cursor) statement    */
#define _SQL_STATEMENT_INSERT    2  /* insert statement             */
#define _SQL_STATEMENT_UPDATE    3  /* update statement             */
#define _SQL_STATEMENT_DELETE    4  /* delete statement             */
#define _SQL_STATEMENT_DDL       5  /* DDL statement                */
#define _SQL_STATEMENT_CONTROL   6  /* run-time control statement   */
#define _SQL_STATEMENT_DCL       7  /* lock, unlock, free resources */
#define _SQL_STATEMENT_GET       8  /* get statement                */

/* CHARACTER SET IDs */
#define _SQL_CHARSETID_UNKNOWN       0 /* charset id for UNKNOWN   */
#define _SQL_CHARSETID_KANJI         1 /* charset id for KANJI     */
#define _SQL_CHARSETID_BIG5         10 /* charset id for BIG5      */
#define _SQL_CHARSETID_KSC5601      12 /* charset id for KSC5601   */
#define _SQL_CHARSETID_ISO88591    101 /* charset id for ISO88591  */
#define _SQL_CHARSETID_ISO88592    102 /* charset id for ISO88592  */
#define _SQL_CHARSETID_ISO88593    103 /* charset id for ISO88593  */
#define _SQL_CHARSETID_ISO88594    104 /* charset id for ISO88594  */
#define _SQL_CHARSETID_ISO88595    105 /* charset id for ISO88595  */
#define _SQL_CHARSETID_ISO88596    106 /* charset id for ISO88596  */
#define _SQL_CHARSETID_ISO88597    107 /* charset id for ISO88597  */
#define _SQL_CHARSETID_ISO88598    108 /* charset id for ISO88598  */
#define _SQL_CHARSETID_ISO88599    109 /* charset id for ISO88599  */
#endif

#endif
