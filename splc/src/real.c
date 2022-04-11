#include <spl/real.h>
#include <stdio.h>
#include <stdlib.h>

float64 float64Parse(const char* cp, const int cplen)
{
	return atof(cp);
}

string* float64ToString(float64 d)
{
	char buf[64];
	
	sprintf(buf, "%.15lf", d);
	
	return stringNew(buf);
}
