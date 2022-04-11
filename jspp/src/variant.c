#ifdef HAVE_CONFIG_H
#include <jspp/config.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <spl/debug.h>
#include <jspp/jsclass.h>
#include <jspp/variant.h>

static void normalize1(const variant **arg1)
{
	ASSERT_PTR( *arg1 );
	if ( TYPE_PTR == (*arg1)->type )
	{
		*arg1 = (*arg1)->data.vp;
		varValidateMem( *arg1 );
	}
	ASSERT( TYPE_PTR != (*arg1)->type );
}

#define normalize(arg1,arg2) if(TYPE_PTR == (*arg1)->type) normalize1( arg1 ); if(TYPE_PTR == (*arg2)->type) normalize1( arg2 );

bool varIsFloat( const variant *var )
{
	switch( var->type )
	{
	case TYPE_PTR:
		return varIsFloat( var->data.vp );
	case TYPE_VOID:
	case TYPE_NULL:
	case TYPE_INT:
	case TYPE_CHAR:
	case TYPE_BOOL:
	case TYPE_INST:
	case TYPE_DELEGATE:
	case TYPE_STRING:
		return false;
	case TYPE_FLOAT:
		return true;
	}
	return false;
}

bool varIsNumeric( const variant *var )
{
	switch( var->type )
	{
	case TYPE_PTR:
		return varIsNumeric( var->data.vp );
	case TYPE_VOID:
	case TYPE_NULL:
	case TYPE_INT:
	case TYPE_FLOAT:
	case TYPE_CHAR:
	case TYPE_BOOL:
		return true;
	case TYPE_INST:
	case TYPE_DELEGATE:
	case TYPE_STRING:
		break;
	}
	return false;
}

variant *varConstruct()
{
	variant *var = (variant *)malloc( sizeof(struct variant) );
	if ( NULL == var )
	{
		return NULL;
	}
	var->type = TYPE_VOID;
	var->data.i = 0;
	return var;
}

variant *varConstructDup( const variant *from )
{
	variant *var = varConstruct();
	if ( NULL == var )
	{
		return NULL;
	}
	if ( ! varAssign( var, from ) )
	{
		varDestruct( var );
		return NULL;
	}
	return var;
}

void varDestruct( variant *var )
{
	varValidateMem( var );
	VARCLEAR( var );
	free( var );
}

