#ifndef _users_h
#define _users_h

#include <spl/collection/hashtable.h>
#include <spl/collection/vector.h>

typedef struct _users
{
	hashtable* m_uid2pw;
	hashtable* m_uid2perms;
	hashtable* m_perms;
} users;

users* usersNew();
void usersDelete(users* u);

bool usersAdd(users* u, const char* uid, const char* pw, bool isadmin);
bool usersAddPerm(users* u, const char* uid, const char* perm);
bool usersParseGrant(users* u, string* system, string* sql);
bool usersParseCreateLogon(users* u, string* sql);

bool userLogon(users* u, string* uid, string* pw);
bool usersHasPerm(users* u, string* uid, string* perm);
bool usersIsPerm(users* u, string* perm);
bool usersIsAdmin(users* u, string* uid);

vector* usersList(users* u);
vector* usersListPerms(users* u, const char* userid);

#ifdef DEBUG
void usersValidateMem(users* u);
#else
#define usersValidateMem(u)
#endif

#endif
