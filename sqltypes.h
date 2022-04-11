#ifndef _sqltypes_h
#define _sqltypes_h

#ifdef _WIN32
#include <spl/configwin32.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef enum _datatype
{
	TYPE_INT = 0,
	TYPE_FLOAT4 = 1,
	TYPE_FLOAT8 = 2,
	TYPE_CHAR = 3,
	TYPE_LONG = 4,
	TYPE_SHORT = 5,
	TYPE_VCHAR = 6
} datatype;

extern char *datatypename[];

#ifdef __cplusplus
}
#endif

#endif
