#include <spl/types.h>
#include <spl/debug.h>

#include <stdio.h>
#include <math.h>

/*#include <spl/Log.h>
#include <spl/UnitTest.h>*/
#include <spl/commandline.h>
#include <spl/datetime.h>
#include <spl/io/file.h>
#include <spl/integer.h>
#include <spl/net/serversocket.h>
#include <spl/stringbuffer.h>

#include "conf.h"
#include "httpget.h"
#include "request.h"
#include "stats.h"
#include "stmtmgr.h"
#include "users.h"

#ifdef __TANDEM
#include <sql.h>
#endif

extern bool serviceRequest(users* u, request *req, stringbuffer* xml);
extern void setUserPerms(users* u);

static void shutdownRequest(request* req, stringbuffer* xml);
static bool echoRequest(request* req, stringbuffer* xml);
static void infodefinesRequest(request* req, stringbuffer* xml);
static void invokeRequest(request* req, stringbuffer* xml);
static void syslistRequest(request* req, stringbuffer* xml);
static void proclistRequest(stmtmgr* dsqlmgr, request* req, stringbuffer* xml);
static void userlistRequest(stmtmgr* dsqlmgr, request* req, stringbuffer* xml);
static void userlistPermsRequest(stmtmgr* dsqlmgr, request* req, stringbuffer* xml);
static void proccodeRequest(stmtmgr* dsqlmgr, request* req, stringbuffer* xml);
static void sqlRequest(stmtmgr* dsqlmgr, request* req, stringbuffer* xml);
static bool executeDynamic(stmtmgr* dsqlmgr, request* req, stringbuffer* xml);

bool g_running = false;
datetime g_dtminception;
byte g_buf[1024];
static fd_set g_read;
static fd_set g_excpt;		
static struct timeval g_to;
msgline g_status;

bool serviceRequest(users* u, request *req, stringbuffer* xml)
{
	return false;
}

bool _writeError(stringbuffer* xml, const char* cp1, const char* cp2)
{
	bool ret = stringbufferAppend(xml, "<error>") &&
		stringbufferAppend(xml, cp1) &&
		stringbufferAppend(xml, cp2) &&
		stringbufferAppend(xml, "</error>\r\n");
	
	stringbufferValidateMem(xml);
	
	StrCpyLen(g_status.msg, cp1, (int)sizeof(g_status.msg));
	
	ASSERT(g_status.chkbtye == 'C');
	
	return ret;
}

bool _writeErrorCode(stringbuffer* xml, const char* cp1, const int i)
{
	bool ret;
	
	sprintf((char *)g_buf, "%d", i);
	
	ret = stringbufferAppend(xml, "<error>") &&
		stringbufferAppend(xml, cp1) &&
		stringbufferAppendCh(xml, ' ') &&
		stringbufferAppend(xml, (char *)g_buf) &&
		stringbufferAppend(xml, "</error>\r\n");

	stringbufferValidateMem(xml);

	StrCpyLen(g_status.msg, g_buf, (int)sizeof(g_status.msg));
	
	ASSERT(g_status.chkbtye == 'C');
		
	return ret;
}

static int doselect(socketBase* sock)
{
	FD_ZERO(&g_read);
	FD_ZERO(&g_excpt);
	
	FD_SET(sock->m_fd, &g_read);
	FD_SET(sock->m_fd, &g_excpt);
	
	return select(1, &g_read, NULL, &g_excpt, &g_to);
}

static bool readRequest(socketBase* sock, int size, stringbuffer* sbuf)
{
	int count;
	int read = 0;
	int readBytes;
	
	if (! socketSetNonBlocking(sock))
	{
		return false;
	}
	
	while (read < size)
	{
		if (socketGetBytesAvail(sock) <= 0)
		{
			count = doselect(sock);
			
			if (count <= 0)
			{
				return false;
			}
			
			if (FD_ISSET(sock->m_fd, &g_excpt))
			{
				return false;
			}
		}
		
		readBytes = socketRecv(sock, g_buf, 0, sizeof(g_buf));
		if (readBytes <= 0)
		{
			return false;
		}
		
		read += readBytes;
		
		if (! stringbufferAppendBuf(sbuf, g_buf, readBytes))
		{
			return false;
		}
		
		if (read == size)
		{
			return true;
		}
	}
	
	printf("read excess bytes, expected %d read %d\n", size, read);
	
	return false;
}

