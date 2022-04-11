#ifdef HAVE_CONFIG_H
#include <jspp/config.h>
#endif
#include <stdio.h>
#include <spl/debug.h>
#include <jspp/program.h>

static void varPrint( FILE *fp, const int x, const variant *vp )
{
	switch( vp->type )
	{
		case TYPE_VOID:
			fprintf( fp, "%d: VOID\n", x );
			break;
		case TYPE_NULL:
			fprintf( fp, "%d: NULL\n", x );
			break;
		case TYPE_INT:
			fprintf( fp, "%d: INT %d\n", x, vp->data.i );
			break;
		case TYPE_FLOAT:
			fprintf( fp, "%d: FLOAT %f\n", x, vp->data.f );
			break;
		case TYPE_BOOL:
			fprintf( fp, "%d: BOOL %i\n", x, vp->data.i );
			break;
		case TYPE_INST:
			fprintf( fp, "%d: INST %s\n", x, vp->data.ip->myclass->name->cstr );
			break;
		case TYPE_DELEGATE:
			fprintf( fp, "%d: DELEGATE\n", x );
			break;
		case TYPE_STRING:
			fprintf( fp, "%d: STRING %s\n", x, vp->data.sp->cstr );
			break;
		default:
			fprintf(fp, "%d: UNKNOWN", x );
	}
}

static void printOpMode( FILE *fp, const enum ArgSource mode, const int arg )
{
	switch ( mode )
	{
	case MODE_CLASS:	/* argument is encoded in the class (static) space */
		fprintf( fp, " STATIC(%d)", arg );
		break;
	case MODE_INST:	/* argument is encoded in the instance space */
		fprintf( fp, " OBJ(%d)", arg );
		break;
	case MODE_BLOCK:	/* local block variable */
		fprintf( fp, " LOCAL(%d)", arg );
		break;
	case MODE_CONST:	/* argument is encoded in the const data space */
		fprintf( fp, " CONST(%d)", arg );
		break;
	case MODE_IM:	/* argument in the <32 bit immediate arg> */
		fprintf( fp, " IM(%d)", arg );
		break;
	default:
		fprintf( fp, " UNKNOWN" );
	}
}

