#ifndef _hashtable_h
#define _hashtable_h

#include <spl/types.h>
#include <spl/debug.h>
#include <spl/string.h>

typedef struct _hashitem
{
	struct _hashitem* m_next;
	string* m_key;
	void *m_value;
} hashitem;

typedef struct _hashtable
{
	int m_size;
	int m_count;
	bool m_keyIgnoreCase;
	hashitem **m_table;
} hashtable;

hashtable* hashtableNew();
void hashtableDelete(hashtable* ht, bool freeValues);
void hashtableDeleteStringValues(hashtable* ht);

bool hashtablePut(hashtable* ht, const char *cp, void *val, bool freeOld);
void* hashtableGet(hashtable* ht, const char *cp);
bool hashtableContainsKey(hashtable* ht, const char *cp);

int hashtableCount(hashtable* ht);

#ifdef DEBUG
void hashtableValidateMem(hashtable* ht);
#else
#define hashtableValidateMem(ht)
#endif

#endif