bool varCast( variant *var, const enum jstype type )
{
	varValidateContentMem( var );

	if ( TYPE_PTR == var->type )
	{
		if ( TYPE_NULL == type )
		{
			varDeref( var );
			var->type = TYPE_NULL;
			return true;
		}
		return varCast( var->data.vp, type );
	}

	switch ( type )
	{
	case TYPE_VOID:
		VARCLEAR( var );
		return true;

	case TYPE_NULL:
		VARCLEAR( var );
		var->type = TYPE_NULL;
		return true;

	case TYPE_CHAR:
		switch ( var->type )
		{
		case TYPE_VOID:
			var->type = TYPE_CHAR;
			var->data.i = 0;
			return true;
		case TYPE_INT:
			var->type = TYPE_CHAR;
			var->data.i = (char)var->data.i;
			return true;
		case TYPE_FLOAT:
			var->type = TYPE_CHAR;
			var->data.i = (char)var->data.f;
			return true;
		case TYPE_BOOL:
			var->type = TYPE_CHAR;
			return true;
		case TYPE_INST:
			return false;
		case TYPE_DELEGATE:
			return false;
		case TYPE_STRING:
			if ( stringIsint( var->data.sp ) )
			{
				int i = atoi( var->data.sp->cstr );
				stringDestruct( var->data.sp );
				var->type = TYPE_CHAR;
				var->data.i = (char)i;
				return true;
			}
		}
		break;

	case TYPE_INT:
		switch ( var->type )
		{
		case TYPE_VOID:
			var->type = TYPE_INT;
			var->data.i = 0;
			return true;
		case TYPE_CHAR:
			var->type = TYPE_INT;
			return true;
		case TYPE_INT:
			return true;
		case TYPE_FLOAT:
			var->type = TYPE_INT;
			var->data.i = (int32)var->data.f;
			return true;
		case TYPE_BOOL:
			var->type = TYPE_INT;
			return true;
		case TYPE_INST:
			return false;
		case TYPE_DELEGATE:
			return false;
		case TYPE_STRING:
			if ( stringIsint( var->data.sp ) )
			{
				int i = atoi( var->data.sp->cstr );
				stringDestruct( var->data.sp );
				var->type = TYPE_INT;
				var->data.i = i;
				return true;
			}
			if ( stringIsfloat( var->data.sp ) )
			{
				float i = (float)atof( var->data.sp->cstr );
				stringDestruct( var->data.sp );
				var->type = TYPE_INT;
				var->data.i = (int)i;
				return true;
			}
		}
		break;

	case TYPE_FLOAT:
		switch ( var->type )
		{
		case TYPE_VOID:
			var->type = TYPE_FLOAT;
			var->data.f = 0;
			return true;
		case TYPE_CHAR:
			var->type = TYPE_FLOAT;
			var->data.f = (float)var->data.i;
		case TYPE_INT:
			var->type = TYPE_FLOAT;
			var->data.f = (float)var->data.i;
		case TYPE_FLOAT:
			return true;
		case TYPE_BOOL:
			var->type = TYPE_FLOAT;
			var->data.f = (float)var->data.i;
			return true;
		case TYPE_INST:
			return false;
		case TYPE_DELEGATE:
			return false;
		case TYPE_STRING:
			if ( stringIsfloat( var->data.sp ) )
			{
				float i = (float)atof( var->data.sp->cstr );
				stringDestruct( var->data.sp );
				var->type = TYPE_FLOAT;
				var->data.f = i;
				return true;
			}
			if ( stringIsint( var->data.sp ) )
			{
				int i = atoi( var->data.sp->cstr );
				stringDestruct( var->data.sp );
				var->type = TYPE_FLOAT;
				var->data.f = (float)i;
				return true;
			}
		}
		break;

	case TYPE_BOOL:
		switch ( var->type )
		{
		case TYPE_VOID:
			var->type = TYPE_BOOL;
			var->data.i = 0;
			return true;
		case TYPE_CHAR:
			var->type = TYPE_BOOL;
			var->data.i = (var->data.i == 0) ? false : true;
			return true;
		case TYPE_INT:
			var->type = TYPE_BOOL;
			var->data.i = (var->data.i == 0) ? false : true;
			return true;
		case TYPE_FLOAT:
			var->type = TYPE_BOOL;
			var->data.i = (var->data.f == 0) ? false : true;
			return true;
		case TYPE_BOOL:
			return true;
		case TYPE_INST:
			return false;
		case TYPE_DELEGATE:
			return false;
		case TYPE_STRING:
			if ( stringCmpChar(var->data.sp, "true") == 0 || stringCmpChar(var->data.sp, "TRUE") == 0 )
			{
				stringDestruct( var->data.sp );
				var->data.i = true;
				return true;
			}
			if ( stringCmpChar(var->data.sp, "false") == 0 || stringCmpChar(var->data.sp, "FALSE") == 0 )
			{
				stringDestruct( var->data.sp );
				var->data.i = false;
				return true;
			}
			return false;
		}
		break;
	case TYPE_INST:
		switch ( var->type )
		{
		case TYPE_VOID:
			return false;
		case TYPE_NULL:
			VARCLEAR( var );
			var->type = TYPE_NULL;
			return true;
		case TYPE_CHAR:
			return false;
		case TYPE_INT:
			return false;
		case TYPE_FLOAT:
			return false;
		case TYPE_BOOL:
			return false;
		case TYPE_INST:
			return true;
		case TYPE_DELEGATE:
			return false;
		case TYPE_STRING:
			return false;
		}
		break;

	case TYPE_DELEGATE:
		switch ( var->type )
		{
		case TYPE_VOID:
			return false;
		case TYPE_NULL:
			VARCLEAR( var );
			var->type = TYPE_NULL;
			return true;
		case TYPE_INT:
		case TYPE_CHAR:
		case TYPE_FLOAT:
		case TYPE_BOOL:
			return false;
		case TYPE_INST:
			return false;
		case TYPE_DELEGATE:
			return false;
		case TYPE_STRING:
			return false;
		}
		break;

	case TYPE_STRING:
		{
			string *str = varToString( var );
			VARCLEAR( var );
			var->type = TYPE_STRING;
			var->data.sp = str;
			return true;
		}
	case TYPE_PTR:
		return false;
	default:
		return false;
	}
	return false;
}