static void printOpCode( FILE *fp, const int pc, const enum OpCode code )
{
	switch ( code )
	{
	case OP_NOP:
		/* 0 args */
		fprintf( fp, "\t\t%d: NOP", pc );
		break;

	case OP_NULL:
		/* 0 args */
		fprintf( fp, "\t\t%d: NULL", pc );
		break;

	case OP_NEW:
		/* 2 args */
		fprintf( fp, "\t\t%d: NEW", pc );		
		break;

	case OP_MULT:
		/* 2 args */
		fprintf( fp, "\t\t%d: MUL", pc );
		break;

	case OP_DIV:
		/* 2 args */
		fprintf( fp, "\t\t%d: DIV", pc );
		break;

	case OP_ADD:
		/* 2 args */
		fprintf( fp, "\t\t%d: ADD", pc );
		break;

	case OP_SUB:
		/* 2 args */
		fprintf( fp, "\t\t%d: SUB", pc );
		break;

	case OP_NEGATE:
		/* 1 args */
		fprintf( fp, "\t\t%d: NEG", pc );
		break;

	case OP_VRET:
		/* 0 args */
		fprintf( fp, "\t\t%d: VRET", pc );
		break;

	case OP_RET:
		/* 0 args */
		fprintf( fp, "\t\t%d: RET", pc );
		break;

	case OP_TRY:
		/* 1 args */
		fprintf( fp, "\t\t%d: TRY", pc );
		break;

	case OP_THROW:
		/* 0 args */
		fprintf( fp, "\t\t%d: THROW", pc );
		break;

	case OP_ENTER:
		/* 1 args */
		fprintf( fp, "\t\t%d: ENTER", pc );
		break;
	
	case OP_LEAVE:
		/* 0 args */
		fprintf( fp, "\t\t%d: LEAVE", pc );
		break;

	case OP_SETBREAK:
		/* 1 args */
		fprintf( fp, "\t\t%d: SETBREAK", pc );
		break;

	case OP_BREAK:
		/* 0 args */
		fprintf( fp, "\t\t%d: BREAK", pc );
		break;

	case OP_CONTINUE:
		/* 0 args */
		fprintf( fp, "\t\t%d: CONTINUE", pc );
		break;

	case OP_PUSH:
		/* 1 args */
		fprintf( fp, "\t\t%d: PUSH", pc );
		break;

	case OP_DUP:
		/* 0 args */
		fprintf( fp, "\t\t%d: DUP", pc );
		break;

	case OP_ASSIGN:
		/* 0 args */
		fprintf( fp, "\t\t%d: ASSIGN", pc );
		break;

	case OP_THIS:
		/* 0 args */
		fprintf( fp, "\t\t%d: THIS", pc );
		break;

	case OP_LVALUE:
		/* 1 args */
		fprintf( fp, "\t\t%d: LVALUE", pc );
		break;

	case OP_RVALUE:
		/* 0 args */
		fprintf( fp, "\t\t%d: RVALUE", pc );
		break;

	case OP_SDEREF:
		/* 2 args */
		fprintf( fp, "\t\t%d: SDEREF", pc );
		break;

	case OP_DEREF:
		/* 1 args */
		fprintf( fp, "\t\t%d: DEREF", pc );
		break;

	case OP_POP:
		/* 0 args */
		fprintf( fp, "\t\t%d: POP", pc );
		break;

	case OP_ROT:
		/* 0 args */
		fprintf( fp, "\t\t%d: ROT", pc );
		break;

	case OP_ROT3:
		/* 0 args */
		fprintf( fp, "\t\t%d: ROT3", pc );
		break;

	case OP_BASECALL:
		/* 2 args */
		fprintf( fp, "\t\t%d: BASECALL", pc );
		break;

	case OP_CALL:
		/* 2 args */
		fprintf( fp, "\t\t%d: CALL", pc );
		break;

	case OP_SCALL:
		/* 1 args */
		fprintf( fp, "\t\t%d: SCALL", pc );
		break;

	case OP_CONSTRUCT:
		fprintf( fp, "\t\t%d: CONSTRUCT", pc );
		break;

	case OP_JMPZ:
		/* 1 args */
		fprintf( fp, "\t\t%d: JMPZ", pc );
		break;

	case OP_JMPNZ:
		/* 1 args */
		fprintf( fp, "\t\t%d: JMPNZ", pc );
		break;

	case OP_JMP:
		/* 1 args */
		fprintf( fp, "\t\t%d: JMP", pc );
		break;

	case OP_LT:
		/* 0 args */
		fprintf( fp, "\t\t%d: LT", pc );
		break;

	case OP_LTEQ:
		/* 0 args */
		fprintf( fp, "\t\t%d: LTEQ", pc );
		break;

	case OP_EQ:
		/* 0 args */
		fprintf( fp, "\t\t%d: EQ", pc );
		break;

	case OP_GT:
		/* 0 args */
		fprintf( fp, "\t\t%d: GT", pc );
		break;

	case OP_GTEQ:
		/* 0 args */
		fprintf( fp, "\t\t%d: GTEQ", pc );
		break;

	case OP_NEQ:
		/* 0 args */
		fprintf( fp, "\t\t%d: NEQ", pc );
		break;

	case OP_MOD:
		/* 0 args */
		fprintf( fp, "\t\t%d: MOD", pc );
		break;

	case OP_BINOR:
		/* 0 args */
		fprintf( fp, "\t\t%d: BOR", pc );
		break;

	case OP_BINAND:
		/* 0 args */
		fprintf( fp, "\t\t%d: BAND", pc );
		break;

	case OP_XOR:
		/* 0 args */
		fprintf( fp, "\t\t%d: XOR", pc );
		break;

	case OP_COMPL:
		fprintf( fp, "\t\t%d: COMP", pc );
		break;

	case OP_NOT:
		fprintf( fp, "\t\t%d: NOT", pc );
		break;

	case OP_PINC:
		fprintf( fp, "\t\t%d: PREINC", pc );
		break;

	case OP_PDEC:
		fprintf( fp, "\t\t%d: PREDEC", pc );
		break;

	case OP_INC:
		fprintf( fp, "\t\t%d: INC", pc );
		break;

	case OP_DEC:
		fprintf( fp, "\t\t%d: DEC", pc );
		break;

	case OP_LINE:
		fprintf( fp, "\t\t%d: LINE", pc );
		break;

	default:
		fprintf( fp, "\t\t%d: UNKNOWN", pc );
	}
}

