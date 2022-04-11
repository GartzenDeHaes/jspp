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
#include <spl/collection/vector.h>
#include <spl/string.h>

static bool _vectorExtend(vector* v, int newlen)
{
	int x;
	void** newbuf;

	ASSERT_MEM(v->m_vdata, sizeof(void *) * v->m_count);

	if ((newbuf = (void **)malloc(newlen * sizeof(void *))) == NULL)
	{
		return false;
	}
	
	for (x = 0; x < v->m_count; x++)
	{
		newbuf[x] = v->m_vdata[x];
	}
	
	v->m_count = newlen;
	free(v->m_vdata);
	v->m_vdata = newbuf;
	
	ASSERT_MEM(v->m_vdata, sizeof(void *) * v->m_count);
	
	return true;
}

static bool _vectorExtendTo(vector* v, int index)
{
	if (index > v->m_count)
	{
		return _vectorExtend(v, index+1);
	}
	
	return true;
}

vector* vectorNew()
{
	return vectorNewSized(20);
}

vector* vectorNewSized(const int count)
{
	vector* v = (vector *)malloc(sizeof(vector));
	if (v == NULL)
	{
		return NULL;
	}
	
	v->m_count = count;
	v->m_pos = 0;
	v->m_vdata = (void**)malloc(sizeof(void *) * v->m_count);
	
	if (v->m_vdata == NULL)
	{
		free(v);
		return NULL;
	}
	
	return v;
}

void vectorDelete(vector* v)
{
	vectorValidateMem(v);
	
	free(v->m_vdata);
	free(v);
}

void vectorDeleteStrings(vector* v)
{
	int x;
	for (x = 0; x < vectorCount(v); x++)
	{
		stringDelete((string *)vectorElementAt(v, x));
	}
	
	vectorDelete(v);
}

void* vectorElementAt(vector* v, int idx)
{
	ASSERT(idx >= 0);
	ASSERT(idx < v->m_pos);
	/*ASSERT_MEM(v->m_vdata, sizeof(void *) * v->m_count);*/
	
	if (idx >= v->m_pos || 0 > idx)
	{
		return NULL;
	}
	
	return v->m_vdata[idx];
}

bool vectorSetElementAt(vector* v, int idx, void *data)
{
	if (idx >= v->m_count)
	{
		if (! _vectorExtendTo(v, idx+1))
		{
			return false;
		}
	}
	if (idx >= v->m_pos)
	{
		v->m_pos = idx + 1;
	}
	v->m_vdata[idx] = data;
	
	return true;
}

bool vectorAdd(vector* v, void *data)
{
	if (v->m_pos == v->m_count)
	{
		if (! _vectorExtend(v, v->m_count * 2))
		{
			return false;
		}
	}
	
	v->m_vdata[v->m_pos++] = data;
	return true;
}

#ifdef DEBUG
void vectorValidateMem(vector* v)
{
	ASSERT_MEM(v, sizeof(vector));
	ASSERT_MEM(v->m_vdata, sizeof(void *) * v->m_count);
}
#endif
