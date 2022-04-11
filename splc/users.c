#include <spl/data/sqliteconnection.h>
#include <spl/data/sqlitecommand.h>
#include <spl/data/sqlitereader.h>

#include "conf.h"
#include "sqllex.h"
#include "users.h"

users* usersNew()
{
	sqlitecommand* cmd;
	sqlitereader* reader;
	sqliteconnection* conn = sqliteconnNew(g_dbName);
	users* u;
	
	if (conn == NULL)
	{
		return false;
	}

	u = (users *)malloc(sizeof(users));
	if (u == NULL)
	{
		sqliteconnDelete(conn);
		return NULL;
	}
	
	u->m_uid2pw = hashtableNew();
	if (u->m_uid2pw == NULL)
	{
		sqliteconnDelete(conn);
		free(u);
		return NULL;
	}
	u->m_uid2pw->m_keyIgnoreCase = true;
	
	u->m_uid2perms = hashtableNew();
	if (u->m_uid2perms == NULL)
	{
		sqliteconnDelete(conn);
		hashtableDelete(u->m_uid2pw, false);
		free(u);
		return NULL;
	}
	u->m_uid2perms->m_keyIgnoreCase = true;
	
	u->m_perms = hashtableNew();
	if (u->m_perms == NULL)
	{
		sqliteconnDelete(conn);
		hashtableDelete(u->m_uid2perms, false);
		hashtableDelete(u->m_uid2pw, false);
		free(u);
		return NULL;
	}
	u->m_perms->m_keyIgnoreCase = true;
	
	if (! sqliteconnOpen(conn))
	{
		usersDelete(u);
		sqliteconnDelete(conn);
		return false;			
	}
	
	if (NULL == (cmd = sqliteconnCreateCommand(conn, "SELECT * FROM USERS;")))
	{
		usersDelete(u);
		sqliteconnDelete(conn);
		return false;
	}

	reader = sqlitecommandExecuteQuery(cmd);
	
	while(sqlitereaderRead(reader))
	{
		if 
		(
			! usersAdd
			(
				u, 
				sqlitereaderText(reader, "UID_NAME"), 
				sqlitereaderText(reader, "PW_TEXT"),
				sqlitereaderInt(reader, "ADMIN_FL") != 0
			)
		)
		{
			sqlitereaderDelete(reader);
			sqlitecommandDelete(cmd);
			sqliteconnClose(conn);
			sqliteconnDelete(conn);
			usersDelete(u);
			
			return NULL;
		}
	}

	sqlitereaderDelete(reader);
	sqlitecommandDelete(cmd);
	
	if (NULL == (cmd = sqliteconnCreateCommand(conn, "SELECT * FROM PERMS;")))
	{
		usersDelete(u);
		sqliteconnDelete(conn);
		return false;
	}

	reader = sqlitecommandExecuteQuery(cmd);
	
	while(sqlitereaderRead(reader))
	{
		if (! usersAddPerm(u, sqlitereaderText(reader, "UID_NAME"), sqlitereaderText(reader, "PERM_NAME")))
		{
			sqlitereaderDelete(reader);
			sqlitecommandDelete(cmd);
			sqliteconnClose(conn);
			sqliteconnDelete(conn);
			usersDelete(u);
			
			return NULL;
		}
	}

	sqlitereaderDelete(reader);
	sqlitecommandDelete(cmd);	
	
	sqliteconnClose(conn);
	sqliteconnDelete(conn);
	
	return u;
}

void usersDelete(users* u)
{
	hashitem* hi;
	int x;
	
	usersValidateMem(u);
	
	hashtableDeleteStringValues(u->m_uid2pw);
	hashtableDelete(u->m_perms, false);
	
	for (x = 0; x < u->m_uid2perms->m_size; x++)
	{
		hi = u->m_uid2perms->m_table[x];
		
		while (hi != NULL)
		{
			hashtableDelete(hi->m_value, false);
			hi->m_value = NULL;
			hi = hi->m_next;
		}
	}
	
	hashtableDelete(u->m_uid2perms, false);
	free(u);
}

