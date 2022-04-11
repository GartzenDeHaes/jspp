#ifndef _httpget_h
#define _httpget_h

#include <spl/string.h>

string* httpget(const char* server, int port, const char* pathAndParams);

#endif