void varClear( variant *var )
{
	varValidateContentMem( var );

	switch ( var->type )
	{
	case TYPE_VOID:
	case TYPE_NULL:
		return;
	case TYPE_CHAR:
	case TYPE_INT:
	case TYPE_FLOAT:
	case TYPE_BOOL:
		break;
	case TYPE_INST:
		jsinstDeref( var->data.ip );
		break;
	case TYPE_DELEGATE:
		/* delete the delegate */
		break;
	case TYPE_STRING:
		stringDestruct( var->data.sp );
		break;
	case TYPE_PTR:
		break;
	default:
		ASSERT(0);
	}
	var->type = TYPE_VOID;
	var->data.i = 0;
}

variant *varDup( const variant *var )
{
	variant *var2;
	
	varValidateMem( var );
	ASSERT( var->type != TYPE_PTR );

	var2 = varConstruct();
	if ( NULL == var2 )
	{
		return NULL;
	}
	var2->type = var->type;
	switch ( var->type )
	{
	case TYPE_STRING:
		var2->data.sp = stringConstructDup( var->data.sp );
		if ( NULL == var2->data.sp )
		{
			free( var2 );
			return NULL;
		}
		break;
	default:
		var2->data.i = var->data.i;
		break;
	}
	return var2;
}

bool varAssign( variant *var, const variant *from )
{
	ASSERT_PTR( var );
	ASSERT_PTR( from );

	if ( NULL == var || NULL == from )
	{
		return false;
	}
	VARCLEAR( var );
	var->type = from->type;

	switch ( from->type )
	{
	case TYPE_VOID:
		return true;

	case TYPE_CHAR:
	case TYPE_BOOL:
	case TYPE_INT:
	case TYPE_NULL:
		var->data.i = from->data.i;
		return true;

	case TYPE_FLOAT:
		var->data.f = from->data.f;
		return true;

	case TYPE_INST:
		var->data.ip = from->data.ip;
		jsinstRef( var->data.ip );
		return true;

	case TYPE_DELEGATE:
		break;

	case TYPE_STRING:
		if ( NULL == (var->data.sp = stringConstructDup( from->data.sp )) )
		{
			return false;
		}
		return true;

	case TYPE_PTR:
		var->data.vp = from->data.vp;
		return true;
	}
	return false;
}

bool varRef( variant *var, variant *ptrto )
{
	ASSERT_PTR( var );
	varValidateMem( ptrto );
	VARCLEAR( var );
	var->type = TYPE_PTR;
	var->data.vp = ptrto;
	return true;
}

void varDeref( variant *var )
{
	ASSERT_PTR( var );
	varValidateContentMem( var );
	if ( TYPE_PTR == var->type )
	{
		variant *vp = var->data.vp;
		ASSERT_PTR( vp );
		VARCLEAR( var );
		varAssign( var, vp );
	}
	ASSERT( TYPE_PTR != var->type );
}

bool varAssignInt( variant *var, const int32 i )
{
	VARCLEAR( var );
	var->type = TYPE_INT;
	var->data.i = i;

	return true;
}

bool varAssignFloat( variant *var, const float i )
{
	VARCLEAR( var );
	var->type = TYPE_FLOAT;
	var->data.f = i;
	
	return true;
}

bool varAssignBool( variant *var, const bool i )
{
	VARCLEAR( var );
	var->type = TYPE_BOOL;
	var->data.i = i;

	return true;
}

bool varAssignCh( variant *var, const int ch )
{
	VARCLEAR( var );
	var->type = TYPE_CHAR;
	var->data.i = ch;
	return true;
}

bool varAssignChar( variant *var, const char *cp )
{
	VARCLEAR( var );
	var->data.sp = stringConstructChar( cp );

	/* bad assert, hard to eject this deep.  need to use setjmp/longjmp */
	if( NULL == var->data.sp )
	{
		return false;
	}
	var->type = TYPE_STRING;
	return true;
}

bool varAssignInst( variant *var, jsinst *inst )
{
	if ( NULL == inst )
	{
		return false;
	}
	ASSERT_MEM( inst, sizeof( jsinst ) );
	VARCLEAR( var );
	var->type = TYPE_INST;
	var->data.ip = inst;
	jsinstRef( inst );
	return true;
}

/*
 *  Takes ownership of str's memory
 */
bool varAssignString( variant *var, string *str )
{
	varValidateMem( var );
	stringValidateMem( str );
	VARCLEAR( var );
	var->type = TYPE_STRING;
	var->data.sp = str;
	return true;
}

