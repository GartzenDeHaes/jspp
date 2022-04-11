#ifndef _variant_h
#define _variant_h

#ifdef _WIN32
#include <spl/configwin32.h>
#endif

#include <spl/types.h>
#include <spl/cstring.h>
#include <spl/collection/chashtable.h>
#include <spl/collection/carray.h>

#ifdef __cplusplus
extern "C" {
#endif

enum jstype
{
	TYPE_VOID,		/* none */
	TYPE_NULL,		/* VOID is unassigned, null is NULL */
	TYPE_INT,		/* 32bit int */
	TYPE_CHAR,
	TYPE_BOOL,		/* t/f */
	TYPE_FLOAT,		/* 32bit float */
	TYPE_INST,		/* class instance */
	TYPE_DELEGATE,	/* class method ref */
	TYPE_STRING,	/* string */
	TYPE_PTR,		/* for ref/out keywords */

	TYPE_END_OF_TYPES
};

typedef struct variant
{
	byte type;		/* enum jstype */
	union {
		int32 i;
		float f;
		string *sp;
		struct variant *vp;
		struct jsinst *ip;
	} data;
} variant;

variant *varConstruct();
variant *varConstructDup( const variant *from );
void varDestruct( variant *var );
void varClear( variant *var );
#define VARCLEAR(var) if((var)->type > TYPE_BOOL) varClear(var)
bool varCast( variant *var, const enum jstype type );
#define VARBOOL(var) ((var->type < TYPE_FLOAT)?true:varCast(var, TYPE_BOOL))
variant *varDup( const variant *var );

bool varAssign( variant *var, const variant *from );
bool varRef( variant *var, variant *ptrto );
void varDeref( variant *var );
#define VARDEREF(var) if(TYPE_PTR==var->type) varDeref(var)
bool varAssignInt( variant *var, const int32 i );
bool varAssignFloat( variant *var, const float i );
bool varAssignBool( variant *var, const bool i );
bool varAssignChar( variant *var, const char *cp );
bool varAssignCh( variant *var, const int ch );
bool varAssignInst( variant *var, struct jsinst *inst );
bool varAssignString( variant *var, string *str );

bool varAssignOpEQ( variant *dest, const variant *arg1, const variant *arg2 );
bool varAssignOpNEQ( variant *dest, const variant *arg1, const variant *arg2 );
bool varAssignOpLT( variant *dest, const variant *arg1, const variant *arg2 );
bool varAssignOpLE( variant *dest, const variant *arg1, const variant *arg2 );
bool varAssignOpGT( variant *dest, const variant *arg1, const variant *arg2 );
bool varAssignOpGE( variant *dest, const variant *arg1, const variant *arg2 );

bool varAssignOpMul( variant *dest, const variant *arg1, const variant *arg2 );
bool varAssignOpDiv( variant *dest, const variant *arg1, const variant *arg2 );
bool varAssignOpPlus( variant *dest, const variant *arg1, const variant *arg2 );
bool varAssignOpSub( variant *dest, const variant *arg1, const variant *arg2 );
bool varAssignOpMod( variant *dest, const variant *arg1, const variant *arg2 );
bool varAssignOpBinOr( variant *dest, const variant *arg1, const variant *arg2 );
bool varAssignOpBinXOr( variant *dest, const variant *arg1, const variant *arg2 );
bool varAssignOpBinAnd( variant *dest, const variant *arg1, const variant *arg2 );
bool varAssignOpRightShift( variant *dest, const variant *arg1, const variant *arg2 );
bool varAssignOpLeftShift( variant *dest, const variant *arg1, const variant *arg2 );

bool varComp( variant *arg1 );
bool varLogNot( variant *arg1 );
bool varNegate( variant *arg1 );

bool varGetInt( const variant *var, int *ip );
bool varGetFloat( const variant *var, float *fp );

string *varToString( const variant *var );
string *varTypeName( const variant *var );
bool varIsNumeric( const variant *var );
bool varIsFloat( const variant *var );


#define varIsEmpty( v ) (v->type == TYPE_VOID)

#if defined( DEBUG ) || defined( _DEBUG )
void varValidateMem( const variant *var );
void varCheckMem( const variant *var );

/* these 2 only validate the content (string, etc), not the variant structure*/
void varValidateContentMem( const variant *var );
void varCheckContentMem( const variant *var );
#else
#define varValidateMem(a)
#define varCheckMem(a)
#define varValidateContentMem(a)
#define varCheckContentMem(a)
#endif

#ifdef __cplusplus
}
#endif

#endif