static string* _callWeb(const char* server, const char* cmd, char env)
{
	int pos;
	string* hbod;
	char* val;
	string* ret;
	stringbuffer* buf = stringbufferNew();
	
	if (NULL == buf)
	{
		return NULL;
	}
	
	stringbufferAppend(buf, "/TandemDataBase/SqlsInterface.aspx?cmd=");
	stringbufferAppend(buf, cmd);
	stringbufferAppend(buf, "&env=");
	stringbufferAppendCh(buf, env);
	
	hbod = httpget(server, 80, buf->m_buf);
	
	stringbufferDelete(buf);
	
	if (NULL == hbod)
	{
		printf("httpget returned NULL\n");
		return NULL;
	}
	
	pos = stringIndexOf(hbod, "\r\n\r\n", 0, false);
	
	val = &hbod->m_cstr[pos+4];
	
	if (0 < (pos = IndexOfCh(val, '\r', 0)))
	{
		val[pos] = '\0';
	}
	
	ret = stringNew(val);
	
	stringDelete(hbod);
		
	return ret;
}

static int _getPort(const char* server, char env)
{
	int port = -1;
	string* val = _callWeb(server, "port", env);
	
	if (val->m_cstr[0] == 'O' && val->m_cstr[1] == 'K')
	{
		char *i = &val->m_cstr[3];
		port = int32Parse(i, (int)strlen(i), 10);
	}
	else
	{
		printf("%c port returned %s\n", env, val->m_cstr);
	}
	
	stringDelete(val);
	
	return port;
}

static bool _sendInner(const char* server, const char* cmd, char env, string** raw, int* pos, int* posnext)
{
	*posnext = 0;
	
	*raw = _callWeb(server, cmd, env);
	*pos = 0;
	
	if (NULL == *raw)
	{
		printf("Error loading %s NULL\n", cmd);
		return false;
	}
	
	if ((*raw)->m_cstr[0] != 'O' || (*raw)->m_cstr[1] != 'K')
	{
		printf("Error loading %s %s\n", cmd, (*raw)->m_cstr);
		stringDelete(*raw);
		return false;
	}
	
	*pos = 3;
	
	return true;
}

static bool _setUserPerms(const char* server, char env, users* u)
{
	string* raw;
	int pos;
	int posnext = 0;
	bool usersComplete = false;
	char delim2 = '\t';
	
	if (! _sendInner(server, "users", env, &raw, &pos, &posnext))
	{
		return false;
	}
	
	while (pos < raw->m_len)
	{
		char* start = &raw->m_cstr[pos];
		char* pwstart;
		
		if (0 > (posnext = stringIndexOfCh(raw, '\t', pos)))
		{
			break;
		}
		raw->m_cstr[posnext] = '\0';		
		pos = posnext + 1;
		
		pwstart = &raw->m_cstr[pos];
		
		if (0 > (posnext = stringIndexOfCh(raw, delim2, pos)))
		{
			break;
		}
		raw->m_cstr[posnext] = '\0';		
		pos = posnext + 1;
		
		if (usersComplete)
		{
			printf("setting perm %s %s\n", start, pwstart);
			
			usersAddPerm(u, start, pwstart);
		}
		else
		{
			printf("adding user %s %s ADMIN=%c\n", start, pwstart, raw->m_cstr[pos]);
			
			usersAdd(u, start, pwstart, raw->m_cstr[pos] == 'Y');
			pos += 2;
		}

		if (!usersComplete && pos >= raw->m_len)
		{
			usersComplete = true;
			delim2 = '\n';
			if (! _sendInner(server, "perms", env, &raw, &pos, &posnext))
			{
				return false;
			}			
		}
	}
	
	stringDelete(raw);
	
	return true;
}

