#include <spl/types.h>
#include <spl/debug.h>

#include <stdio.h>

#include <spl/io/file.h>
#include <spl/integer.h>
#include <spl/string.h>

int main(int argc, const char **argv)
{
	const int start = 24;
	FILE* fp;
	int x;
	vector* lines = fileLoadLines("defines.txt");
	
	if (lines == NULL)
	{
		printf("error loading defines.txt\n");
		return 20;	
	}
	
	if (NULL == (fp = fopen("defines.xml", "w")))
	{
		vectorDeleteStrings(lines);
		printf("can't open defines.xml for writing\n");
		return 20;
	}
	
	for (x = 0; x < vectorCount(lines)-3; x++)
	{
		string* defline = vectorElementAt(lines, x++);
		string* mapline = vectorElementAt(lines, x++);
		string* fileline  = vectorElementAt(lines, x);
		
		if (! stringStartsWith(mapline, "    CLASS             = MAP", false))
		{
			continue;
		}
		
		StrLTrimInPlace(&defline->m_cstr[start]);
		
		fprintf(fp, "%s", "<row define='");
		fprintf(fp, "%s", &defline->m_cstr[start]);
		fprintf(fp, "%s", "' file='");
		fprintf(fp, "%s", &fileline->m_cstr[start]);
		fprintf(fp, "%s", "' />\r\n");
	}

	fclose(fp);
	vectorDeleteStrings(lines);
	
	return 0;
}
