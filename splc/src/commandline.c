#include <spl/commandline.h>

commandline* commandlineNew(const int argc, const char** argv)
{
	int x;
	char buf[255];
	commandline* cl = (commandline *)malloc(sizeof(commandline));
	
	if (cl == NULL)
	{
		return NULL;
	}
	
	cl->m_argc = argc;
	cl->m_argv = argv;
	
	if (NULL == (cl->m_args = vectorNew()))
	{
		free(cl);
		return NULL;
	}
	
	if (NULL == (cl->m_switches = hashtableNew()))
	{
		vectorDelete(cl->m_args);
		free(cl);
		return NULL;
	}
	
	for (x = 0; x < argc; x++)
	{
		if (argv[x][0] == '-')
		{
			int pos = 1;
			int bufpos = 0;
			string* key;
			string* val = NULL;
			
			if (argv[x][1] == '-')
			{
				pos++;
			}
			
			while (argv[x][pos] != '\0' && argv[x][pos] != '=' && pos < 254)
			{
				buf[bufpos++] = argv[x][pos++];
			}
			buf[bufpos] = '\0';
			
			if (NULL == (key = stringNew(buf)))
			{
				commandlineDelete(cl);
				return NULL;
			}
			
			if (argv[x][pos] == '=')
			{
				pos++;
				bufpos = 0;
				while (argv[x][pos] != '\0' && pos < 254)
				{
					buf[bufpos++] = argv[x][pos++];
				}
				buf[bufpos] = '\0';
				
				if (NULL == (val = stringNew(buf)))
				{
					stringDelete(key);
					commandlineDelete(cl);
					return NULL;			
				}
			}
			else
			{
				if (NULL == (val = stringNew("")))
				{
					stringDelete(key);
					commandlineDelete(cl);
					return NULL;			
				}				
			}
			hashtablePut(cl->m_switches, key->m_cstr, val, false);
			stringDelete(key);				
		}
		else
		{
			if (! vectorAdd(cl->m_args, stringNew(argv[x])))
			{
				commandlineDelete(cl);
				return NULL;
			}
		}
	}	
	
	return cl;
}

void commandlineDelete(commandline* cl)
{
	int x;
	
	for (x = 0; x < vectorCount(cl->m_args); x++)
	{
		stringDelete((string*)vectorElementAt(cl->m_args, x));
	}
	
	vectorDelete(cl->m_args);
	
	for (x = 0; x < cl->m_switches->m_size; x++)
	{
		if (cl->m_switches->m_table[x] != NULL)
		{
			hashitem* item = cl->m_switches->m_table[x];
			while (item != NULL)
			{
				if (item->m_value != NULL)
				{
					stringDelete((string *)item->m_value);
					item->m_value = NULL;
				}
				item = item->m_next;
			}	
		}
	}
	
	hashtableDelete(cl->m_switches, false);
	free(cl);
}

bool commandlineHasSwitch(commandline* cl, const char* cp)
{
	return hashtableContainsKey(cl->m_switches, cp);
}

string* commandlineGetSwitch(commandline* cl, const char* cp)
{
	return hashtableGet(cl->m_switches, cp);
}

int commandlineArgCount(commandline* cl)
{
	return vectorCount(cl->m_args);
}

string* commandlineArgAt(commandline* cl, int idx)
{
	return (string *)vectorElementAt(cl->m_args, idx);
}

#ifdef DEBUG
void commandlineValidateMem(commandline* cl)
{
	ASSERT_MEM(cl, sizeof(commandline));
	vectorValidateMem(cl->m_args);
	hashtableValidateMem(cl->m_switches);
}
#endif
