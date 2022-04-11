#include <spl/debug.h>
#include <jspp/program.h>

#ifdef DEBUG

extern uint16 encodeOp( enum OpCode op, int16 argcount, enum ArgSource mode, enum ArgSource mode2 );

/*
 *  Creates a class called "test", a static method called
 *	"call", and a static int field called "data".  call() adds
 *	1 + 1 and stores the result in the data field.
 */
void progtestEmitStaticAdd( program *pp )
{
	jsclass *cls;
	vardef *vdp;
	jsmethod *mp;

	cls = programDefineClass(pp, "test", "", NULL);
	vdp = jsclassAddField( cls, "data", true, TYPE_INT, TYPE_VOID );

	mp = programDefineMethod( pp, "@@constructor", true, ACC_PRIVATE );
	programEmit0( pp, OP_VRET );
	mp = programDefineMethod( pp, "@@constructor", false, ACC_PRIVATE );
	jsmethodAddArg( mp, "this", false );
	programEmit0( pp, OP_VRET );

	mp = programDefineMethod(pp, "call", true, ACC_PUBLIC );

	programEmit1( pp, OP_LVALUE, MODE_CLASS );
	programEmitArg( pp, vdp->offset );

	programEmit1( pp, OP_PUSH, MODE_IM );
	programEmitArg( pp, 1 );

	programEmit0( pp, OP_ASSIGN );

	programEmit1( pp, OP_LVALUE, MODE_CLASS );
	programEmitArg( pp, vdp->offset );

	programEmit1( pp, OP_PUSH, MODE_CLASS );
	programEmitArg( pp, vdp->offset );

	programEmit1( pp, OP_PUSH, MODE_IM );
	programEmitArg( pp, 1 );

	programEmit0( pp, OP_ADD );

	programEmit0( pp, OP_ASSIGN );

	programEmit0( pp, OP_VRET );
}

/*
 *  Creates a class called "test", a static method called "main",
 *	and a static method called "n".  n(i) recursively calcs and
 *	returns a factorial.
 */
void programEmitStaticFactorial( program *pp, int i )
{
	jsclass *cls;
	jsmethod *mp;

	int methodsigIdx;
	int argidx = 0;
	int pcExit;

	cls = programDefineClass(pp, "test", "", NULL);
	jsclassAddField( cls, "data", true, TYPE_INT, TYPE_VOID );

	mp = programDefineMethod( pp, "@@constructor", true, ACC_PRIVATE );
	programEmit0( pp, OP_VRET );
	mp = programDefineMethod( pp, "@@constructor", false, ACC_PRIVATE );
	jsmethodAddArg( mp, "this", false );
	programEmit0( pp, OP_VRET );

	mp = programDefineMethod(pp, "main", true, ACC_PUBLIC );

	methodsigIdx = programEmitConstChars( pp, "test.n:Svv" );
	if ( methodsigIdx < 0 )
	{
		/* out of mem*/
		ASSERT(0);
	}
	programEmit1( pp, OP_PUSH, MODE_IM );
	programEmitArg( pp, i );

	programEmit1( pp, OP_SCALL, MODE_CONST );
	programEmitArg( pp, methodsigIdx );	

	programEmit0( pp, OP_RET );

	mp = programDefineMethod(pp, "n", true, ACC_PUBLIC );
	if ( NULL == mp )
	{
		/* out of mem*/
		ASSERT(0);
	}
	argidx = jsmethodAddArg( mp, "i", false );

	/* if i <= 1, return 1; */
	programEmit1( pp, OP_PUSH, MODE_BLOCK );
	programEmitArg( pp, argidx );
	programEmit1( pp, OP_PUSH, MODE_IM );
	programEmitArg( pp, 1 );

	programEmit0( pp, OP_LTEQ );

	programEmit1( pp, OP_JMPZ, MODE_IM );
	pcExit = programEmitArg( pp, 0 );

	programEmit1( pp, OP_PUSH, MODE_IM );
	programEmitArg( pp, 1 );
	programEmit0( pp, OP_RET );

	/* return i * n(i-1); */
	programFixUpPc( pp, pcExit );

	programEmit1( pp, OP_PUSH, MODE_BLOCK );
	programEmitArg( pp, argidx );
	programEmit1( pp, OP_PUSH, MODE_IM );
	programEmitArg( pp, 1 );

	programEmit0( pp, OP_SUB );

	programEmit1( pp, OP_SCALL, MODE_CONST );
	programEmitArg( pp, methodsigIdx );	

	programEmit1( pp, OP_PUSH, MODE_BLOCK );
	programEmitArg( pp, argidx );

	programEmit0( pp, OP_MULT );

	programEmit0( pp, OP_RET );
}