static bool jsclassDump( const jsclass *jcp, FILE *fp )
{
	int x, count;

	jsclassValidateMem( jcp );
	if ( NULL != jcp->baseclass )
	{
		jsclassDump( jcp->baseclass, fp );
	}

	fprintf( fp, "class %s : %s \n{\n", jcp->name->cstr, (NULL == jcp->baseclass) ? "Object" : jcp->baseclass->name->cstr );

	count = vectorCount( jcp->fields );
	for ( x = 0; x < count; x++ )
	{
		vardef *vdp = (vardef *)vectorElementAt( jcp->fields, x );
		fprintf( fp, "\t%svar %s%s%s;\t//%d\n", (vdp->isstatic) ? "static " : "", (NULL == vdp->nativefield) ? "" : "native " , (vdp->isptr) ? "*" : "", vdp->name->cstr, vdp->offset );
	}
	fprintf( fp, "\n" );
	count = vectorCount( jcp->methods );
	for ( x = 0; x < count; x++ )
	{
		int y;
		jsmethod *jsm = (jsmethod *)vectorElementAt( jcp->methods, x );
		fprintf( fp, "\t%s ", (jsm->access == ACC_PRIVATE) ? "private" : ((jsm->access == ACC_PUBLIC) ? "public" : "protected") );
		if ( jsm->isnative )
		{
			fprintf( fp, " native" );
		}
		fprintf( fp, " %s(", jsm->name->cstr );
		for ( y = 0; y < vectorCount( jsm->args ); y++ )
		{
			vardef *vdp = (vardef *)vectorElementAt( jsm->args, y );
			fprintf( fp, "%svar %s", (y > 0) ? ", " : "", vdp->name->cstr );
		}
		fprintf( fp, ")\n\t{\n" );

		if ( ! jsm->isnative )
		{
			int pc = 0;
			enum OpCode op;
			enum ArgSource mode;
			enum ArgSource mode2;
			int16 argcount;
			int32 arg;

			while ( pc < jsmethodCodeLen( jsm ) )
			{
				int oldpc = pc;
				pc = blockDecode( jsm->code, pc, &op, &argcount, &mode, &mode2 );
				printOpCode( fp, oldpc, op );
				if ( argcount > 0 )
				{
					pc = blockFetchInt32( jsm->code, pc, &arg );
					printOpMode( fp, mode, arg );
				}
				if ( argcount > 1 )
				{
					pc = blockFetchInt32( jsm->code, pc, &arg );
					printOpMode( fp, mode2, arg );
				}
				fprintf( fp, "\n" );
			}
		}
		fprintf( fp, "\t}\n" );
	}

	fprintf( fp, "} //%s\n\n", jcp->name->cstr );
	
	return true;
}

bool programDump( const program *pp, const char *filename )
{
	int x, count;
	FILE *fp;

	programValidateMem( pp );

	fp = fopen( filename, "w" );
	if ( NULL == fp )
	{
		return false;
	}
	fprintf( fp, "//\n// BEGIN CONST DATA\n//\nconst {\n" );
	for ( x = 0; x < vectorCount( pp->cdata ); x++ )
	{
		variant *vp = (variant *)vectorDataElementAt( pp->cdata, x );
		varValidateContentMem( vp );
		varPrint( fp, x, vp );
	}
	fprintf( fp, "}\n//\n// END CONST DATA\n//\n\n" );

	count = vectorCount( pp->lclasses );
	for ( x = 0; x < count; x++ )
	{
		jsclass *jcp = (jsclass *)vectorElementAt( pp->lclasses, x );
		jsclassValidateMem( jcp );

		if ( ! jsclassDump( jcp, fp ) )
		{
			fclose( fp );
			return false;
		}
	}

	fclose( fp );
	return true;
}
