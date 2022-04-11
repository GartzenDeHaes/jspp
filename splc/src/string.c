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
#include <spl/string.h>
#include <spl/stringbuffer.h>

int StrCmpNoCase(const char *str1, int str1len, const char *str2)
{
	while (*str1 != '\0' && *str2  != '\0' && str1len-- >= 0)
	{
		if (toupper(*str1) != toupper(*str2))
		{
			if (*str1 > *str2)
			{
				return 1;
			}
			return -1;
		}
		str1++;
		str2++;
	}
	if ( *str1 != '\0' )
	{
		return 1;
	}
	if ( *str2 != '\0' )
	{
		return -1;
	}
	return 0;
}

int StrCpyLen(char *strto, const char *strfrom, const int maxlen)
{
	int len = maxlen;
	while (*strfrom != '\0' && --len > 0)
	{
		*(strto++) = *(strfrom++);
	}
	*strto = '\0';
	return -(len - maxlen);
}

int IndexofchfromWithLen( const char *str, const char ch, int start, const int len )
{
	int pos;

	for (pos = start; pos < len; pos++)
	{
		if (str[pos] == ch)
		{
			return pos;
		}
	}
	return -1;
}

int StrIndexOfWithLen(const char *str, const char* cp, int start, const int len)
{
	int pos;
	int m_len = (int)strlen(str);
	for (pos = start; pos < m_len; pos++)
	{
		int end;
		int cppos = 1;

		if (str[pos] == cp[0])
		{
			for (end = pos+1; end < m_len && cp[cppos] != '\0'; end++)
			{
				if (str[end] != cp[cppos])
				{
					break;
				}
				cppos++;
			}
			if (cp[cppos] == '\0')
			{
				return pos;
			}
		}
	}
	
	return -1;
}

int StrCatLen(char* to, const char* frm, int len)
{
	int topos = 0;
	
	/* ensure space for null */
	len--;
	
	while(to[topos] != '\0' && topos < len)
	{
		topos++;
	}
	while(*frm != '\0' && topos < len)
	{
		to[topos++] = *frm++;
	}
	
	to[topos] = '\0';
	
	return topos;
}

int IndexOfAnyCh(const char* str, const char* cp, const int start, const int strLen)
{
	int pos;
	int cppos;
	int cplen = (int)strlen(cp);
	
	for (pos = start; pos < strLen; pos++)
	{
		for (cppos = 0; cppos < cplen; cppos++)
		{
			if (str[pos] == cp[cppos])
			{
				return pos;
			}
		}
	}
	
	return -1;	
}

bool ChRequiresXmlEncoding(const char ch)
{
	return ch == '&' ||
		ch == '\'' ||
		ch == '<' ||
		ch == '>' ||
		ch == '"';
}

bool StrRequiresXmlEncoding(const char* str)
{
	return IndexOfAnyCh(str, "&'<>\"\r\n", 0, (int)strlen(str)) > -1;
}

void StrReplaceChLen(char *str, const char chfrom, const char chto, const int maxlen)
{
	int x = 0;
	while (x < maxlen && str[x] != '\0')
	{
		if (str[x] == chfrom)
		{
			str[x] = chto;
		}
		x++;
	}
}

void StrLTrimInPlace(char *str)
{
	int len = (int)strlen(str) - 1;
	
	while (len >= 0 && str[len] == ' ')
	{
		str[len] = '\0';
		len--;
	}
}

string* stringNew(const char *cp)
{
	string *str = (string *)malloc(sizeof(string));
	if (str == NULL)
	{
		return NULL;
	}
	
	str->m_isintern = false;
	str->m_len = (int)strlen(cp);
	
	if (NULL == (str->m_cstr = (char *)malloc(str->m_len+1)))
	{
		free(str);
		return NULL;
	}
	
	StrCpyLen(str->m_cstr, &cp[0], str->m_len);
	
	return str;
}

