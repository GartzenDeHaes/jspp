#include <spl/collection/hashtable.h>

hashtable* hashtableNew()
{
	hashtable* ht = (hashtable *)malloc(sizeof(hashtable));
	if (ht == NULL)
	{
		return NULL;
	}
	
	ht->m_count = 0;
	ht->m_size = 701;
	ht->m_keyIgnoreCase = false;
	ht->m_table = (hashitem **)malloc(sizeof(hashitem *) * ht->m_size);
	if (ht->m_table == NULL)
	{
		free(ht);
		return NULL;
	}
	
	memset(ht->m_table, 0, sizeof(hashitem *) * ht->m_size);
	
	return ht;
}

void hashtableDelete(hashtable* ht, bool freeValues)
{
	int x;
	
	hashtableValidateMem(ht);
	
	for (x = 0; x < ht->m_size; x++)
	{
		if (ht->m_table[x] != NULL)
		{
			stringDelete(ht->m_table[x]->m_key);
			if (freeValues)
			{
				free(ht->m_table[x]->m_value);
			}
			free(ht->m_table[x]);
		}
	}
	
	free(ht->m_table);
	free(ht);
}

void hashtableDeleteStringValues(hashtable* ht)
{
	int x;
	
	hashtableValidateMem(ht);
	
	for (x = 0; x < ht->m_size; x++)
	{
		if (ht->m_table[x] != NULL)
		{
			stringDelete(ht->m_table[x]->m_key);
			stringDelete(ht->m_table[x]->m_value);
			free(ht->m_table[x]);
		}
	}
	
	free(ht->m_table);
	free(ht);
}

static hashitem* _hashtableGetItem(hashtable* ht, const char *cp)
{
	hashitem* hi;
	uint32 hash = charHash(cp, (int)strlen(cp), ht->m_keyIgnoreCase) % ht->m_size;

	hi = ht->m_table[hash];
	if (hi == NULL)
	{
		return NULL;
	}
	
	while (hi != NULL)
	{
		if (stringIsEqual(hi->m_key, cp, ht->m_keyIgnoreCase))
		{
			return hi;
		}
		hi = hi->m_next;
	}
	
	return NULL;
}

bool hashtableContainsKey(hashtable* ht, const char *cp)
{
	return _hashtableGetItem(ht, cp) != NULL;
}

void* hashtableGet(hashtable* ht, const char *cp)
{
	hashitem* hi = _hashtableGetItem(ht, cp);
	if (hi != NULL)
	{
		return hi->m_value;
	}
	
	return NULL;
}

bool hashtablePut(hashtable* ht, const char *cp, void *val, bool freeOld)
{
	hashitem* hi;
	uint32 hash = charHash(cp, (int)strlen(cp), ht->m_keyIgnoreCase) % ht->m_size;

	hi = ht->m_table[hash];
	
	while (hi != NULL)
	{
		if (stringIsEqual(hi->m_key, cp, ht->m_keyIgnoreCase))
		{
			if (freeOld)
			{
				free(hi->m_value);
			}
			hi->m_value = val;
			return true;
		}
		hi = hi->m_next;		
	}
	
	hi = (hashitem *)malloc(sizeof(hashitem));
	if (hi == NULL)
	{
		return false;
	}
	hi->m_next = ht->m_table[hash];
	hi->m_key = stringNew(cp);
	if (hi->m_key == NULL)
	{
		free(hi);
		return false;
	}
	hi->m_value = val;
	ht->m_table[hash] = hi;
	
	ht->m_count++;
	
	return true;
}

int hashtableCount(hashtable* ht)
{
	return ht->m_count;
}

#ifdef DEBUG

void hashtableValidateMem(hashtable* ht)
{
	int x;
	
	ASSERT_MEM(ht, sizeof(hashtable));
	ASSERT_MEM(ht->m_table, sizeof(hashitem *) * ht->m_size);

	for (x = 0; x < ht->m_size; x++)
	{
		if (ht->m_table[x] != NULL)
		{
			ASSERT_MEM(ht->m_table[x], sizeof(hashitem));
			stringValidateMem(ht->m_table[x]->m_key);
			if (ht->m_table[x]->m_value != NULL)
			{
				ASSERT_PTR(ht->m_table[x]->m_value);
			}
		}
	}	
}

#endif