int main(int argc, const char **argv)
{
	int sendlen = 0;
	stringbuffer* sbuf;
	serversocket* ss;
	socketBase* client;
	stmtmgr* dsqlmgr;
	commandline* cl;
	int port;
	bool dumpoutput = false;
	char windowsServer[20];
	
#ifdef __TANDEM
	EXEC SQL BEGIN DECLARE SECTION;
	
	short sqlcode;
	char ENV_CODE;
	
	EXEC SQL END DECLARE SECTION;
#else
	char ENV_CODE = 'T';
#endif

#if defined(_WIN32) || defined(_WIN64)
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;	
	
	wVersionRequested = MAKEWORD(2, 2);
	 
	err = WSAStartup(wVersionRequested, &wsaData);
	if ( err != 0 ) 
	{
		printf("wsa startup failed\n");
		abort();
	}
#endif

	g_status.chkbtye = 'C';
	
	StrCpyLen(windowsServer, "192.209.36.133", sizeof(windowsServer));

	if (NULL == (cl = commandlineNew(argc, argv)))
	{
		printf("Error parsing the commandline\n");
		abort();
	}

	if (commandlineHasSwitch(cl, "?") || commandlineHasSwitch(cl, "help"))
	{
		printf("USAGE: %s [--port=8081 | --env=[DEV|TEST|DEMO|PROD]] [--dumpxml] \n", __FILE__);
		commandlineDelete(cl);
		return 0;
	}

	dumpoutput = commandlineHasSwitch(cl, "dumpxml");

	if (commandlineHasSwitch(cl, "server"))
	{
		string* srv = commandlineGetSwitch(cl, "server");
		StrCpyLen(windowsServer, srv->m_cstr, sizeof(windowsServer));
	}

#ifdef __TANDEM
	EXEC SQL 
		SELECT
			ENV_CODE
		INTO
			:ENV_CODE
		FROM
			=ENV;	
#endif

	if (commandlineHasSwitch(cl, "port"))
	{
		string* sport = commandlineGetSwitch(cl, "port");
		port = int32Parse(sport->m_cstr, sport->m_len, 10);
	}
	else
	{
		if (0 > (port = _getPort(windowsServer, ENV_CODE)))
		{
			printf("Get port from service failed\n");
			abort();
		}
	}
	
	g_to.tv_sec = 5;
	g_to.tv_usec = 0;

	if (! createProcedureTable())
	{
		commandlineDelete(cl);
		printf("error creating procedure tables.\n");
		abort();
	}
	
	dsqlmgr = stmtmgrNew();
	if (dsqlmgr == NULL)
	{
		printf("out of memory creating stmtmgr\n");
		commandlineDelete(cl);
		abort();
	}
	
	ss = serversocketNew(port, 64);
	if (ss == NULL)
	{
		printf("create server socket failed\n");
		stmtmgrDelete(dsqlmgr);
		commandlineDelete(cl);
		abort();
	}
	
	statsInit();

	if (! _setUserPerms(windowsServer, ENV_CODE, dsqlmgr->m_users))
	{
		stmtmgrDelete(dsqlmgr);
		commandlineDelete(cl);
		abort();	
	}

	datetimeInit(&g_dtminception);
	datetimeToStringBuf(&g_dtminception, (char *)g_buf, sizeof(g_buf));
	printf("Inception %s on port %d\n", g_buf, port);
	
	g_running = true;
	
	while (g_running && (NULL != (client = serversocketAccept(ss))))
	{
		int b1, b2, size, endian;
		
		string* rip = socketGetRemoteIp(client);
		
		StrCpyLen(g_status.ip, rip->m_cstr, sizeof(g_status.ip));
		stringDelete(rip);
		
		g_status.msg[0] = '\0';
		g_status.procName[0] = '\0';
		g_status.userId[0] = '\0';
		
#ifndef DEBUG
		if (! socketSetRecvTimeout(client, 1000 * 3))
		{
			printf("set timeout failed\n");
		}
#endif

		if (! socketSetLingerOn(client))
		{
			printf("set linger on failed\n");
		}
		
		if (! socketSetBlocking(client))
		{
			printf("set blocking failed\n");
		}
		
		endian = socketRecvByte(client);
		if (endian < 0)
		{
			printf("recv error on endian\n");
		}

		b1 = socketRecvByte(client);
		if (b1 < 0)
		{
			printf("recv error on b1 %d %d\n", (int)b1, client->m_errorStatus);
		}

		b2 = socketRecvByte(client);
		if (b2 < 0)
		{
			printf("recv error on b2 %d %d\n", (int)b2, client->m_errorStatus);
		}
		
		if (63 != socketRecvByte(client))
		{
			printf("recv expected 63 after b2\n");
		}
		
		size = b2 | (b1 << 8);
		
		statsBeginCall(size);
				
		if (NULL == (sbuf = stringbufferSizeNew(4096)))
		{
			printf("out of memory\n");
			StrCpyLen(g_status.msg, "out of memory", sizeof(g_status.msg));			
		}
		else
		{
			if (! readRequest(client, size - 4, sbuf))
			{
				printf("error reading request\n");
				StrCpyLen(g_status.msg, "error reading requesst", sizeof(g_status.msg));			
			}
			else
			{
				request* req = requestNew(sbuf);
				if (req == NULL)
				{
					_writeError(sbuf, "request parse failed", "");
					StrCpyLen(g_status.msg, "parse request failed", sizeof(g_status.msg));			
				}
				else
				{
					bool logonOk = false;
					stringbufferClear(sbuf);
					stringbufferAppend(sbuf, "<?xml version='1.0'?>\r\n<ROWS>\r\n");
					
					g_status.bytesRead = req->m_bytes;
					StrCpyLen(g_status.procName, req->m_name->m_cstr, sizeof(g_status.procName));
					StrCpyLen(g_status.userId, req->m_userId->m_cstr, sizeof(g_status.userId));
					
					if (stringIsEqual(req->m_name, "ADMIN", true))
					{
						logonOk = true;
					}
					else if (userLogon(dsqlmgr->m_users, req->m_userId, req->m_password))
					{
						logonOk = true;
					}
					if (! logonOk)
					{
						_writeError(sbuf, "logon failed", "");
						stringbufferAppend(sbuf, "</ROWS>\r\n");
						StrCpyLen(g_status.msg, "logon failed ", sizeof(g_status.msg));			
						StrCatLen(g_status.msg, req->m_userId->m_cstr, sizeof(g_status.msg));
						
						ASSERT(g_status.chkbtye == 'C');
					}
					else
					{
						if (stringIsEqual(req->m_name, "SQL", true))
						{
							sqlRequest(dsqlmgr, req, sbuf);
						}
						else if (stringIsEqual(req->m_name, "ADMIN", true))
						{
							requestItem* item;
							
							if (! requestHasItem(req, "COMMAND"))
							{
								_writeError(sbuf, "expected COMMAND parameter", "");
							}
							
							item = requestGetItem(req, "COMMAND");
							
							StrCatLen(g_status.procName, "-", sizeof(g_status.procName));
							StrCatLen(g_status.procName, item->m_data.m_str->m_cstr, sizeof(g_status.procName));
							
							if (stringIsEqual(item->m_data.m_str, "stats", true))
							{
								statsWriteRecord(sbuf);
							}
							else if (stringIsEqual(item->m_data.m_str, "ping", true))
							{
								echoRequest(req, sbuf);
							}
							else if (stringIsEqual(item->m_data.m_str, "shutdown", true))
							{
								shutdownRequest(req, sbuf);
							}
							else if (stringIsEqual(item->m_data.m_str, "info_defines", true))
							{
								infodefinesRequest(req, sbuf);
							}
							else if (stringIsEqual(item->m_data.m_str, "invoke", true))
							{
								invokeRequest(req, sbuf);
							}
							else if (stringIsEqual(item->m_data.m_str, "systemslist", true))
							{
								syslistRequest(req, sbuf);
							}
							else if (stringIsEqual(item->m_data.m_str, "proclist", true))
							{
								proclistRequest(dsqlmgr, req, sbuf);
							}
							else if (stringIsEqual(item->m_data.m_str, "userlist", true))
							{
								userlistRequest(dsqlmgr, req, sbuf);
							}
							else if (stringIsEqual(item->m_data.m_str, "userperms", true))
							{
								userlistPermsRequest(dsqlmgr, req, sbuf);
							}
							else if (stringIsEqual(item->m_data.m_str, "displaysql", true))
							{
								proccodeRequest(dsqlmgr, req, sbuf);
							}
							else
							{
								_writeError(sbuf, "unknown admin command ", item->m_data.m_str->m_cstr);
								StrCpyLen(g_status.msg, "unknown admin command ", sizeof(g_status.msg));
								StrCatLen(g_status.msg, item->m_data.m_str->m_cstr, sizeof(g_status.msg));
							}
						}
						else if (executeDynamic(dsqlmgr, req, sbuf))
						{
							/* fin */
						}
						else
						{
							if (! serviceRequest(dsqlmgr->m_users, req, sbuf))
							{
								_writeError(sbuf, "unknown procedure ", req->m_name->m_cstr);
								StrCpyLen(g_status.msg, "unknown procedure", sizeof(g_status.msg));
							}
						}
						
						stringbufferAppend(sbuf, "</ROWS>\r\n");
					}
					requestDelete(req);
				}
				
				sendlen = sbuf->m_used;
				
				socketSetBlocking(client);
				
				if (! socketSendChars(client, sbuf->m_buf))
				{
					printf("socketSendChars failed\n");
				}
				
				if (dumpoutput)
				{
					printf("\n%s\n\n", sbuf->m_buf);
				}
				
				g_status.bytesSent = sbuf->m_used;
				if (g_status.msg[0] == '\0')
				{
					if (socketGetErrorCode(client) == 0)
					{
						g_status.msg[0] = 'O';
						g_status.msg[1] = 'K';
						g_status.msg[2] = '\0';
					}
					else
					{
						sprintf(g_status.msg, "%d", socketGetErrorCode(client));
					}
				}
				
				datetimeInit(&g_dtminception);
				datetimeToStringBuf(&g_dtminception, (char *)g_buf, sizeof(g_buf));
	
				ASSERT(g_status.chkbtye == 'C');
	
				printf("%s %s %s %s %d %d %s\n", g_buf, g_status.ip, g_status.userId, g_status.procName, g_status.bytesRead, g_status.bytesSent, g_status.msg);
			}
						
			stringbufferDelete(sbuf);
		}
		
		socketClose(client);
		socketDelete(client);
		
		statsEndCall(sendlen);
	}
	
	commandlineDelete(cl);
	stmtmgrDelete(dsqlmgr);
	
#ifdef DEBUG
	_debugValidateHeap();
#endif

	serversocketClose(ss);
	serversocketDelete(ss);
	
	datetimeInit(&g_dtminception);
	datetimeToStringBuf(&g_dtminception, (char *)g_buf, sizeof(g_buf));
	printf("Shutdown at %s\n", g_buf);
	
	DEBUG_DUMP_MEM_LEAKS();
	ASSERT_MEM_FREE();
	DEBUG_TEAR_DOWN(true);
	
#if defined(_WIN32) || defined(_WIN64)
	WSACleanup();
#endif
	
	return 0;
}

