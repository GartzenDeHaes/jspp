#include "errors.h"

char* sqlErrorMessages[] = {
	"OK",					/* 0 */
	"out of memory",		/* 1 */
	"expected CREATE",		/* 2 */
	"expected PROCEDURE",	/* 3 */
	"expected ( to start parameter list", /* 4 */ 
	"sql text empty",		/* 5 */
	"syntax error",			/* 6 */
	"sql too long",			/* 7 */
	"setupvarbuffers failed", /* 8 */
	""
};