string* stringDup(string* str)
{
	return stringNew(str->m_cstr);
}

string* stringTakeOwnership(char* cp)
{
	string *str = stringIntern(cp);
	if (str == NULL)
	{
		return NULL;
	}
	
	str->m_isintern = false;
		
	return str;	
}

string* stringIntern(const char* cp)
{
	string *str = (string *)malloc(sizeof(string));
	if (str == NULL)
	{
		return NULL;
	}
	
	str->m_isintern = true;
	str->m_len = (int)strlen(cp);
	str->m_cstr = (char *)cp;
	
	return str;	
}

void stringDelete(string *str)
{
	if (! str->m_isintern)
	{
		free(str->m_cstr);
	}
	free(str);
}

string *StrReplaceCh(const char* cp, const char frm, const char to)
{
	int x;
	string* ret = stringNew(cp);
	if (ret == NULL)
	{
		return NULL;
	}
	
	for (x = 0; x < ret->m_len; x++)
	{
		if (ret->m_cstr[x] == frm)
		{
			ret->m_cstr[x] = to;
		}
	}
	
	return ret;
}

string* stringReplaceCh(string* str, char frm, char to)
{
	return StrReplaceCh(str->m_cstr, frm, to);
}

string* stringToUpperCh(const char* cp)
{
	int x = 0;
	string* s2 = stringNew(cp);
	if (s2 == NULL)
	{
		return NULL;
	}
	
	while (s2->m_cstr[x] != '\0')
	{
		s2->m_cstr[x] = (char)toupper(s2->m_cstr[x]);
		x++;
	}
	
	return s2;
}

string* stringToUpper(string* str)
{	
	return stringToUpperCh(str->m_cstr);	
}

bool stringIsEqual(string* str, const char* cp, bool ignoreCase)
{
	char *scp = str->m_cstr;
	while (*cp != '\0' && *scp != '\0')
	{
		if (ignoreCase)
		{
			if (toupper(*cp++) != toupper(*scp++))
			{
				return false;
			}
		}
		else
		{
			if (*cp++ != *scp++)
			{
				return false;
			}
		}
	}
	
	return *cp == '\0' && *scp == '\0';	
}

bool stringStartsWith(string* str, const char* cp, bool ignoreCase)
{
	int x;
	int cplen = (int)strlen(cp);
	
	if (str->m_len < cplen)
	{
		return false;
	}

	for (x = 0; x < str->m_len && *cp != '\0'; x++)
	{
		if (ignoreCase)
		{
			if (toupper(str->m_cstr[x]) != toupper(*cp++))
			{
				return false;
			}
		}
		else
		{
			if (str->m_cstr[x] != *cp++)
			{
				return false;
			}
		}
	}
	return true;
}

bool stringEndsWith(string* str, const char* cp, bool ignoreCase)
{
	int pos = str->m_len - (int)strlen(cp);
	int x;

	if ( pos <= 0 )
	{
		return false;
	}
	for (x = pos; x < str->m_len; x++)
	{
		if (ignoreCase)
		{
			if (toupper(str->m_cstr[x]) != toupper(*cp++))
			{
				return false;
			}
		}
		else
		{
			if (str->m_cstr[x] != *cp++)
			{
				return false;
			}
		}
	}
	return true;
}

string* stringCat(string* s, const char *cp, const int cplen)
{
	int len = s->m_len + cplen + 1;
	char* buf = (char *)malloc(len);
	if (buf == NULL)
	{
		return NULL;
	}
	
	StrCpyLen(buf, s->m_cstr, len);
	StrCatLen(buf, cp, len);
	
	return stringTakeOwnership(buf);
}

