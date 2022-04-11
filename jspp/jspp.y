%{
#include <stdio.h>
#include <string.h>
/*#include "debug.h"*/

#define YYDEBUG 1

void emit( char *str );
extern int yylineno;
extern char *yytext;
extern int yylen;

void yyerror(const char *str)
{
	printf( "%s on line %d\n", str, yylineno );
}

%}

%token IF WHILE FOR VAR RETURN CLASS PUBLIC PRIVATE PROTECT LBRACE RBRACE LPAR RPAR
%token LBRAC RBRAC ID CHAR STRING INT REAL ISNEQ BANG ISEQUAL PLUSEQ MINUSEQ TIMESEQ
%token DIVEQ MODEQ XOREQ ASSIGN INC DEC PLUS MINUS STAR SLASHSLASH SLASH AND AMPR
%token OR PIPE XOR MOD SCOPE COLON SEMI DOT COMMA JEOF STATIC ELSE BREAK LT GT
%token LTEQ GTEQ

%%

file	: declaration file
		| JEOF
		;

declaration	: CLASS ID { emit("start class "); emit(yytext); } inherits LBRACE classbody RBRACE { emit("end class"); }
			| ID SCOPE methoddef {emit("end cpp method def");}
			;

inherits	: COLON ID {emit("inherits"); emit(yytext);}
			|
			;

decspec	: ASSIGN literal 
		| 
		;

literal	: STRING { emit(yytext); }
		| INT { emit(yytext); } 
		| REAL { emit(yytext); } 
		;

classbody	: vardef classbody
			| method classbody
			|
			;
			
vardef	: accessspec storagetype VAR ID {emit("declare var");} decspec SEMI
		;

method	: accessspec storagetype methoddef 
		;

storagetype	: STATIC
			|
			;

accessspec	: PUBLIC | PRIVATE | PROTECT 
			;

methoddef	: ID {emit("method");} LPAR idlist RPAR methodbody
			;

methodbody	: stmtblock
			| SEMI
			;

idlist	: VAR ID {emit("param");} moreidlist
		|
		;
	
moreidlist	: COMMA VAR ID {emit("param");} moreidlist
			|
			;
			
stmtblock	: LBRACE stmts RBRACE
			;
			
stmts	: bstmt stmts
		| sstmt stmts
		|
		;
		
bstmt	: ifstmt
		| whilestmt
		| forstmt
		| error RBRACE { yyerrok; }
		;

sstmt	: return SEMI
		| var SEMI
		| expr SEMI
		| SEMI
		| error SEMI { yyerrok; }
		;

return	: RETURN expr
		| RETURN
		;

ifstmt	: IF LPAR expr RPAR {emit("if");} stmtblock {emit("end if");} elsestmt
		;

elsestmt	: ELSE ifstmt
			| ELSE stmtblock {emit("end if blocks");}
			|
			;

whilestmt	: WHILE {emit("top of while");} LPAR expr RPAR {emit("w eval");} stmtblock {emit("end while block");}
			;

forstmt	: FOR LPAR initfor
		;

initfor	: var {emit("for var top");} SEMI evalfor
		| expr {emit("for expr top");} SEMI evalfor
		| SEMI evalfor
		;
		
evalfor	: expr {emit("for eval");} SEMI postfor
		| SEMI postfor
		;
		
postfor	: expr {emit("for post");} forblock
		| forblock 
		;
		
forblock	: RPAR stmtblock {emit("end for block");}
			;
		
var		: VAR ID {emit("declare var");} lhs
		;

lhs		: ASSIGN expr {emit("var assign");}
		|
		;

expr	: relop morerelops 
		;

relop	: term moreterms
		;
		
morerelops	: LT relop morerelops	{emit("<");}
			| GT relop morerelops	{emit(">");}
			| LTEQ relop morerelops	{emit("<=");}
			| GTEQ relop morerelops	{emit(">=");}
			|
			;

term	: factor morefactors
		;

moreterms	: ASSIGN term moreterms {emit("assign");}
			| PLUS term moreterms {emit("plus");}
			| MINUS term moreterms {emit("minus");}
			|
			;
			
factor	: unary
		;
		
morefactors	: STAR factor	{emit("*");}
			| SLASH factor	{emit("/");}
			| MOD factor	{emit("%");}
			|
			;

unary	: MINUS singleton	{emit("negate");}
		| PLUS singleton	{emit("pos");}
		| INC singleton		{emit("++");}
		| DEC singleton		{emit("--");}
		| singleton INC		{emit("w eval");}
		| singleton DEC		{emit("w eval");}
		| singleton
		;
		
singleton	: ID {emit(yytext);} deref
			| literal
			| LPAR expr RPAR
			;
			
deref	: DOT ID {emit("dot");} deref
		| LBRAC expr RBRAC {emit("array");} deref
		|
		;

%%

void emit( char *str )
{
	printf( "%s\n", str );
}