bool usersAdd(users* u, const char* uid, const char* pw, bool isadmin)
{
	bool ret;
	sqlitecommand* cmd;
	sqliteconnection* conn = sqliteconnNew(g_dbName);
	
	if (conn == NULL)
	{
		return false;
	}
	
	if (! sqliteconnOpen(conn))
	{
		sqliteconnDelete(conn);
		return false;			
	}
	
	if (hashtableContainsKey(u->m_uid2pw, uid))
	{
		sqliteconnDelete(conn);
		return true;
	}
	
	ret = hashtablePut(u->m_uid2pw, uid, stringNew(pw), false);
	
	if (ret)
	{
		bool insert = false;
		
		if (NULL == (cmd = sqliteconnCreateCommand(conn, "SELECT * FROM USERS WHERE UID_NAME = @UID_NAME;")))
		{
			sqliteconnDelete(conn);
			return false;
		}

		if (sqlitecommandSetParameterText(cmd, "@UID_NAME", uid))
		{
			sqlitereader* reader = sqlitecommandExecuteQuery(cmd);
			insert = ! sqlitereaderRead(reader);
			
			sqlitereaderDelete(reader);
		}

		sqlitecommandDelete(cmd);

		if (insert)
		{
			if (NULL == (cmd = sqliteconnCreateCommand(conn, "INSERT INTO USERS (UID_NAME, PW_TEXT, ADMIN_FL) VALUES (@UID_NAME, @PW_TEXT, @ADMIN_FL)")))
			{
				sqliteconnDelete(conn);
				return false;
			}

			if (sqlitecommandSetParameterText(cmd, "@UID_NAME", uid))
			{
				if (sqlitecommandSetParameterText(cmd, "@PW_TEXT", pw))
				{
					if (sqlitecommandSetParameterInt(cmd, "@ADMIN_FL", isadmin ? 1 : 0))
					{
						ret = sqlitecommandExecuteNonQuery(cmd) > 0;
					}
				}
			}
			sqlitecommandDelete(cmd);
		}		
	}
	
	sqliteconnClose(conn);
	sqliteconnDelete(conn);
	
	return ret;
}

bool usersAddPerm(users* u, const char* uid, const char* perm)
{
	bool insert = false;
	sqlitecommand* cmd;
	sqliteconnection* conn = sqliteconnNew(g_dbName);
	hashtable* ht = hashtableGet(u->m_uid2perms, uid);
	
	if (conn == NULL)
	{
		return false;
	}
	
	if (! sqliteconnOpen(conn))
	{
		sqliteconnDelete(conn);
		return false;			
	}
	
	if (NULL == (cmd = sqliteconnCreateCommand(conn, "SELECT * FROM PERMS WHERE UID_NAME = @UID_NAME AND PERM_NAME = @PERM_NAME;")))
	{
		sqliteconnDelete(conn);
		return false;
	}

	if (sqlitecommandSetParameterText(cmd, "@UID_NAME", uid))
	{
		if (sqlitecommandSetParameterText(cmd, "@PERM_NAME", perm))
		{
			sqlitereader* reader = sqlitecommandExecuteQuery(cmd);
			insert = ! sqlitereaderRead(reader);
			
			sqlitereaderDelete(reader);
		}
	}
	
	sqlitecommandDelete(cmd);

	if (insert)
	{
		if (NULL == (cmd = sqliteconnCreateCommand(conn, "INSERT INTO PERMS (UID_NAME, PERM_NAME) VALUES (@UID_NAME, @PERM_NAME)")))
		{
			sqliteconnDelete(conn);
			return false;
		}
		
		if (sqlitecommandSetParameterText(cmd, "@UID_NAME", uid))
		{
			if (sqlitecommandSetParameterText(cmd, "@PERM_NAME", perm))
			{
				if (sqlitecommandExecuteNonQuery(cmd) <= 0)
				{
					sqlitecommandDelete(cmd);
					sqliteconnDelete(conn);
					return false;
				}
			}
		}
		
		sqlitecommandDelete(cmd);
	}
	sqliteconnClose(conn);
	sqliteconnDelete(conn);	
	
	if (ht == NULL)
	{	
		ht = hashtableGet(u->m_uid2perms, uid);
		
		if (ht == NULL)
		{
			ht = hashtableNew();
			if (ht == NULL)
			{
				return false;
			}
			if (! hashtablePut(u->m_uid2perms, uid, ht, false))
			{
				hashtableDelete(ht, false);
				return false;
			}
		}

		hashtablePut(ht, perm, NULL, false);
		
		if (! hashtableContainsKey(u->m_perms, perm))
		{
			return hashtablePut(u->m_perms, perm, NULL, false);
		}
	}
	
	if (hashtableContainsKey(ht, perm))
	{
		return true;
	}
	
	return hashtablePut(ht, perm, NULL, false);
}

