#ifndef _comamndline_h
#define _commandline_h

#include <spl/types.h>
#include <spl/debug.h>
#include <spl/string.h>
#include <spl/collection/hashtable.h>
#include <spl/collection/vector.h>

typedef struct _comamndline
{
	int m_argc;
	const char** m_argv;
	vector* m_args;
	hashtable* m_switches;
} commandline;

commandline* commandlineNew(const int argc, const char** argv);
void commandlineDelete(commandline* cl);

int commandlineArgCount(commandline* cl);
string* commandlineArgAt(commandline* cl, int idx);

bool commandlineHasSwitch(commandline* cl, const char* cp);
string* commandlineGetSwitch(commandline* cl, const char* cp);

#ifdef DEBUG
void commandlineValidateMem(commandline* cl);
#else
#define commandlineValidateMem(c)
#endif

#endif
