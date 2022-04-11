#ifndef _opcodes_h
#define _opcodes_h

#ifdef _WIN32
#include <spl/configwin32.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*
 *	System
 *		CLASSES
 *			CLASS
 *				BLOCK
 *
 *	Location Format
 *
 *		<32 bit type info><1-8 bytes data>
 *
 *  Program Format
 *
 *		"JS\0"
 *		<64 bit CODE TABLE LEN>
 *		<64 bit STATIC LEN>
 *		<64 bit CONST LEN>
 *		CODE TABLE
 *		STATIC CLASS DATA
 *		CONST STORAGE
 *
 *	CODE TABLE
 *
 *		(<16 bit instruction>(<32 bit immediate arg>)-(<32 bit immediate arg>)-)*
 *
 *	Instruction Coding
 *		0-6:	OP
 *		7-8:	arg count (0-3)
 *		9-11:	Mode
 *		12-14:	Mode2
 */

enum ArgSource
{
	MODE_CLASS,	/* argument is encoded in the class (static) space */
	MODE_INST,	/* argument is encoded in the instance space */
	MODE_BLOCK,	/* local block variable */
	MODE_CONST,	/* argument is encoded in the const data space */
	MODE_IM	/* argument in the <32 bit immediate arg> */
};

enum OpCode
{
		OP_NOP,		/* .. ==> .. */
		OP_POP,		/* a .. ==> .. */
		OP_PUSH,	/* .. ==> a */
		OP_ROT,		/* a b .. ==> b a .. */
		OP_ROT3,	/* a b c .. ==> c b a */
		OP_DUP,		/* a .. ==> a a .. */

		OP_ADD,		/* a b .. ==> b+a .. */
		OP_SUB,		/* a b .. ==> b-a .. */
		OP_NEGATE,	/* a .. ==> -a .. */
		OP_MULT,	/* a b .. ==> b*a .. */
		OP_DIV,		/* a b .. ==> b/a .. */
		OP_MOD,		/* a b .. ==> b%a .. */
		OP_INC,		/* a .. ==> a+1 .. */
		OP_DEC,		/* a .. ==> a-1 .. */
		OP_PINC,	/* a .. ==> a+1 .. */
		OP_PDEC,	/* a .. ==> a-1 .. */

		OP_AND,		/* a b .. ==> b&&a .. */
		OP_BINAND,	/* a b .. ==> b&a .. */
		OP_OR,		/* a b .. ==> b||a .. */
		OP_BINOR,	/* a b .. ==> b|a .. */
		OP_XOR,		/* a b .. ==> b^a .. */
		OP_COMPL,	/* a .. ==> ~a .. */
		OP_NOT,		/* a .. ==> !a .. */

		OP_RSHIFT,	/* a b .. ==> b>>a */
		OP_LSHIFT,	/* a b .. ==> b<<a */

		OP_LT,		/* a b .. ==> b<a .. */
		OP_GT,		/* a b .. ==> b>a .. */
		OP_LTEQ,	/* a b .. ==> b<=a .. */
		OP_GTEQ,	/* a b .. ==> b>=a .. */
		OP_EQ,		/* a b .. ==> b==a .. */
		OP_NEQ,		/* a b .. ==> b!=a .. */

		OP_ASSIGN,	/* a lval .. ==> .. */
		OP_THIS,	/* .. ==> a .. */
		OP_RVALUE,	/* a .. ==> a .. */
		OP_LVALUE,	/* .. ==> a .. */
		OP_DEREF,	/* .. ==> a .. */
		OP_SDEREF, /* .. ==> a .. */
		OP_NULL,	/* .. ==> null */

		OP_ENTER,	/* .. ==> .. */
		OP_LEAVE,	/* .. ==> .. */
		OP_CALL,	/* this argn arg2 arg1 .. ==> .. */
		OP_CONSTRUCT, /* this argn arg2 arg1 .. ==> .. */
		OP_BASECALL,/* this argn arg2 arg1 .. ==> .. */
		OP_SCALL,	/* argn arg2 arg1 .. ==> .. */
		OP_VRET,	/* .. ==> .. */
		OP_RET,		/* ret .. ==> .. */
		OP_NEW,		/* .. ==> this .. */
		OP_NATIVE,	/* argn arg2 arg1 this .. ==> .. */

		OP_JMPZ,	/* bool .. ==> .. */
		OP_JMPNZ,	/* bool .. ==> .. */
		OP_JMP,		/*  .. ==> .. */
		OP_BREAK,	/*  .. ==> .. */
		OP_SETBREAK,/*  .. ==> .. */
		OP_CONTINUE, /*  .. ==> .. */

		OP_TRY,		/* .. ==> .. */
		OP_THROW,	/* a .. ==> a .. */

		OP_LINE		/* .. ==> .. */
};

#ifdef __cplusplus
}
#endif

#endif