static bool logonAdminUser(request* req, stringbuffer* xml)
{
	/* NOTE: password is checked by caller */
	
	if (! stringIsEqual(req->m_userId, "sa", true))
	{
		StrCpyLen(g_status.msg, "admin logon failed", sizeof(g_status.msg));
		_writeError(xml, "logon denied ", req->m_userId->m_cstr);
		return false;
	}
	
	return true;
}

static void syslistRequest(request* req, stringbuffer* xml)
{
	int x;
	vector* syss = loadAllSystems();
	
	for (x = 0; x < vectorCount(syss); x++)
	{
		if (! stringbufferAppend(xml, "<row system='"))
		{
			StrCpyLen(g_status.msg, "out of memory", sizeof(g_status.msg));
			_writeError(xml, "out of memory", NULL);
			break;
		}
		
		if (! stringbufferAppend(xml, ((string*)vectorElementAt(syss, x))->m_cstr))
		{
			StrCpyLen(g_status.msg, "out of memory", sizeof(g_status.msg));
			_writeError(xml, "'/>out of memory", NULL);
			break;
		}

		if (! stringbufferAppend(xml, "' />\r\n"))
		{
			StrCpyLen(g_status.msg, "out of memory", sizeof(g_status.msg));
			_writeError(xml, "out of memory", NULL);
			break;
		}
	}
	
	vectorDeleteStrings(syss);
}

