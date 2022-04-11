#ifndef _program_h
#define _program_h

#ifdef _WIN32
#include <spl/configwin32.h>
#endif

#include <spl/types.h>
#include <spl/collection/cvector.h>
#include <spl/collection/chashtable.h>
#include <spl/collection/clist.h>

#include <jspp/opcodes.h>
#include <jspp/jsclass.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct symtab
{
	int cumulative;
	int count;
	int patchupPc;
	hashtable *syms;
	vector *symlist;
} symtab;

typedef struct program
{
	vector *sourcecode;
	int errornum;
	int errorline;
	string *errmsg;
	bool isrhs;

	hashtable *classes;
	vector *lclasses;
	jsclass *curclass;
	jsmethod *curmethod;
	vector *cdata;		/* constants, contains (struct variant)'s, not pointers */

	vector *intrinsics; /* native classes defined by qinterp */

	vector *symstack;
	bool isdebuging;

} program;

program *programConstruct();
void programDestruct( program * );
jsclass *programDefineClass( program *pp, const char *name, const char *basename, void *privateclassdata );
jsmethod *programDefineMethod( program *pp, const char *name, const bool isstatic, const enum jsaccess access );
jsmethod *programDefineNativeMethod( program *pp, NATIVEMETHOD meth, const char *name, const bool isstatic, const enum jsaccess access, const bool hasretvalue );
vardef *programDefineNativeField( program *pp, NATIVEMETHOD meth, const char *name, const bool isstatic, const enum jsaccess access );
vardef *programDefineField( program *pp, const char *name, const bool isstatic, const enum jstype typ, const enum jstype subtyp );
jsclass *programFindClass( const program *pp, const char *clsname );
jsclass *programFindClassWithMethod( const program *pp, const char *methodname, const bool isstatic );
void programSetIntrinsics( program *pp, vector *programs );
bool programDump( const program *pp, const char *filename );

int programEmit0( program *pp, const enum OpCode op );
int programEmit1( program *pp, const enum OpCode op, const enum ArgSource mode );
int programEmit2( program *pp, const enum OpCode op, const enum ArgSource mode, const enum ArgSource mode2 );
int programEmitArg( program *pp, const int32 arg );
bool programEmitArgAt( program *pp, const int pc, const int32 arg );
bool programFixUpPc( program *pp, int const fixuppcat );
int programEmitConstInt( program *pp, const int32 val );
int programEmitConstFloat( program *pp, const float val );
int programEmitConstChars( program *pp, const char * val );
int programEmitConstChar( program *pp, const char val );
int blockDecode( const struct int16array *code, const int pc, enum OpCode *op, int16 *argcount, enum ArgSource *mode, enum ArgSource *mode2 );
int blockFetchInt32( const struct int16array *code, const int pc, int32 *val );

#ifdef DEBUG2
void decodeOp(const uint16 opcode, enum OpCode *op, int16 *argcount,enum ArgSource *mode,enum ArgSource *mode2);
#else
#define decodeOp(opcode,op,argcount,mode,mode2) \
	*op = (enum OpCode)(opcode & (1<<0 | 1<<1 | 1<<2 | 1<<3 | 1<<4 | 1<<5 | 1<<6)); \
	*argcount = (int16)((opcode & (1<<0 | 1<<1)<<7)>>7); \
	*mode = (enum ArgSource)((opcode & (1<<0 | 1<<1 | 1<<2)<<9)>>9); \
	*mode2 = (enum ArgSource)((opcode & (1<<0 | 1<<1 | 1<<2)<<12)>>12); 
#endif

#if defined( DEBUG ) || defined( _DEBUG )
void programValidateMem( const program *pp );
void programCheckMem( const program *pp );
#else
#define programValidateMem(a)
#define programCheckMem(a)
#endif

#ifdef __cplusplus
}
#endif

#endif