void programEmitSimpleNew( program *pp, char *clsname )
{
	jsclass *cls;
	vardef *vdp;
	jsmethod *mp;
	int clsNameIdx;

	cls = programDefineClass(pp, clsname, "", NULL);
	vdp = jsclassAddField( cls, "data", false, TYPE_INT, TYPE_VOID );
	clsNameIdx = programEmitConstChars( pp, clsname );

	/* static constructor*/
	mp = programDefineMethod( pp, "@@constructor", true, ACC_PRIVATE );
	programEmit0( pp, OP_VRET );

	/* internal constructor */
	mp = programDefineMethod(pp, "@@constructor", false, ACC_PRIVATE );
	jsmethodAddArg( mp, "this", false );

	programEmit1( pp, OP_LVALUE, MODE_INST );
	programEmitArg( pp, vdp->offset );

	programEmit1( pp, OP_PUSH, MODE_IM );
	programEmitArg( pp, 0 );

	programEmit0( pp, OP_ASSIGN );

	programEmit0( pp, OP_VRET );

	/* main*/
	mp = programDefineMethod(pp, "main", true, ACC_PUBLIC );

	programEmit1( pp, OP_NEW, MODE_CONST );
	programEmitArg( pp, clsNameIdx );

	programEmit2( pp, OP_CONSTRUCT, MODE_CONST, MODE_IM );
	programEmitArg( pp, clsNameIdx );
	programEmitArg( pp, 0 );

	programEmit0( pp, OP_POP );

	programEmit0( pp, OP_VRET );
}

static void createDestroyTest(  )
{
	program *prg;
	
	logWriteCheck(NULL, "program create/destroy test ... " );

	UNIT_ASSERT_MEM_NOTED( "program createDestroyTest NULL" );

	DEBUG_CHECK_POINT_HEAP();
	prg = programConstruct();
	programValidateMem( prg );
	programCheckMem( prg );
	UNIT_ASSERT_MEM_NOTED( "program createDestroyTest" );

	programDestruct( prg );
	UNIT_ASSERT_CHECK_POINT( "program createDestroyTest" );

	logWriteOkFail( NULL );
}

static void createClassTest(  )
{
	jsclass *cls;

	logWriteCheck(NULL, "program class test ... " );

	DEBUG_CHECK_POINT_HEAP();

	cls = jsclassConstruct("test", "object", NULL);
	jsclassCheckMem( cls );
	UNIT_ASSERT_MEM_NOTED( "jsclass createClassTest" );
	
	jsclassDestruct( cls );
	UNIT_ASSERT_CHECK_POINT( "jsclass createClassTest" );

	logWriteOkFail( NULL );
}

static void createClassTestP(  )
{
	program *prg;
	jsclass *cls;

	logWriteCheck(NULL, "program create class P test ... " );

	DEBUG_CHECK_POINT_HEAP();
	prg = programConstruct();
	programCheckMem( prg );
	UNIT_ASSERT_MEM_NOTED( "program createClassTestP 1" );

	cls = programDefineClass(prg, "test", "", NULL);
	programCheckMem( prg );
	UNIT_ASSERT_MEM_NOTED( "program createClassTestP 2" );
	
	jsclassAddField( cls, "data1", 1, TYPE_INT, TYPE_VOID );
	programCheckMem( prg );
	UNIT_ASSERT_MEM_NOTED( "program createClassTestP 2" );

	programDestruct( prg );
	UNIT_ASSERT_CHECK_POINT( "program createClassTestP" );

	logWriteOkFail( NULL );
}

