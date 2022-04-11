#ifndef _native_h
#define _native_h

#ifdef _WIN32
#include <spl/configwin32.h>
#endif

#include <jspp/interp.h>

#ifdef __cplusplus
extern "C" {
#endif

enum InterpErrors createJSClassInstance( 
	qinterp *qip, 
	string *classname, 
	jsinst **inst, 
	const int constructorArgCount,
	...
);

/**
 *  Push arguments in preparation for a call to
 *  qinterpCall()
 */
bool pushArgument( variant *arg );

/**
 *  Retrieve an argument to a native function call.  THIS
 *  is arg 0 for non-static calls.
 */
variant *getArgumentN( qinterp *qip, const int argnum );

/**
 *  Returns the number of arguments to a function call
 */
#ifdef DEBUG2
int getArgumentCount( const qinterp *qip );
#else
#define getArgumentCount(qip) vectorCount(qip->curframe->curmethod->args)
#endif

/**
 *	Note that this will return the VALUE calculated by a 
 *  native field or a pointer to the actual variant data.
 */
variant *findField( qinterp *qip, const jsinst *inst, const char *name );

bool stackPush( qinterp *qip, variant *var );

#ifdef __cplusplus
}
#endif

#endif