string* stringReplace(string* s, const char* frm, const char* to)
{
	stringbuffer* buf;
	string* ret;
	int start = 0;
	int fromlen;
	int pos = stringIndexOf(s, frm, 0, false);
	if ( 0 > pos )
	{
		return stringDup(s);
	}

	fromlen = (int)strlen(frm);
	
	if (NULL == (buf = stringbufferSizeNew(s->m_len + 10)))
	{
		return NULL;
	}

	while (start < s->m_len)
	{
		stringbufferAppendEx(buf, s->m_cstr, start, pos - start);
		start = pos;
		stringbufferAppend(buf, to);
		start += fromlen;

		if (0 > (pos = stringIndexOf(s, frm, start, false)))
		{
			stringbufferAppendEx(buf, s->m_cstr, start, s->m_len - start);
			start = s->m_len;
		}
	}
	
	ret = stringbufferToString(buf);
	stringbufferDelete(buf);
	return ret;
}

string* stringTrim(string* str)
{
	int x;
	int pos;
	int endpos;
	string* ret;
	stringbuffer* buf = stringbufferSizeNew(str->m_len);
	if (buf == NULL)
	{
		return NULL;
	}

	for (pos = 0; str->m_cstr[pos] == ' ' && pos < str->m_len; pos++)
	{
	}
	for (endpos = str->m_len-1; str->m_cstr[endpos] == ' ' && endpos >= 0; endpos--)
	{
	}
	for (x = pos; x <= endpos; x++)
	{
		stringbufferAppendCh(buf, str->m_cstr[x]);
	}
	
	ret = stringbufferToString(buf);
	stringbufferDelete(buf);
	return ret;
}

#define CHECKUPPER(c,b) (b ? toupper(c) : c)

uint64 charHashLong(const char* str, const int len, const bool caseInvariant)
{
	uint64 h = 0;
	int x;
	
	for (x = 0; x < len; x++)
	{
		h = (h << 6) ^ (h >> 58) ^ CHECKUPPER(str[x], caseInvariant);
	}

	return h;
}

uint32 charHash(const char* str, const int len, const bool caseInvariant)
{
	uint64 h = charHashLong(str, len, caseInvariant);
	return (uint32)((h & 0xFFFFFFFF) ^ (h >> 32));
}

uint32 stringHash(string* str, bool caseInvariant)
{
	return charHash(str->m_cstr, str->m_len, caseInvariant);
}

int stringIndexOf(string* str, const char* cp, const int start, bool ignoreCase)
{
	int pos;
	for (pos = start; pos < str->m_len; pos++)
	{
		int end;
		int cppos = 1;

		if (str->m_cstr[pos] == cp[0])
		{
			for(end = pos+1; end < str->m_len && cp[cppos] != '\0'; end++)
			{
				if (ignoreCase)
				{
					if (toupper(str->m_cstr[end]) != toupper(cp[cppos]))
					{
						break;
					}
				}
				else
				{
					if (str->m_cstr[end] != cp[cppos])
					{
						break;
					}
				}
				cppos++;
			}
			if (cp[cppos] == '\0')
			{
				return pos;
			}
		}
	}
	return -1;	
}

int stringIndexOfCh(string* str, const char ch, int start)
{
	return IndexofchfromWithLen(str->m_cstr, ch, start, str->m_len);
}

int stringIndexOfAnyCh(string* str, const char* cp, const int start)
{
	return IndexOfAnyCh(str->m_cstr, cp, start, str->m_len);
}

string* stringRight(string* str, int len)
{
	if (len > str->m_len)
	{
		len = str->m_len;
	}
	
	return stringSubstringFrom(str, str->m_len - len);
}

string* stringMid(string* str, int start, const int stop)
{
	return stringSubstring(str, start, stop - start);
}

string* stringSubstringFrom(string *str, int start)
{
	return stringSubstring(str, start, str->m_len - start);
}