bool varAssignOpEQ( variant *dest, const variant *arg1, const variant *arg2 )
{
	VARCLEAR( dest );
	normalize( &arg1, &arg2 );

	if ( varIsNumeric( arg1 ) || varIsNumeric( arg2 ) )
	{
		if( varIsFloat( arg1 ) || varIsFloat( arg2 ) )
		{
			float a, b;
			if ( ! varGetFloat( arg1, &a ) )
			{
				return false;
			}
			if ( ! varGetFloat( arg2, &b ) )
			{
				return false;
			}
			dest->type = TYPE_BOOL;
			dest->data.i = a == b;
			return true;
		}
		else
		{
			int a, b;
			if ( ! varGetInt( arg1, &a ) )
			{
				return false;
			}
			if ( ! varGetInt( arg2, &b ) )
			{
				return false;
			}
			dest->type = TYPE_BOOL;
			dest->data.i = a == b;
			return true;
		}
	}
	if ( TYPE_STRING == arg1->type && TYPE_STRING == arg2->type )
	{
		dest->type = TYPE_BOOL;
		dest->data.i = stringCmpStr( arg1->data.sp, arg2->data.sp) == 0;
		return true;
	}
	return false;
}

bool varAssignOpNEQ( variant *dest, const variant *arg1, const variant *arg2 )
{
	VARCLEAR( dest );
	normalize( &arg1, &arg2 );

	if ( varIsNumeric( arg1 ) || varIsNumeric( arg2 ) )
	{
		if( varIsFloat( arg1 ) || varIsFloat( arg2 ) )
		{
			float a, b;
			if ( ! varGetFloat( arg1, &a ) )
			{
				return false;
			}
			if ( ! varGetFloat( arg2, &b ) )
			{
				return false;
			}
			dest->type = TYPE_BOOL;
			dest->data.i = a != b;
			return true;
		}
		else
		{
			int a, b;
			if ( ! varGetInt( arg1, &a ) )
			{
				return false;
			}
			if ( ! varGetInt( arg2, &b ) )
			{
				return false;
			}
			dest->type = TYPE_BOOL;
			dest->data.i = a != b;
			return true;
		}
	}
	if ( TYPE_STRING == arg1->type && TYPE_STRING == arg2->type )
	{
		dest->type = TYPE_BOOL;
		dest->data.i = stringCmpStr( arg1->data.sp, arg2->data.sp) != 0;
		return true;
	}
	return false;}

bool varAssignOpLT( variant *dest, const variant *arg1, const variant *arg2 )
{
	VARCLEAR( dest );
	normalize( &arg1, &arg2 );

	if ( varIsFloat( arg1 ) || varIsFloat( arg2 ) )
	{
		float a, b;
		varGetFloat( arg1, &a );
		varGetFloat( arg2, &b );
		dest->type = TYPE_BOOL;
		dest->data.i = a < b;
		return true;
	}
	if ( varIsNumeric( arg1 ) && varIsNumeric( arg2 ) )
	{
		int a, b;
		varGetInt( arg1, &a );
		varGetInt( arg2, &b );
		dest->type = TYPE_BOOL;
		dest->data.i = a < b;
		return true;
	}
	return false;
}

bool varAssignOpLE( variant *dest, const variant *arg1, const variant *arg2 )
{
	VARCLEAR( dest );
	normalize( &arg1, &arg2 );

	if ( varIsFloat( arg1 ) || varIsFloat( arg2 ) )
	{
		float a, b;
		varGetFloat( arg1, &a );
		varGetFloat( arg2, &b );
		dest->type = TYPE_BOOL;
		dest->data.i = a <= b;
		return true;
	}
	if ( varIsNumeric( arg1 ) && varIsNumeric( arg2 ) )
	{
		int a, b;
		varGetInt( arg1, &a );
		varGetInt( arg2, &b );
		dest->type = TYPE_BOOL;
		dest->data.i = a <= b;
		return true;
	}
	return false;
}

bool varAssignOpGT( variant *dest, const variant *arg1, const variant *arg2 )
{
	VARCLEAR( dest );
	normalize( &arg1, &arg2 );

	if ( varIsFloat( arg1 ) || varIsFloat( arg2 ) )
	{
		float a, b;
		varGetFloat( arg1, &a );
		varGetFloat( arg2, &b );
		dest->type = TYPE_BOOL;
		dest->data.i = a > b;
		return true;
	}
	if ( varIsNumeric( arg1 ) && varIsNumeric( arg2 ) )
	{
		int a, b;
		varGetInt( arg1, &a );
		varGetInt( arg2, &b );
		dest->type = TYPE_BOOL;
		dest->data.i = a > b;
		return true;
	}
	return false;
}

