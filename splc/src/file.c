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
#include <spl/io/file.h>
#include <spl/stringbuffer.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_IO_H
#include <io.h>
#endif
#ifdef HAVE_DIRECT_H
#include <direct.h>
#endif
#ifdef HAVE_DIRENT_H
#include <dirent.h>
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif

FILE *_msokfopen(const char* filename, const char* mode)
{
#if defined(_MSC_VER) && (_MSC_VER >= 1400)
	FILE* fp = NULL;
	errno_t err;
	string* osfn = fileToOsFilePath(filename);
	if (osfn == NULL)
	{
		return NULL;
	}
	
	err = fopen_s(&fp, osfn->m_cstr, mode);
	
	stringDelete(osfn);
	
	if (!err && fp)
	{
		return fp;
	}
	return NULL;
#else
	return fopen(filename, mode);
#endif
}

string *fileToOsFilePath(const char* filepathname)
{
#if defined(_WIN32) || defined(_WIN64)
	return StrReplaceCh(filepathname, '/', '\\');
#else
	if (IndexOfCh(filepathname, ':', 0) > -1)
	{
		return NULL;
	}
	return StrReplaceCh(filepathname, '\\', '/');
#endif
}

bool fileDelete(const char* filename)
{
	bool ret = false;
	string* osfn = fileToOsFilePath(filename);
	if (osfn == NULL)
	{
		return false;
	}
	
	ret = remove(osfn->m_cstr) == 0;
	stringDelete(osfn);
	return ret;	
}

bool fileExists(const char* filename)
{
	bool ret = false;
	FILE *fp;
	string* osfn = fileToOsFilePath(filename);
	if (osfn == NULL)
	{
		return false;
	}
	
	fp = _msokfopen(osfn->m_cstr, "r");
	if (NULL != fp)
	{
		ret = true;
		fclose(fp);
	}
	
	stringDelete(osfn);
	
	return ret;
}

long fileSize(const char* filename)
{
	long ret;
	struct stat file_stats;
	string* osfn = fileToOsFilePath(filename);
	if (osfn == NULL)
	{
		return false;
	}
	if(0 != stat(osfn->m_cstr, &file_stats))
    {
		ret = -1;
	}
	else
	{     
		ret = (long)file_stats.st_size;
	}
	
	stringDelete(osfn);
	
	return ret;
}

string* fileLoadText(const char* filename)
{
	#define cbufSize 1024

	FILE *fp;
	char cbuf[cbufSize];
	stringbuffer* buf;
	string* ret;
	int count;
	int size = fileSize(filename);
	if (size < 1)
	{
		size = 4096;
	}

	fp = _msokfopen(filename, "r");
	if ( NULL == fp )
	{
		return NULL;
	}

	if (NULL == (buf = stringbufferNew(size+1)))
	{
		return NULL;
	}

	while ((count = (int)fread(cbuf, 1, cbufSize, fp) ) > 0)
	{
		if (! stringbufferAppendBuf(buf, cbuf, count))
		{
			stringbufferDelete(buf);
			return NULL;
		}
	}
	
	ret = stringbufferToString(buf);
	stringbufferDelete(buf);
	
	return ret;
}

bool fileAppendText(const char* filename, string* txt)
{
	FILE *fp = _msokfopen(filename, "a");
	bool ret = fprintf(fp, "%s", txt->m_cstr) > 0;
	fclose(fp);
	
	return ret;
}

vector* fileLoadLines(const char* filename)
{
	int x;
	stringbuffer* buf;
	vector* lines;
	string* txt = fileLoadText(filename);
	
	if (txt == NULL)
	{
		return NULL;
	}
	
	if (NULL == (lines = vectorNewSized(2048)))
	{
		stringDelete(txt);
		return NULL;
	}
	
	if (NULL == (buf = stringbufferSizeNew(128)))
	{
		vectorDelete(lines);
		stringDelete(txt);
		return NULL;		
	}
	
	for (x = 0; x < txt->m_len; x++)
	{
		if (txt->m_cstr[x] == '\n')
		{
			if (buf->m_used > 0)
			{
				vectorAdd(lines, stringbufferToString(buf));
			}
			stringbufferClear(buf);
		}
		else if (txt->m_cstr[x] != '\r')
		{
			stringbufferAppendCh(buf, txt->m_cstr[x]);
		}
	}

	vectorAdd(lines, stringbufferToString(buf));
	
	stringbufferDelete(buf);	
	stringDelete(txt);
	
	return lines;
}
