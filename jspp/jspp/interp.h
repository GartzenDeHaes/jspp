#ifndef _interp_h
#define _interp_h

#ifdef _WIN32
#include <spl/configwin32.h>
#endif

#include <stdarg.h>
#include <jspp/jsclass.h>
#include <jspp/program.h>
#include <jspp/interperrors.h>

#ifdef __cplusplus
extern "C" {
#endif

#define STACK_SIZE 10

typedef struct stackframe
{
	jsclass *ctx;		/* current execution class */
	jsmethod *curmethod; /* current execution method */
	vector *locals;		/* block vars */
	vector *blockframes;
	enum InterpErrors err;
	variant retval;
	int stackcount;
	int pc;
} stackframe;

typedef struct blockframe
{
	int localcount;
	int breakpc;
	int blockcount;
	int catchpc;
} blockframe;

typedef struct qinterp
{
	vector *programs;
	hashtable *classes;	/* memory for struct class(es) held by program */

	/* the following vars would need to be in a "context"
	// object to make qinterp reentrant.
	*/
	variant tempdata1;		/* used for IM and return values(?) */

	vector *frames;			/* runtime stack (of stackframes) */
	stackframe *curframe;	/* current stack frame */

	vector *stack;			/* call and return stack (struct variant)'s */

	variant null;			/* null */
	int linenum;

	vector *stkframecache;
} qinterp;

qinterp *qinterpConstruct();
void qinterpDestruct( qinterp *qip );
bool qinterpPrepare( qinterp *qip, program *pp );
void qinterpRemoveProgram( qinterp *qip, program *pp );
variant *qinterpResolve( qinterp *qip, const enum ArgSource am, const int32 arg, variant *imvar );
enum InterpErrors qinterpRun( qinterp *qip, const char *startclass, const char *startmethod, const int argcount, variant **varret );
enum InterpErrors qinterpCall( qinterp *qip, jsinst *inst, const char *methodname, variant **varret, const int argc, ... );
jsclass *qinterpFindClass( qinterp *qip, const string *clsname );
/*program *qinterpGetIntrinsics( qinterp *qip );*/
#define qinterpGetPrograms(qip) qip->programs
char *qinterpGetErrorMessage( const enum InterpErrors err, char *buf, const int buflen );

#ifdef DEBUG
void jsppRunUnitTests();
#endif

#if defined( DEBUG ) || defined( _DEBUG )
void qinterpValidateMem( const qinterp *qip );
void qinterpCheckMem( const qinterp *qip );
#else
#define qinterpValidateMem(a)
#define qinterpCheckMem(a)
#endif

#ifdef __cplusplus
}
#endif

#endif