bool varAssignOpGE( variant *dest, const variant *arg1, const variant *arg2 )
{
	VARCLEAR( dest );
	normalize( &arg1, &arg2 );

	if ( varIsFloat( arg1 ) || varIsFloat( arg2 ) )
	{
		float a, b;
		varGetFloat( arg1, &a );
		varGetFloat( arg2, &b );
		dest->type = TYPE_BOOL;
		dest->data.i = a >= b;
		return true;
	}
	if ( varIsNumeric( arg1 ) && varIsNumeric( arg2 ) )
	{
		int a, b;
		varGetInt( arg1, &a );
		varGetInt( arg2, &b );
		dest->type = TYPE_BOOL;
		dest->data.i = a >= b;
		return true;
	}
	return false;}

/*
 *  If all of the arguments are numeric, they are casted to the LCD type 
 *	in the order (bool -> int -> float).  If any of the arguments are strings,
 *  a CAT is performed.  Arrays cause an error.
 */
bool varAssignOpPlus( variant *dest, const variant *arg1, const variant *arg2 )
{
	VARCLEAR( dest );
	normalize( &arg1, &arg2 );

	if ( TYPE_STRING == arg1->type || TYPE_STRING == arg2->type )
	{
		string *str;

		/* cat the strings */
		str = varToString( arg1 );
		varAssignString( dest, str );
		str = varToString( arg2 );
		stringAppend( dest->data.sp, str );
		stringDestruct( str );
		return true;
	}
	if ( varIsFloat( arg1 ) || varIsFloat( arg2 ) )
	{
		float a, b;
		varGetFloat( arg1, &a );
		varGetFloat( arg2, &b );
		dest->type = TYPE_FLOAT;
		dest->data.f = a + b;
		return true;
	}
	if ( varIsNumeric( arg1 ) && varIsNumeric( arg2 ) )
	{
		int a, b;
		varGetInt( arg1, &a );
		varGetInt( arg2, &b );
		dest->type = TYPE_INT;
		dest->data.i = a + b;
		return true;
	}
	return false;
}

bool varAssignOpSub( variant *dest, const variant *arg1, const variant *arg2 )
{
	VARCLEAR( dest );
	normalize( &arg1, &arg2 );

	if ( varIsFloat( arg1 ) || varIsFloat( arg2 ) )
	{
		float a, b;
		varGetFloat( arg1, &a );
		varGetFloat( arg2, &b );
		dest->type = TYPE_FLOAT;
		dest->data.f = a - b;
		return true;
	}
	if ( varIsNumeric( arg1 ) && varIsNumeric( arg2 ) )
	{
		int a, b;
		varGetInt( arg1, &a );
		varGetInt( arg2, &b );
		dest->type = TYPE_INT;
		dest->data.i = a - b;
		return true;
	}
	return false;
}

bool varAssignOpMul( variant *dest, const variant *arg1, const variant *arg2 )
{
	VARCLEAR( dest );
	normalize( &arg1, &arg2 );

	if ( varIsFloat( arg1 ) || varIsFloat( arg2 ) )
	{
		float a, b;
		varGetFloat( arg1, &a );
		varGetFloat( arg2, &b );
		dest->type = TYPE_FLOAT;
		dest->data.f = a * b;
		return true;
	}
	if ( varIsNumeric( arg1 ) && varIsNumeric( arg2 ) )
	{
		int a, b;
		varGetInt( arg1, &a );
		varGetInt( arg2, &b );
		dest->type = TYPE_INT;
		dest->data.i = a * b;
		return true;
	}
	return false;
}

bool varAssignOpRightShift( variant *dest, const variant *arg1, const variant *arg2 )
{
	int iarg1, iarg2;

	VARCLEAR( dest );
	normalize( &arg1, &arg2 );

	if ( ! varGetInt(arg1, &iarg1 ) )
	{
		return false;
	}
	if ( ! varGetInt(arg1, &iarg2 ) )
	{
		return false;
	}
	dest->type = TYPE_INT;
	dest->data.i = iarg1 >> iarg2;
	return true;
}

