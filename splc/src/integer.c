#include <spl/integer.h>
#include <ctype.h>
#include <stdio.h>

static const int _base10 = 10;
/*static char _cHexa[] = { 'A', 'B', 'C', 'D', 'E', 'F' };*/
static int _iHexaNumeric[] = { 10, 11, 12, 13, 14, 15 };
/*static int _iHexaIndices[] = { 0, 1, 2, 3, 4, 5 };*/
static const int _asciiDiff = 48;

static uint64 _baseToInt(const char *sbase, int numbase)
{
	int i;
	uint64 dec = 0;
	int b;
	int iProduct = 1;
	int sbaselen = (int)strlen(sbase);
	int stopAt = 0;
	
	if (sbase[0] == '0' && (sbase[1] == 'x' || sbase[1] == 'X'))
	{
		stopAt = 2;
	}

	for (i = sbaselen - 1; i >= stopAt; i--, iProduct *= numbase)
	{
		char ch = sbase[i];
		if (numbase > _base10 && isalpha(ch))
		{
			ASSERT( 'A' > 'a' );
			if ( ch >= 'A' )
			{
				b = _iHexaNumeric[ch-'A'];
			}
			else
			{
				b = _iHexaNumeric[ch-'a'];
			}
		}
		else 
		{
			b = (int) sbase[i] - _asciiDiff;
		}
		dec += (b * iProduct);
	} 
	return dec; 
}

int32 int32Parse(const char* cp, const int cplen, int radix)
{
	if (radix == 10)
	{
		return atoi(cp);
	}
	
	if (cp[0] == '-')
	{
		int32 i = (int32)_baseToInt(&cp[1], radix);
		return -i;
	}
	return (int32)_baseToInt(cp, radix);	
}

int64 int64Parse(const char* cp, const int cplen, int radix)
{
	if (cp[0] == '-')
	{
		int64 i = (int64)_baseToInt(&cp[1], radix);
		return -i;
	}
	return (int64)_baseToInt(cp, radix);	
}

string* int32ToString(int32 i)
{
	char buf[32];
	
	sprintf(buf, "%d", i);
	
	return stringNew(buf);
}

string* int64ToString(int64 i)
{
	char buf[64];
	
	sprintf(buf, "%ld", i);
	
	return stringNew(buf);
}