static void proclistRequest(stmtmgr* dsqlmgr, request* req, stringbuffer* xml)
{
	int x;
	vector* procs = stmtmgrGetProcedures(dsqlmgr);
	
	for (x = 0; x < vectorCount(procs); x++)
	{
		preparedstatement* stmt = (preparedstatement*)vectorElementAt(procs, x);
		
		if (! stringbufferAppend(xml, "<row system='"))
		{
			StrCpyLen(g_status.msg, "out of memory", sizeof(g_status.msg));
			_writeError(xml, "out of memory", NULL);
			break;
		}
		
		if (! stringbufferAppend(xml, stmt->m_system->m_cstr))
		{
			StrCpyLen(g_status.msg, "out of memory", sizeof(g_status.msg));
			_writeError(xml, "'/>out of memory", NULL);
			break;
		}

		if (! stringbufferAppend(xml, "' procedure='"))
		{
			StrCpyLen(g_status.msg, "out of memory", sizeof(g_status.msg));
			_writeError(xml, "out of memory", NULL);
			break;
		}
		
		if (! stringbufferAppend(xml, stmt->m_name->m_cstr))
		{
			StrCpyLen(g_status.msg, "out of memory", sizeof(g_status.msg));
			_writeError(xml, "'/>out of memory", NULL);
			break;
		}

		if (! stringbufferAppend(xml, "' />\r\n"))
		{
			StrCpyLen(g_status.msg, "out of memory", sizeof(g_status.msg));
			_writeError(xml, "out of memory", NULL);
			break;
		}
	}
	
	vectorDelete(procs);
}

