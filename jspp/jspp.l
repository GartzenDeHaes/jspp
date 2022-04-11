%{
#include <string.h>
#include "y.tab.h"

int yylineno = 1;

%}

delim	[ \t\n]
ws		{delim}+
letter	[A-Za-z]
digit	[0-9]
real	{digit}+(\.{digit}+)?(E[+/-]?{digit}+)
integer	{digit}+
id		(_|{letter})(_|{digit}|{letter})*
string	\".*\"
char	(\'.\')|(\'\\.\')
comment	"/*".*"*/"

%%

"if"				{return IF;}
"else"				{return ELSE;}
"while"				{return WHILE;}
"for"				{return FOR;}
"break"				{return BREAK;}
"var"				{return VAR;}
"return"			{return RETURN;}
"class"				{return CLASS;}
"public"			{return PUBLIC;}
"private"			{return PRIVATE;}
"protected"			{return PROTECT;}
"static"			{return STATIC;}
"{"					{return LBRACE;}
"}"					{return RBRACE;}
"("					{return LPAR;}
")"					{return RPAR;}
"["					{return LBRAC;}
"]"					{return RBRAC;}
{id}				{return ID;}
{char}				{return CHAR;}
{string}			{return STRING;}
{integer}			{return INT;}
{real}				{return REAL;}
{ws}				{}
"!="				{return ISNEQ;}
"!"					{return BANG;}
"=="				{return ISEQUAL;}
"+="				{return PLUSEQ;}
"-="				{return MINUSEQ;}
"*="				{return TIMESEQ;}
"/="				{return DIVEQ;}
"%="				{return MODEQ;}
"^="				{return XOREQ;}
"="					{return ASSIGN;}
"++"				{return INC;}
"--"				{return DEC;}
"+"					{return PLUS;}
"-"					{return MINUS;}
"*"					{return STAR;}
"//"				{return SLASHSLASH;}
"/"					{return SLASH;}
"&&"				{return AND;}
"&"					{return AMPR;}
"||"				{return OR;}
"|"					{return PIPE;}
"^"					{return XOR;}
"%"					{return MOD;}
"::"				{return SCOPE;}
":"					{return COLON;}
";"					{return SEMI;}
"."					{return DOT;}
","					{return COMMA;}
"<="				{return LTEQ;}
"<"					{return LT;}
">="				{return GTEQ;}
">"					{return GT;}
{comment}			{}
<<EOF>>				{return JEOF;}
"\n"				{yylineno++;}

%%

