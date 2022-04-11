#ifndef _interperrors_h
#define _interperrors_h

#ifdef _WIN32
#include <spl/configwin32.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

enum InterpErrors
{
	IERR_NONE = 0,
	IERR_OUT_OF_MEM = 1,
	IERR_MAIN_CLASS_NOT_FOUND = 2,
	IERR_CLASS_NOT_FOUND = 3,
	IERR_UNKNOWN_METHOD = 4,
	IERR_UNKNOWN_OPCODE = 5,
	IERR_FIELD_NOT_FOUND = 6,
	IERR_TYPE_MISMATCH = 7,
	IERR_INTERNAL_ERROR1 = 8,
	IERR_INTERNAL_ERROR2 = 9,
	IERR_BAD_BREAK = 10,
	IERR_BAD_CONTINUE = 11,
	IERR_EXCEPTION = 12,
	IERR_CALL_REQUIRES_THIS = 13,
	IERR_INDEX_OUT_OF_BOUNDS = 14,
	IERR_WRONG_ARRAY_INDEXS = 15,
	IERR_RETURN = 16
};

#ifdef __cplusplus
}
#endif

#endif