static void userlistRequest(stmtmgr* dsqlmgr, request* req, stringbuffer* xml)
{
	int x;
	vector* users;
	
	if (! logonAdminUser(req, xml))
	{
		_writeError(xml, "unauthorized", "");
		return;
	}
	
	users = usersList(dsqlmgr->m_users);
	
	if (users == NULL)
	{
		StrCpyLen(g_status.msg, "error listing users", sizeof(g_status.msg));
		_writeError(xml, "internal error", NULL);
		return;
	}
	
	for (x = 0; x < vectorCount(users); x++)
	{
		stringValidateMem(vectorElementAt(users, x));
		
		if (! stringbufferAppend(xml, "<row userid='"))
		{
			StrCpyLen(g_status.msg, "out of memory", sizeof(g_status.msg));
			_writeError(xml, "out of memory", NULL);
			break;
		}
		
		if (! stringbufferAppend(xml, ((string *)vectorElementAt(users, x))->m_cstr))
		{
			StrCpyLen(g_status.msg, "out of memory", sizeof(g_status.msg));
			_writeError(xml, "'/>out of memory", NULL);
			break;
		}

		if (! stringbufferAppend(xml, "' />\r\n"))
		{
			StrCpyLen(g_status.msg, "out of memory", sizeof(g_status.msg));
			_writeError(xml, "out of memory", NULL);
			break;
		}
	}
	
	vectorDelete(users);
}

static void userlistPermsRequest(stmtmgr* dsqlmgr, request* req, stringbuffer* xml)
{
	int x;
	vector* perms;
	requestItem* item;

	if (! logonAdminUser(req, xml))
	{
		_writeError(xml, "unauthorized", "");
		return;
	}
	
	if (NULL == (item = requestGetItem(req, "userid")))
	{
		StrCpyLen(g_status.msg, "missing parameter", sizeof(g_status.msg));
		_writeError(xml, "parameter not found ", "userid");
		return;
	}

	if (item->m_type != 'S')
	{
		StrCpyLen(g_status.msg, "userid type error", sizeof(g_status.msg));
		_writeError(xml, "userid must be a string", "");
		return;
	}

	perms = usersListPerms(dsqlmgr->m_users, item->m_data.m_str->m_cstr);
	
	if (perms == NULL)
	{
		StrCpyLen(g_status.msg, "error listing perms", sizeof(g_status.msg));
		_writeError(xml, "internal error", "");
		return;
	}
	
	for (x = 0; x < vectorCount(perms); x++)
	{
		stringValidateMem(vectorElementAt(perms, x));
		
		if (! stringbufferAppend(xml, "<row perm='"))
		{
			StrCpyLen(g_status.msg, "out of memory", sizeof(g_status.msg));
			_writeError(xml, "out of memory", NULL);
			break;
		}
		
		if (! stringbufferAppend(xml, ((string *)vectorElementAt(perms, x))->m_cstr))
		{
			StrCpyLen(g_status.msg, "out of memory", sizeof(g_status.msg));
			_writeError(xml, "'/>out of memory", NULL);
			break;
		}

		if (! stringbufferAppend(xml, "' />\r\n"))
		{
			StrCpyLen(g_status.msg, "out of memory", sizeof(g_status.msg));
			_writeError(xml, "out of memory", NULL);
			break;
		}
	}
	
	vectorDelete(perms);
}