static void createMethodTest(  )
{
	program *prg;
	jsclass *cls;

	logWriteCheck(NULL, "program method test ... " );

	DEBUG_CHECK_POINT_HEAP();
	prg = programConstruct();

	cls = programDefineClass( prg, "test", "", NULL );
	jsmethodConstruct( cls, "call", 1, ACC_PUBLIC );
	programCheckMem( prg );
	UNIT_ASSERT_MEM_NOTED( "program createClassTestP 1" );

	programDestruct( prg );
	UNIT_ASSERT_CHECK_POINT( "program createMethodTest" );

	logWriteOkFail( NULL );
}

static void createMethodTest2(  )
{
	program *prg;
	jsclass *cls;
	jsmethod *mp;

	logWriteCheck(NULL, "program method 2 test ... " );

	DEBUG_CHECK_POINT_HEAP();
	prg = programConstruct();

	cls = programDefineClass( prg, "test", "", NULL );
	mp = jsmethodConstruct( cls, "call", 1, ACC_PUBLIC );
	jsmethodAddArg( mp, "argc", false );
	programCheckMem( prg );
	UNIT_ASSERT_MEM_NOTED( "program createClassTestP 1" );

	programDestruct( prg );
	UNIT_ASSERT_CHECK_POINT( "program createMethodTest" );

	logWriteOkFail( NULL );
}

static void encodeDecodeTest(  )
{
	enum ArgSource mode;
	enum ArgSource mode2;
	enum OpCode op;
	int16 argcount;
	uint16 encoded;

	logWriteCheck(NULL, "program opcode encode/decode test ... " );

	encoded = encodeOp(OP_ADD, 1, MODE_BLOCK, (enum ArgSource)0);
	decodeOp( encoded, &op, &argcount, &mode, &mode2 );
	UNIT_ASSERT( "op code 1", op == OP_ADD );
	UNIT_ASSERT( "op code 2", mode == MODE_BLOCK );
	UNIT_ASSERT( "op code 5", argcount == 1 );

	encoded = encodeOp(OP_ADD, 2, MODE_IM, MODE_IM);
	decodeOp( encoded, &op, &argcount, &mode, &mode2 );
	UNIT_ASSERT( "op code 1", op == OP_ADD );
	UNIT_ASSERT( "op code 3", mode == MODE_IM );
	UNIT_ASSERT( "op code 3", mode2 == MODE_IM );
	UNIT_ASSERT( "op code 5", argcount == 2 );

	encoded = encodeOp(OP_ADD, 1, MODE_CLASS, (enum ArgSource)0);
	decodeOp( encoded, &op, &argcount, &mode, &mode2 );
	UNIT_ASSERT( "op code 1", op == OP_ADD );
	UNIT_ASSERT( "op code 2", mode == MODE_CLASS );
	UNIT_ASSERT( "op code 5", argcount == 1 );

	encoded = encodeOp( OP_LTEQ, 0, (enum ArgSource)0, (enum ArgSource)0 );
	decodeOp( encoded, &op, &argcount, &mode, &mode2 );
	UNIT_ASSERT( "op code 1", op == OP_LTEQ );
	UNIT_ASSERT( "op code 2", mode == 0 );
	UNIT_ASSERT( "op code 5", argcount == 0 );

	logWriteOkFail( NULL );
}