bool varAssignOpLeftShift( variant *dest, const variant *arg1, const variant *arg2 )
{
	int iarg1, iarg2;

	VARCLEAR( dest );
	normalize( &arg1, &arg2 );

	if ( ! varGetInt(arg1, &iarg1 ) )
	{
		return false;
	}
	if ( ! varGetInt(arg1, &iarg2 ) )
	{
		return false;
	}
	dest->type = TYPE_INT;
	dest->data.i = iarg1 << iarg2;
	return true;
}

bool varAssignOpDiv( variant *dest, const variant *arg1, const variant *arg2 )
{
	VARCLEAR( dest );
	normalize( &arg1, &arg2 );

	if ( varIsFloat( arg1 ) || varIsFloat( arg2 ) )
	{
		float a, b;
		varGetFloat( arg1, &a );
		varGetFloat( arg2, &b );
		dest->type = TYPE_FLOAT;
		dest->data.f = a / b;
		return true;
	}
	if ( varIsNumeric( arg1 ) && varIsNumeric( arg2 ) )
	{
		int a, b;
		varGetInt( arg1, &a );
		varGetInt( arg2, &b );
		dest->type = TYPE_INT;
		dest->data.i = a / b;
		return true;
	}
	return false;
}

bool varAssignOpMod( variant *dest, const variant *arg1, const variant *arg2 )
{
	int iarg1, iarg2;

	VARCLEAR( dest );
	normalize( &arg1, &arg2 );

	if ( ! varGetInt(arg1, &iarg1 ) )
	{
		return false;
	}
	if ( ! varGetInt(arg2, &iarg2 ) )
	{
		return false;
	}
	dest->type = TYPE_INT;
	dest->data.i = iarg1 % iarg2;
	return true;
}

bool varAssignOpBinOr( variant *dest, const variant *arg1, const variant *arg2 )
{
	int iarg1, iarg2;

	VARCLEAR( dest );
	normalize( &arg1, &arg2 );

	if ( ! varGetInt(arg1, &iarg1 ) )
	{
		return false;
	}
	if ( ! varGetInt(arg2, &iarg2 ) )
	{
		return false;
	}
	dest->type = TYPE_INT;
	dest->data.i = iarg1 | iarg2;
	return true;
}

bool varAssignOpBinAnd( variant *dest, const variant *arg1, const variant *arg2 )
{
	int iarg1, iarg2;

	VARCLEAR( dest );
	normalize( &arg1, &arg2 );

	if ( ! varGetInt(arg1, &iarg1 ) )
	{
		return false;
	}
	if ( ! varGetInt(arg2, &iarg2 ) )
	{
		return false;
	}
	dest->type = TYPE_INT;
	dest->data.i = iarg1 & iarg2;
	return true;
}

bool varAssignOpBinXOr( variant *dest, const variant *arg1, const variant *arg2 )
{
	int iarg1, iarg2;

	VARCLEAR( dest );
	normalize( &arg1, &arg2 );

	if ( ! varGetInt(arg1, &iarg1 ) )
	{
		return false;
	}
	if ( ! varGetInt(arg2, &iarg2 ) )
	{
		return false;
	}
	dest->type = TYPE_INT;
	dest->data.i = iarg1 ^ iarg2;
	return true;
}

bool varComp( variant *arg1 )
{
	int iarg1;

	normalize1( (const variant **)&arg1 );
	if ( ! varGetInt( arg1, &iarg1 ) )
	{
		return false;
	}
	arg1->type = TYPE_INT;
	arg1->data.i = ~iarg1;
	return true;
}

bool varLogNot( variant *arg1 )
{
	int iarg1;

	normalize1( (const variant **)&arg1 );
	if ( ! varGetInt( arg1, &iarg1 ) )
	{
		return false;
	}
	arg1->type = TYPE_BOOL;
	arg1->data.i = !iarg1;
	return true;
}

bool varNegate( variant *arg1 )
{
	int iarg1;

	normalize1( (const variant **)&arg1 );
	if ( ! varGetInt( arg1, &iarg1 ) )
	{
		return false;
	}
	arg1->type = TYPE_INT;
	arg1->data.i = -iarg1;
	return true;}