static void proccodeRequest(stmtmgr* dsqlmgr, request* req, stringbuffer* xml)
{
	requestItem* item;
	preparedstatement* stmt;
	string* system;
	string* name;

	if (NULL == (item = requestGetItem(req, "system")))
	{
		StrCpyLen(g_status.msg, "missing parameter system", sizeof(g_status.msg));
		_writeError(xml, "parameter not found ", "system");
		return;
	}

	system = item->m_data.m_str;

	if (NULL == (item = requestGetItem(req, "procedure")))
	{
		StrCpyLen(g_status.msg, "missing parameter procedure", sizeof(g_status.msg));
		_writeError(xml, "parameter not found ", "procedure");
		return;
	}

	name = item->m_data.m_str;
	
	if (NULL == (stmt = stmtmgrGetProcedure(dsqlmgr, system, name)))
	{
		_writeError(xml, "procedure not found ", name->m_cstr);
		return;
	}
	
	if (! stringbufferAppend(xml, "<row sql='"))
	{
		StrCpyLen(g_status.msg, "out of memory", sizeof(g_status.msg));
		_writeError(xml, "out of memory", NULL);
		return;
	}
	
	if (! stringbufferAppendXmlEncode(xml, stmt->m_procDef->m_text->m_cstr, true))
	{
		StrCpyLen(g_status.msg, "out of memory", sizeof(g_status.msg));
		_writeError(xml, "'/>out of memory", NULL);
		return;
	}

	if (! stringbufferAppend(xml, "' />\r\n"))
	{
		StrCpyLen(g_status.msg, "out of memory", sizeof(g_status.msg));
		_writeError(xml, "out of memory", NULL);
		return;
	}
}

static void invokeRequest(request* req, stringbuffer* xml)
{
	string* sys;
	string* sql;
	stringbuffer* buf;
	requestItem* item;
	preparedstatement* stmt;
	int errorcode;

	if (NULL == (buf = stringbufferNew()))
	{
		StrCpyLen(g_status.msg, "out of memory", sizeof(g_status.msg));
		_writeError(xml, "out of memory", NULL);
		return;
	}

	if (NULL == (item = requestGetItem(req, "define")))
	{
		stringbufferDelete(buf);
		StrCpyLen(g_status.msg, "missing parameter", sizeof(g_status.msg));
		_writeError(xml, "parameter not found ", "define");
		return;
	}

	stringbufferAppend(buf, "CREATE CURSOR TMP () SELECT * FROM ");
	stringbufferAppend(buf, item->m_data.m_str->m_cstr);
	stringbufferAppend(buf, " WHERE 1 = 0 FOR BROWSE ACCESS");

	sql = stringbufferToString(buf);
	
	stringbufferDelete(buf);

	if (sql == NULL)
	{
		StrCpyLen(g_status.msg, "out of memory", sizeof(g_status.msg));
		_writeError(xml, "out of memory", NULL);
		return;
	}
	
	if (NULL == (sys = stringNew("*")))
	{
		StrCpyLen(g_status.msg, "out of memory", sizeof(g_status.msg));
		_writeError(xml, "out of memory", NULL);
		return;
	}

	stmt = preparedstatementNew(sys, sql, -1, &errorcode);

	stringDelete(sys);
	stringDelete(sql);
	
	if (stmt == NULL)
	{
		StrCpyLen(g_status.msg, "invoke internal error", sizeof(g_status.msg));
		_writeErrorCode(xml, "invoke internal error", errorcode);
		return;
	}
	
	preparedstatementValidateMem(stmt);
	
	preparedstatementExec(stmt, req, xml);
	
	preparedstatementDelete(stmt);
}

