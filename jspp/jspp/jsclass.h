#ifndef _jsclass_h
#define _jsclass_h

#ifdef _WIN32
#include <spl/configwin32.h>
#endif

#include <spl/types.h>
#include <spl/cstring.h>
#include <spl/collection/chashtable.h>
#include <spl/collection/clist.h>
#include <spl/collection/cvector.h>
#include <spl/collection/intarray.h>
#include <jspp/variant.h>
#include <jspp/opcodes.h>
#include <jspp/interperrors.h>

#ifdef __cplusplus
extern "C" {
#endif

enum jsaccess
{
	ACC_PRIVATE,
	ACC_PROTECTED,
	ACC_PUBLIC
};

struct qinterp;

typedef enum InterpErrors (* NATIVEMETHOD)(struct qinterp *qip, const void *mp, variant *ret);
typedef void (* NATIVEDESTRUCTOR)( void *inst );
typedef void (* NATIVEMEMCHECK)( void *inst );

typedef struct vardef
{
	int offset;
	string *name;
	enum ArgSource srcmode;
	unsigned isstatic : 1;
	unsigned isauto : 1;
	unsigned isptr : 1;
	unsigned isnative : 1;

	NATIVEMETHOD nativefield;
} vardef;

typedef struct jsclass
{
	string *name;
	string *basename;
	struct jsclass *baseclass;
	vector *methods;
	vector *fields;

	vector *staticlocations;
	vector *instancelocations;

	hashtable *vtable;
	vector *constdata;	/* pointer to program->cdata */
	void *privateclassdata;
} jsclass;

typedef struct jsmethod
{
	string *name;
	vector *args;	
	unsigned isstatic : 1;
	unsigned isnative : 1;
	unsigned hasretval : 1;
	enum jsaccess access;
	string *sig;

	jsclass *myclass;

	int pc;	/* pc is a compile helper */
	struct int16array *code;

	NATIVEMETHOD nativemethod;
} jsmethod;

typedef struct nativedataHolder
{
	void *data;
	NATIVEDESTRUCTOR finializer;
	NATIVEMEMCHECK memcheck;
} nativedataHolder;

typedef struct jsinst
{
	jsclass *myclass;
	vector *fields;
	int refcount;

	nativedataHolder *nativedata;
	byte magic;
} jsinst;

jsclass *jsclassConstruct(const char *name, const char *basename, void *privateclassdata);
void jsclassDestruct( jsclass * );
vardef *jsclassAddField( jsclass *jsp, const char *name, const bool isstatic, const enum jstype typ, const enum jstype subtyp );
vardef *jsclassAddNativeField( jsclass *jsp, NATIVEMETHOD fld, const char *name, const bool isstatic, const enum jstype typ, const enum jstype subtyp );
vardef *jsclassFindField( const jsclass *jsp, const char *name );
jsmethod *jsclassFindMethod( const jsclass *jsp, const char *sig );
variant *jsclassGetStaticField( const jsclass *jsp, const int offset );
bool jsclassInitVtab( jsclass *jsp );
jsmethod *jsclassAddNativeMethod( jsclass *jsp, NATIVEMETHOD meth, const char *name, const bool isstatic, const enum jsaccess access, const bool returnsvalue );

jsmethod *jsmethodConstruct( jsclass *, const char *name, const bool isstatic, const enum jsaccess acc );
void jsmethodDestruct( jsmethod * );
int jsmethodAddArg( jsmethod *mp, const char *name, const bool isptr );
vardef *jsmethodGetArg( const jsmethod *mp, const int offset );
int jsmethodCodeLen( const jsmethod *mp );

string *buildMethodSig( const string *name, const bool isstatic );
void methodSigAppendArg( string *sig );

jsinst *jsinstConstruct( jsclass *myclass );
void jsinstDestruct( jsinst *inst );
void jsinstRef( jsinst *inst );
void jsinstDeref( jsinst *inst );

#if defined( DEBUG ) || defined( _DEBUG )
void jsclassValidateMem( const jsclass * );
void jsclassCheckMem( const jsclass * );
void jsinstValidateMem( const jsinst * );
void jsinstCheckMem( const jsinst * );
void jsmethodValidateMem( const jsmethod *mp );
#else
#define jsclassValidateMem(a)
#define jsclassCheckMem(a)
#define jsinstValidateMem(a)
#define jsinstCheckMem(a)
#define jsmethodValidateMem(a)

#endif

#ifdef __cplusplus
}
#endif

#endif