string* stringSubstring(string* str, int start, int len)
{
	char *ret;
	const char *cpstart;
	string* strret;
	
	if (len == 0)
	{
		return stringNew("");
	}

	ASSERT ( len > 0 );

	if (start > str->m_len)
	{
		start = str->m_len;
	}
	if (start + len > str->m_len)
	{
		len = str->m_len - start;
	}
	if (len < 0)
	{
		len = 0;
	}
	
	cpstart = &str->m_cstr[start];
	ASSERT_PTR(cpstart);

	if (NULL == (ret = (char *)malloc( len+1 )))
	{
		return NULL;
	}
	
	StrCpyLen(ret, cpstart, len);
	ret[len] = '\0';
	ASSERT_MEM(ret, len+1);

	strret = stringNew(ret);
	free(ret);
	return strret;
}

void stringDeleteSplitVector(vector* v)
{
	int count = vectorCount(v);
	int x;
	
	vectorValidateMem(v);
	
	for (x = 0; x < count; x++)
	{
		string* str = (string *)vectorElementAt(v, x);
		stringValidateMem(str);
		stringDelete(str);
	}
	
	vectorDelete(v);
}

vector* stringSplit(string* str, const char* cp)
{
	string* span;
	int delimlen = (int)strlen(cp);
	int delimpos = stringIndexOf(str, cp, 0, false);
	int pos = 0;
	vector* v = vectorNew();
	if (v == NULL)
	{
		return NULL;
	}
	
	if (delimpos < 0)
	{
		if (! vectorAdd(v, stringDup(str)))
		{
			stringDeleteSplitVector(v);
			v = NULL;
		}
		return v;
	}
	
	while (pos < str->m_len)
	{
		span = stringMid(str, pos, delimpos);
		pos = delimpos + delimlen;
		if (! vectorAdd(v, span))
		{
			stringDeleteSplitVector(v);
			return NULL;
		}

		delimpos = stringIndexOf(str, cp, delimpos+1, false);
		if ( delimpos < 0 )
		{
			break;
		}
	}
	
	pos = str->m_len - pos;
	ASSERT(pos >= 0);
	if (pos > 0)
	{
		span = stringRight(str, pos);
		if (! vectorAdd(v, span))
		{
			stringDeleteSplitVector(v);
			return NULL;
		}
	}
	
	return v;
}

vector* stringSplitAnyCh(string* str, const char* cp)
{
	string* span;
	int delimlen = 1;
	int delimpos = stringIndexOfAnyCh(str, cp, 0);
	int pos = 0;
	vector* v = vectorNew();
	if (v == NULL)
	{
		return NULL;
	}
	
	if (delimpos < 0)
	{
		if (! vectorAdd(v, stringDup(str)))
		{
			stringDeleteSplitVector(v);
			v = NULL;
		}
		return v;
	}
	
	while (pos < str->m_len)
	{
		if (NULL == (span = stringMid(str, pos, delimpos)))
		{
			stringDeleteSplitVector(v);
			return NULL;
		}
		
		pos = delimpos + delimlen;
		
		if (span->m_len == 0)
		{
			stringDelete(span);
		}
		else
		{
			if (! vectorAdd(v, span))
			{
				stringDeleteSplitVector(v);
				return NULL;
			}
		}

		delimpos = stringIndexOfAnyCh(str, cp, delimpos+1);
		if ( delimpos < 0 )
		{
			break;
		}
	}
	
	pos = str->m_len - pos;
	ASSERT(pos >= 0);
	if (pos > 0)
	{
		if (NULL == (span = stringRight(str, pos)))
		{
			stringDeleteSplitVector(v);
			return NULL;
		}
		if (span->m_len == 0)
		{
			stringDelete(span);
		}
		else
		{
			if (! vectorAdd(v, span))
			{
				stringDeleteSplitVector(v);
				return NULL;
			}
		}
	}
	
	return v;
}

int stringCountChars(string* str, const char ch)
{
	int x;
	int count = 0;
	
	for (x = 0; x < str->m_len; x++)
	{
		if (str->m_cstr[x] == ch)
		{
			count++;
		}
	}
	
	return count;
}