static void encodeDecodeTest2(  )
{
	program *pp;
	jsclass *cls;
	vardef *vdp;
	int pc;
	int32 val;

	enum ArgSource mode;
	enum ArgSource mode2;
	enum OpCode op;
	int16 argcount;

	logWriteCheck(NULL, "program opcode encode/decode 2 test ... " );

	pp = programConstruct();
	cls = programDefineClass(pp, "test", "", NULL);
	vdp = jsclassAddField( cls, "data", 1, TYPE_INT, TYPE_VOID );
	programDefineMethod(pp, "call", 1, ACC_PUBLIC );

	programEmit1( pp, OP_PUSH, MODE_CLASS );
	programEmitArg( pp, vdp->offset );

	programEmit1( pp, OP_PUSH, MODE_IM );
	programEmitArg( pp, 1 );

	programEmit0( pp, OP_ADD );
	programEmit0( pp, OP_RET );

	pc = blockDecode( pp->curmethod->code, 0, &op, &argcount, &mode, &mode2 );
	UNIT_ASSERT("enc2", pc == 1 );
	UNIT_ASSERT("enc2", op == OP_PUSH );
	UNIT_ASSERT("enc2", mode == MODE_CLASS );
	UNIT_ASSERT("enc2", argcount == 1 );

	pc = blockFetchInt32( pp->curmethod->code, pc, &val );
	UNIT_ASSERT( "enc2", pc == 3 );
	UNIT_ASSERT( "enc2", val == 0 );

	pc = blockDecode( pp->curmethod->code, pc, &op, &argcount, &mode, &mode2 );
	UNIT_ASSERT("enc2", pc == 4 );
	UNIT_ASSERT("enc2", op == OP_PUSH );
	UNIT_ASSERT("enc2", mode == MODE_IM );
	UNIT_ASSERT("enc2", argcount == 1 );

	pc = blockFetchInt32( pp->curmethod->code, pc, &val );
	UNIT_ASSERT( "enc2", pc == 6 );
	UNIT_ASSERT( "enc2", val == 1 );

	pc = blockDecode( pp->curmethod->code, pc, &op, &argcount, &mode, &mode2 );
	UNIT_ASSERT("enc2", pc == 7 );
	UNIT_ASSERT("enc2", op == OP_ADD );
	UNIT_ASSERT("enc2", mode == 0 );
	UNIT_ASSERT("enc2", argcount == 0 );

	pc = blockDecode( pp->curmethod->code, pc, &op, &argcount, &mode, &mode2 );
	UNIT_ASSERT("enc2", pc == 8 );
	UNIT_ASSERT("enc2", op == OP_RET );
	UNIT_ASSERT("enc2", argcount == 0 );

	programDestruct( pp );

	logWriteOkFail( NULL );
}

static void emitTest(  )
{
	program *pp;
	vardef *vdp;

	int pc;

	logWriteCheck(NULL, "program emit test ... " );

	pp = programConstruct();
	programDefineClass( pp, "test", "", NULL );
	programDefineMethod( pp, "call", 1, ACC_PUBLIC );
	vdp = programDefineField( pp, "field1", 1, TYPE_INT, TYPE_VOID );

	pc = programEmit1( pp, OP_PUSH, MODE_CLASS );
	UNIT_ASSERT( "emit", pc == 0 );
	pc = programEmitArg( pp, vdp->offset );
	UNIT_ASSERT( "emit", pc == 1 );

	pc = programEmit1( pp, OP_PUSH, MODE_IM );
	UNIT_ASSERT( "emit", pc == 3 );
	pc = programEmitArg( pp, 2 );
	UNIT_ASSERT( "emit", pc == 4 );

	programDestruct( pp );

	logWriteOkFail( NULL );
}

static void methodSigTest(  )
{
	string *sig;
	string *name;

	name = stringConstructChar("m");

	sig = buildMethodSig( name, 1 );
	UNIT_ASSERT("method sig", sig->cstr[0] == 'm' );
	UNIT_ASSERT("method sig", sig->cstr[1] == ':' );
	UNIT_ASSERT("method sig", sig->cstr[2] == 'S' );
	UNIT_ASSERT("method sig", sig->cstr[3] == 'v' );
	UNIT_ASSERT("method sig", sig->cstr[4] == '\0' );

	methodSigAppendArg( sig );
	UNIT_ASSERT("method sig", sig->cstr[4] == 'v' );
	UNIT_ASSERT("method sig", sig->cstr[5] == '\0' );

	stringDestruct( name );
	stringDestruct( sig );
}

void runProgramTests(  )
{
	createDestroyTest(  );
	createClassTest(  );
	createClassTestP(  );
	createMethodTest(  );
	createMethodTest2(  );
	encodeDecodeTest(  );
	encodeDecodeTest2(  );
	emitTest(  );
	methodSigTest(  );
}

#endif /*DEBUG*/

