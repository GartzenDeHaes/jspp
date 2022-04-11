#ifndef _errors_h
#define _errors_h

extern char* sqlErrorMessages[];

enum sqlErrorMessages
{
	SQLE_OK = 0,
	SQLE_OUTOFMEM = 1,
	SQLE_EXP_CREATE = 2,
	SQLE_EXP_PROC = 3,
	SQLE_EXP_LPAR = 4,
	SQLE_EMPTY = 5,
	SQLE_SYNTAX = 6,
	SQLE_TOOLONG = 7,
	SQLE_VARBUFS = 8
};

#endif