bool userLogon(users* u, string* uid, string* pw)
{
	string* upw = (string *)hashtableGet(u->m_uid2pw, uid->m_cstr);
	if (upw == NULL)
	{
		return false;
	}
	
	stringValidateMem(upw);
	
	return stringIsEqual(upw, pw->m_cstr, true);
}

bool usersHasPerm(users* u, string* uid, string* perm)
{
	hashtable* ht = (hashtable *)hashtableGet(u->m_uid2perms, uid->m_cstr);
	
	if (ht != NULL && hashtableContainsKey(ht, perm->m_cstr))
	{
		return true;
	}
	else
	{
		return usersIsAdmin(u, uid);
	}
}

bool usersIsPerm(users* u, string* perm)
{
	return hashtableContainsKey(u->m_perms, perm->m_cstr);
}

bool usersIsAdmin(users* u, string* uid)
{
	bool isAdmin = false;
	sqlitecommand* cmd;
	sqliteconnection* conn = sqliteconnNew(g_dbName);
	
	if (! sqliteconnOpen(conn))
	{
		sqliteconnDelete(conn);
		return false;			
	}
	
	if (NULL == (cmd = sqliteconnCreateCommand(conn, "SELECT ADMIN_FL FROM USERS WHERE UID_NAME = @UID_NAME;")))
	{
		sqliteconnDelete(conn);
		return false;
	}

	if (sqlitecommandSetParameterText(cmd, "@UID_NAME", uid->m_cstr))
	{
		sqlitereader* reader = sqlitecommandExecuteQuery(cmd);
		if (sqlitereaderRead(reader))
		{
			isAdmin = 0 != sqlitereaderIntAt(reader, 0);
		}
		sqlitereaderDelete(reader);
	}
	
	sqlitecommandDelete(cmd);
	sqliteconnClose(conn);
	sqliteconnDelete(conn);	
	
	return isAdmin;
}

static bool _usersLexMatch(sqllex* lex, const char* cp)
{
	if (! sqllexNext(lex))
	{
		sqllexDelete(lex);
		return false;
	}
	
	if (! stringIsEqual(lex->m_lexum, cp, true))
	{
		sqllexDelete(lex);
		return false;
	}
	
	return true;
}

bool usersParseGrant(users* u, string* system, string* sql)
{
	string* spname;
	string* userId;
	string* stmp;
	string* stmp2;
	bool ret;
	sqllex* lex = sqllexNew(sql);
	
	if (lex == NULL)
	{
		return false;
	}
	
	if (lex->m_token == SQL_EOF)
	{
		sqllexDelete(lex);
		return false;
	}
	
	if (! stringIsEqual(lex->m_lexum, "GRANT", true))
	{
		sqllexDelete(lex);
		return false;
	}
	
	if (! sqllexNext(lex))
	{
		sqllexDelete(lex);
		return false;
	}

	if (stringIsEqual(lex->m_lexum, "EXEC", true))
	{
		if (! sqllexMatch(lex, "EXEC"))
		{
			sqllexDelete(lex);
			return false;
		}
	}
	else if (stringIsEqual(lex->m_lexum, "EXECUTE", true))
	{
		if (! sqllexMatch(lex, "EXECUTE"))
		{
			sqllexDelete(lex);
			return false;
		}
	}
	else
	{
		sqllexDelete(lex);
		return false;
	}	
	
	if (! sqllexMatch(lex, "ON"))
	{
		sqllexDelete(lex);
		return false;
	}
	
	if (NULL == (spname = stringDup(lex->m_lexum)))
	{
		sqllexDelete(lex);
		return false;
	}
	
	if (! sqllexNext(lex))
	{
		sqllexDelete(lex);
		return false;
	}

	if (lex->m_token == SQL_DOT)
	{
		string* s = stringCat(spname, ".", 1);
		stringDelete(spname);
		
		sqllexNext(lex);
		
		spname = stringCat(s, lex->m_lexum->m_cstr, lex->m_lexum->m_len);
		stringDelete(s);

		if (! sqllexMatchToken(lex, SQL_ID))
		{
			stringDelete(s);
			sqllexDelete(lex);
			return false;
		}		
	}

	if (! sqllexMatch(lex, "TO"))
	{
		stringDelete(spname);
		sqllexDelete(lex);
		return false;
	}
		
	if (NULL == (userId = stringDup(lex->m_lexum)))
	{
		stringDelete(spname);
		sqllexDelete(lex);
		return false;
	}

	if (stringIndexOf(spname, ".", 0, false) < 0)
	{
		if (NULL == (stmp = stringCat(system, ".", 1)))
		{
			stringDelete(spname);
			stringDelete(userId);
			sqllexDelete(lex);
			return false;
		}
		
		if (NULL == (stmp2 = stringCat(stmp, spname->m_cstr, spname->m_len)))
		{
			stringDelete(stmp);
			stringDelete(spname);
			stringDelete(userId);
			sqllexDelete(lex);
			return false;
		}
		
		ret = usersAddPerm(u, userId->m_cstr, stmp2->m_cstr);
		
		stringDelete(stmp);
		stringDelete(stmp2);
	}
	else
	{
		ret = usersAddPerm(u, userId->m_cstr, spname->m_cstr);
	}
		
	stringDelete(userId);
	stringDelete(spname);
	sqllexDelete(lex);
	
	return ret;
}