string *varToString( const variant *var )
{
	switch ( var->type )
	{
	case TYPE_VOID:
		return stringConstructChar( "*unassigned*" );	
	case TYPE_NULL:
		return stringConstructChar( "<null>" );
	case TYPE_INT:
		{
			char buf[80];
			itoa( var->data.i, buf, 10 );
			return stringConstructChar( buf );
		}
	case TYPE_FLOAT:
		{
			char buf[80];
			sprintf( buf, "%f", var->data.f );
			return stringConstructChar( buf );
		}
	case TYPE_CHAR:
		{
			char buf[2];
			buf[0] = (char)var->data.i;
			buf[1] = '\0';
			return stringConstructChar( buf );
		}
		break;
	case TYPE_BOOL:
		return stringConstructChar( (var->data.i) ? "true" : "false" );
	case TYPE_INST:
		/* should call object::toString(); */
		return stringConstructChar( "<instance>" );
	case TYPE_DELEGATE:
		return stringConstructChar( "<delegate>" );
	case TYPE_STRING:
		return stringConstructDup( var->data.sp );
	case TYPE_PTR:
		return varToString( var->data.vp );
	}	
	ASSERT(0);
	return NULL;
}

string *varTypeName( const variant *var )
{
	string *str;
	char *msg;

	switch ( var->type )
	{
	case TYPE_VOID:
		msg = "void";
		break;
	case TYPE_NULL:
		msg = "null";
		break;
	case TYPE_INT:
		msg = "int";
		break;
	case TYPE_FLOAT:
		msg = "float";
		break;
	case TYPE_CHAR:
		msg = "char";
		break;
	case TYPE_BOOL:
		msg = "bool";
		break;
	case TYPE_INST:
		msg = var->data.ip->myclass->name->cstr;
		break;
	case TYPE_DELEGATE:
		msg = "delegate";
		break;
	case TYPE_STRING:
		msg = "string";
		break;
	case TYPE_PTR:
		{
			string *strptr = varTypeName( var->data.vp );
			if ( NULL == strptr )
			{
				return NULL;
			}
			str = stringConstructChar( "*" );
			if ( NULL == str )
			{
				stringDestruct (strptr);
				return NULL;
			}
			if ( ! stringAppend( str, strptr ) )
			{
				stringDestruct( strptr );
				stringDestruct( str );
				return NULL;
			}
			stringDestruct( strptr );
			return str;
		}
	}	
	str = stringConstructChar( msg );
	return str;
}

bool varGetInt( const variant *var, int *ip )
{
	switch ( var->type )
	{
	case TYPE_PTR:
		return varGetInt( var->data.vp, ip );
	case TYPE_VOID:
	case TYPE_NULL:
		*ip = 0;
		return true;	
	case TYPE_INT:
	case TYPE_CHAR:
	case TYPE_BOOL:
		*ip = var->data.i;
		return true;
	case TYPE_FLOAT:
		*ip = (int)var->data.f;
		return true;
	case TYPE_INST:
	case TYPE_DELEGATE:
	case TYPE_STRING:
		return false;
	}
	ASSERT(0);
	return false;
}

bool varGetFloat( const variant *var, float *fp )
{
	switch ( var->type )
	{
	case TYPE_PTR:
		return varGetFloat( var->data.vp, fp );
	case TYPE_VOID:
	case TYPE_NULL:
		*fp = 0;
		return true;	
	case TYPE_INT:
	case TYPE_CHAR:
	case TYPE_BOOL:
		*fp = (float)var->data.i;
		return true;
	case TYPE_FLOAT:
		*fp = (float)var->data.f;
		return true;
	case TYPE_INST:
	case TYPE_DELEGATE:
	case TYPE_STRING:
		return false;
	}
	ASSERT(0);
	return false;
}

#ifdef DEBUG

void varValidateContentMem( const variant *var )
{
	ASSERT( var->type <= TYPE_END_OF_TYPES );

	switch ( var->type )
	{
	case TYPE_STRING:
		stringValidateMem( var->data.sp );
		break;
	case TYPE_INST:
		jsinstValidateMem( var->data.ip );
		break;
	}
}

void varValidateMem( const variant *var )
{
	ASSERT_PTR( var );
	varValidateContentMem( var );
}

void varCheckMem( const variant *var )
{
	DEBUG_NOTE_MEM_ALLOCATION( var );
	varCheckContentMem( var );
}

void varCheckContentMem( const variant *var )
{
	switch ( var->type )
	{
	case TYPE_STRING:
		stringCheckMem( var->data.sp );
		break;
	case TYPE_INST:
		jsinstCheckMem( var->data.ip );
		break;
	case TYPE_PTR:
		varCheckContentMem( var->data.vp );
		break;
	}
}

#endif
