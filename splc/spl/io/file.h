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
#ifndef _file_h
#define _file_h

#include <spl/types.h>
#include <spl/debug.h>
#include <spl/string.h>
#include <spl/collection/vector.h>

/*
typedef enum _filemode
{
	FILEMODE_Append = 6,
	FILEMODE_Create = 2,
	FILEMODE_CreateNew = 1,
	FILEMODE_Open = 3,
	FILEMODE_OpenOrCreate = 4,
	FILEMODE_Truncate = 5
} filemode;

typedef enum _fileaccess
{
	FILEACC_Read = 0x1,
	FILEACC_Write = 0x2,
	FILEACC_ReadWrite = FILEACC_Read | FILEACC_Write
} fileaccess;
*/

string* fileToOsFilePath(const char* filepathname);

bool fileDelete(const char* filename);
bool fileExists(const char* filename);
long fileSize(const char* filename);

string* fileLoadText(const char* filename);
bool fileAppendText(const char* filename, string* txt);
vector* fileLoadLines(const char* filename);

#endif
