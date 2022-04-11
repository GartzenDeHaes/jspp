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
#ifndef _vector_h
#define _vector_h

#include <spl/types.h>
#include <spl/debug.h>

typedef struct _vector
{
	int m_count;
	int m_pos;
	void **m_vdata;
} vector;

vector* vectorNew();
vector* vectorNewSized(const int count);
void vectorDelete(vector* v);
void vectorDeleteStrings(vector* v);

#define vectorCount(v) v->m_pos

void* vectorElementAt(vector* v, int idx);
bool vectorSetElementAt(vector* v, int idx, void *data);
bool vectorAdd(vector* v, void *data);

#ifdef DEBUG
void vectorValidateMem(vector* ht);
#else
#define vectorValidateMem(v)
#endif

#endif
