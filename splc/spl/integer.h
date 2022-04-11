#ifndef _integer_h
#define _integer_h

#include <spl/types.h>
#include <spl/debug.h>
#include <spl/string.h>

int32 int32Parse(const char* cp, const int cplen, int radix);
int64 int64Parse(const char* cp, const int cplen, int radix);
string* int32ToString(int32 i);
string* int64ToString(int64 i);

#endif
