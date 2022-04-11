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
#ifndef _Dates_h
#define _Dates_h

#include <stdlib.h>

#include <spl/types.h>
#include <spl/debug.h>
#include <spl/string.h>

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#ifdef HAVE_TIME_H
#include <time.h>
#endif

typedef struct _datetime
{
	struct tm m_dtm;
	time_t m_timet;
	
} datetime;

void datetimeInit(datetime* dtm);
datetime* datetimeNew();
void datetimeDelete(datetime* dtm);

int datetimeYear(datetime* dtm);
int datetimeMonth(datetime* dtm);
int datetimeDay(datetime* dtm);
int datetimeHour(datetime* dtm);
int datetimeMinute(datetime* dtm);
int datetimeSecond(datetime* dtm);

string* datetimeToString(datetime* dtm);
void datetimeToStringBuf(datetime* dtm, char* buf, const int buflen);

#endif
