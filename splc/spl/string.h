/*
 *   This file is part of the Standard Portable Library (SPL).
 *
 *   SPL is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   SPL is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with SPL.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef _string_h
#define _string_h

#include <stdlib.h>

#include <spl/types.h>
#include <spl/debug.h>
#include <spl/collection/vector.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>

int StrCmpNoCase(const char *str1, int str1len, const char *str2);
int StrCpyLen(char *strto, const char *strfrom, const int maxlen);
void StrReplaceChLen(char *str, const char chfrom, const char chto, const int maxlen);
int IndexofchfromWithLen(const char *str, const char ch, int start, const int len);
#define IndexOfCh(s,c,z) IndexofchfromWithLen(s,c,z,(int)strlen(s))
int StrIndexOfWithLen(const char *str, const char* cp, int start, const int len);
#define StrIndexOf(s,c,z) IndexofchfromWithLen(s,c,z,(int)strlen(s))
int StrCatLen(char* to, const char* frm, int len);
int IndexOfAnyCh(const char* str, const char* cp, const int start, const int strLen);
bool ChRequiresXmlEncoding(const char ch);
bool StrRequiresXmlEncoding(const char* str);
void StrLTrimInPlace(char *str);

#define iswhitespace(ch) (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\n')

typedef struct _string
{
	char *m_cstr;
	bool m_isintern;
	int m_len;
} string;

string *StrReplaceCh(const char* cp, const char frm, const char to);

string* stringNew(const char* cp);
string* stringIntern(const char* cp);
string* stringTakeOwnership(char* cp);
void stringDelete(string* str);
string* stringDup(string* str);

string* stringReplaceCh(string* str, char frm, char to);
string* stringToUpper(string* str);
string* stringToUpperCh(const char* cp);

bool stringIsEqual(string* str, const char* cp, bool ignoreCase);
bool stringEndsWith(string* str, const char* cp, bool ignoreCase);
bool stringStartsWith(string* str, const char* cp, bool ignoreCase);

#define stringCharAt(str, idx) (str)->m_cstr[idx]
#define stringGetChars(str) str->m_cstr

int stringIndexOf(string* str, const char* cp, const int start, bool ignoreCase);
int stringIndexOfCh(string* str, const char ch, int start);
int stringIndexOfAnyCh(string* str, const char* cp, const int start);

uint32 stringHash(string* str, bool caseInvariant);
uint64 charHashLong(const char* str, const int len, const bool caseInvariant);
uint32 charHash(const char* str, const int len, const bool caseInvariant);

int stringCountChars(string* str, const char ch);
bool stringIsNumeric(string* str);
bool stringIsInt(string* str);
bool stringIsFloat(string* str);

string* stringCat(string* s, const char *cp, const int cplen);
string* stringReplace(string* s, const char* frm, const char* to);
string* stringRight(string* str, int len);
string* stringMid(string* str, int start, const int stop);
string* stringSubstring(string* str, int start, int len);
string* stringSubstringFrom(string* str, int start);
string* stringTrim(string* str);
string* stringReverse(string* str);
vector* stringSplit(string* str, const char* cp);
vector* stringSplitAnyCh(string* str, const char* cp);
void stringDeleteSplitVector(vector* v);

#ifdef DEBUG
void stringValidateMem(string* s);
#else
#define stringValidateMem(s)
#endif

#endif