static void infodefinesRequest(request* req, stringbuffer* xml)
{
	string* txt = fileLoadText("defines.xml");
	
	if (txt == NULL)
	{
		StrCpyLen(g_status.msg, "error loading defines.xml", sizeof(g_status.msg));
		_writeError(xml, "error loading defines.xml", req->m_userId->m_cstr);
		return;	
	}
	
	if (! stringbufferAppendString(xml, txt))
	{
		StrCpyLen(g_status.msg, "out of memory", sizeof(g_status.msg));
		_writeError(xml, "'/>out of memory", NULL);
	}
	
	stringDelete(txt);
}

static void shutdownRequest(request* req, stringbuffer* xml)
{
	if (logonAdminUser(req, xml))
	{
		g_running = false;
		stringbufferAppend(xml, "<row message='shuting down' />\r\n");
	}
}

static bool echoRequest(request* req, stringbuffer* xml)
{
	requestItem* item;
	
	if (! stringbufferAppend(xml, "<row data='"))
	{
		return false;
	}
	
	if (! stringbufferAppend(xml, req->m_name->m_cstr))
	{
		return false;
	}
	
	if (! stringbufferAppend(xml, "' />\r\n"))
	{
		return false;
	}
	
	item = req->m_items;
	while (item->m_name != NULL)
	{
		if (! stringbufferAppend(xml, "<row "))
		{
			return false;
		}
		if (! stringbufferAppend(xml, item->m_name->m_cstr))
		{
			return false;
		}
		if (! stringbufferAppend(xml, "='"))
		{
			return false;
		}
		if (item->m_type == 'S')
		{
			if (! stringbufferAppend(xml, item->m_data.m_str->m_cstr))
			{
				return false;
			}
		}
		else if (item->m_type == 'd')
		{
			if (! stringbufferAppendReal(xml, item->m_data.m_real))
			{
				return false;
			}
		}
		else if (item->m_type == 'L')
		{
			if (! stringbufferAppendLong(xml, item->m_data.m_long))
			{
				return false;
			}
		}
		else
		{
			if (! stringbufferAppendInt(xml, item->m_data.m_int))
			{
				return false;
			}
		}
		if (! stringbufferAppend(xml, "' />\r\n"))
		{
			return false;
		}
		
		item = item->m_next;
	}
	
	return true;
}

static void sqlRequest(stmtmgr* dsqlmgr, request* req, stringbuffer* xml)
{
	requestItem* sql;
	requestItem* sys;
	
	if (! logonAdminUser(req, xml))
	{
		return;
	}

	if (NULL == (sql = requestGetItem(req, "sql")))
	{
		StrCpyLen(g_status.msg, "expected parameter sql", sizeof(g_status.msg));
		_writeError(xml, "expected parameter sql", "");
		return;
	}
	if (sql->m_type != 'S')
	{
		StrCpyLen(g_status.msg, "sql parameter must be a string", sizeof(g_status.msg));
		_writeError(xml, "sql parameter must be a string", "");
	}
	
	if (NULL == (sys = requestGetItem(req, "system")))
	{
		StrCpyLen(g_status.msg, "expected parameter system", sizeof(g_status.msg));
		_writeError(xml, "expected parameter system", "");
		return;
	}
	if (sys->m_type != 'S')
	{
		StrCpyLen(g_status.msg, "system paramter must be a string", sizeof(g_status.msg));
		_writeError(xml, "system parameter must be a string", "");
	}

	stmtmgrProcessSql
	(
		dsqlmgr, 
		sys->m_data.m_str, 
		req->m_userId, 
		sql->m_data.m_str, 
		req->m_transid,
		xml
	);
}

static bool executeDynamic(stmtmgr* dsqlmgr, request* req, stringbuffer* xml)
{
	requestItem* sys;
	
	if (NULL == (sys = requestGetItem(req, "%%system%%")))
	{
		StrCpyLen(g_status.msg, "expected parameter %%system%%", sizeof(g_status.msg));
		_writeError(xml, "expected parameter system", "");
		return true;
	}
	if (sys->m_type != 'S')
	{
		StrCpyLen(g_status.msg, "system parameter must be a string", sizeof(g_status.msg));
		_writeError(xml, "system parameter must be a string", "");
		return true;
	}
	
	return stmtmgrExec(dsqlmgr, sys->m_data.m_str, req, xml);
}