bool stringIsNumeric(string* str)
{
	return stringIsInt(str) || stringIsFloat(str);
}

bool stringIsInt(string* str)
{
	int x;
	for (x = 0; x < str->m_len; x++)
	{
		if (! isdigit(str->m_cstr[x]))
		{
			return false;
		}
	}
	
	return true;
}

bool stringIsFloat(string* str)
{
	int x;
	int dotcount = 0;
	char ch;
	
	for (x = 0; x < str->m_len; x++)
	{
		ch = str->m_cstr[x];
		
		if (! isdigit(ch))
		{
			if (ch == '.' && dotcount == 0)
			{
				dotcount++;
			}
			else
			{
				return false;
			}
		}
	}

	return true;	
}

string* stringReverse(string* str)
{
	char c;
	int i, j;
	string* ret = stringDup(str);
	char* s;
	
	if (ret == NULL)
	{
		return NULL;
	}
	
	s = ret->m_cstr;

	for (i = 0, j = (int)strlen(s)-1; i < j; i++, j--) 
	{
		c = s[i];
		s[i] = s[j];
		s[j] = c;
	}
	
	return ret;
}

#ifdef DEBUG
void stringValidateMem(string* s)
{
	ASSERT_MEM(s, sizeof(string));
	if (! s->m_isintern)
	{
		ASSERT_MEM(s->m_cstr, s->m_len);
	}
}
#endif

#ifdef __TANDEM

#include <errno.h>
#include <stdlib.h>

/* Copyright (C) 1995 DJ Delorie, see COPYING.DJ for details */
char *itoa(int value, char *string, int radix)
{
	char tmp[33];
	char *tp = tmp;
	int i;
	unsigned v;
	int sign;
	char *sp;

	if (radix > 36 || radix <= 1)
	{
		errno = EDOM;
		return 0;
	}

	sign = (radix == 10 && value < 0);
	if (sign)
		v = -value;
	else
		v = (unsigned)value;
	while (v || tp == tmp)
	{
		i = v % radix;
		v = v / radix;
		if (i < 10)
		  *tp++ = i+'0';
		else
		  *tp++ = i + 'a' - 10;
	}

	if (string == 0)
	string = (char *)malloc((tp-tmp)+sign+1);
	sp = string;

	if (sign)
		*sp++ = '-';
	while (tp > tmp)
		*sp++ = *--tp;
	*sp = 0;
	return string;
}

char *ltoa(long value, char *string, int radix)
{
	char tmp[33];
	char *tp = tmp;
	long i;
	unsigned long v;
	int sign;
	char *sp;

	if (radix > 36 || radix <= 1)
	{
	 errno = EDOM;
	return 0;
	}

	sign = (radix == 10 && value < 0);
	if (sign)
	v = -value;
	else
	v = (unsigned long)value;
	while (v || tp == tmp)
	{
	i = v % radix;
	v = v / radix;
	if (i < 10)
	  *tp++ = i+'0';
	else
	  *tp++ = i + 'a' - 10;
	}

	if (string == 0)
	string = (char *)malloc((tp-tmp)+sign+1);
	sp = string;

	if (sign)
	*sp++ = '-';
	while (tp > tmp)
	*sp++ = *--tp;
	*sp = 0;
	return string;
}

char *_ultoa(unsigned long value, char *string, int radix)
{
  char tmp[33];
  char *tp = tmp;
  long i;
  unsigned long v = value;
  char *sp;

  if (radix > 36 || radix <= 1)
  {
    errno = EDOM;
    return 0;
  }
 
  while (v || tp == tmp)
  {
    i = v % radix;
    v = v / radix;
    if (i < 10)
      *tp++ = i+'0';
    else
      *tp++ = i + 'a' - 10;
  }

  if (string == 0)
    string = (char *)malloc((tp-tmp)+1);
  sp = string;

 
  while (tp > tmp)
    *sp++ = *--tp;
  *sp = 0;
  return string;
}
#endif
