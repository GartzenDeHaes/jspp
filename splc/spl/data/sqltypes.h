#ifndef _sqltypes_h
#define _sqltypes_h

enum
{
	SQL_TYPE_UNASSIGNED = 0,
	SQL_TYPE_INT8 = 1,
	SQL_TYPE_INT16 = 2,
	SQL_TYPE_INT32 = 3,
	SQL_TYPE_INT64 = 4,
	SQL_TYPE_DECIMAL = 5,	/* MYSQL: 65 digit fixed point */
	SQL_TYPE_FLOAT32 = 6,
	SQL_TYPE_FLOAT64 = 7,
	SQL_TYPE_FLAG = 8,
	SQL_TYPE_TIMESTAMP = 9,
	SQL_TYPE_DATE = 10,
	SQL_TYPE_DATETIME = 11,
	SQL_TYPE_CHAR = 12,
	SQL_TYPE_VARCHAR = 13,
	SQL_TYPE_BLOB = 14
};

#endif
