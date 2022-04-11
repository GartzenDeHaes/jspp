#ifndef _conf_h
#define _conf_h

#define TEST_PORT 8082
#define DEMO_PORT 8083
#define PROD_PORT 8084

typedef struct _msgline
{
	char ip[40];
	char userId[80];
	char procName[80];
	int bytesRead;
	int bytesSent;
	char msg[128];
	char chkbtye;
} msgline;

extern const char* g_dbName;

#endif