bool usersParseCreateLogon(users* u, string* sql)
{
	bool ret;
	string* userId;
	sqllex* lex = sqllexNew(sql);
	
	if (lex == NULL)
	{
		return false;
	}
	
	if (lex->m_token == SQL_EOF)
	{
		sqllexDelete(lex);
		return false;
	}
	
	if (! stringIsEqual(lex->m_lexum, "CREATE", true))
	{
		sqllexDelete(lex);
		return false;
	}
	
	if (! _usersLexMatch(lex, "LOGON"))
	{
		return false;
	}
	
	if (! sqllexNext(lex))
	{
		sqllexDelete(lex);
		return false;
	}
	
	if (NULL == (userId = stringDup(lex->m_lexum)))
	{
		sqllexDelete(lex);
		return false;		
	}
	
	if (! _usersLexMatch(lex, "PASSWORD"))
	{
		stringDelete(userId);
		return false;
	}
	
	if (! sqllexNext(lex))
	{
		stringDelete(userId);
		sqllexDelete(lex);
		return false;
	}
	
	if (lex->m_token != SQL_STRLIT)
	{
		stringDelete(userId);
		sqllexDelete(lex);
		return false;
	}	

	if (lex->m_lexum->m_cstr[0] == '\'' || lex->m_lexum->m_cstr[0] == '"')
	{
		lex->m_lexum->m_cstr[lex->m_lexum->m_len-1] = '\0';
		ret = usersAdd(u, userId->m_cstr, &lex->m_lexum->m_cstr[1], false);
	}
	else
	{
		ret = usersAdd(u, userId->m_cstr, lex->m_lexum->m_cstr, false);
	}
	
	stringDelete(userId);
	sqllexDelete(lex);
	
	return ret;
}

vector* usersList(users* u)
{
	int x;
	hashitem* hi;
	vector* v = vectorNew();
	
	if (v == NULL)
	{
		return NULL;
	}
	
	for (x = 0; x < u->m_uid2pw->m_size; x++)
	{
		hi = u->m_uid2pw->m_table[x];
		
		while (hi != NULL)
		{
			vectorAdd(v, hi->m_key);
			hi = hi->m_next;
		}
	}
	
	return v;
}

vector* usersListPerms(users* u, const char* userid)
{
	int x;
	hashitem* hi;
	hashtable* ht;
	vector* v = vectorNew();
	
	if (v == NULL)
	{
		return NULL;
	}
	
	ht = hashtableGet(u->m_uid2perms, userid);
	
	if (ht == NULL)
	{
		return v;
	}
	
	for (x = 0; x < ht->m_size; x++)
	{
		hi = ht->m_table[x];
		
		while (hi != NULL)
		{
			vectorAdd(v, hi->m_key);
			hi = hi->m_next;
		}
	}
	
	return v;
}

#ifdef DEBUG
void usersValidateMem(users* u)
{
	hashitem* hi;
	int x;
	
	ASSERT_MEM(u, sizeof(users));
	hashtableValidateMem(u->m_uid2pw);
	hashtableValidateMem(u->m_uid2perms);
	
	for (x = 0; x < u->m_uid2perms->m_size; x++)
	{
		hi = u->m_uid2perms->m_table[x];
		
		while (hi != NULL)
		{
			hashtableValidateMem(hi->m_value);
			hi = hi->m_next;
		}
	}	
}
#endif
