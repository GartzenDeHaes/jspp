#ifndef _real_h
#define _real_h

#include <spl/types.h>
#include <spl/debug.h>
#include <spl/string.h>

float64 float64Parse(const char* cp, const int cplen);
string* float64ToString(float64 d);

#endif
