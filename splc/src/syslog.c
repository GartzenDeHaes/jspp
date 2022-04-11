#include <spl/datetime.h>
#include <spl/stringbuffer.h>
#include <spl/io/syslog.h>
#include <spl/net/udpsocket.h>
#include <stdio.h>

static bool g_syslogFileNameInited = false;
static char g_syslogFileName[512];

static int g_syslogRemotePort = 514;
static int g_syslogRemoteHostInited = false;
static char g_syslogRemoteHost[80];

const char* syslogSeverityLit(enum _Severity s)
{
	switch (s)
	{
		case SEVERTITY_EMERGENCY:
			return "Emergency";
		case SEVERITY_ALERT:
			return "Alert";
		case SEVERITY_CRIT:
			return "Critical";
		case SEVERITY_ERROR:
			return "Error";
		case SEVERITY_WARN:
			return "Warning";
		case SEVERITY_NOTICE:
			return "Notice";
		case SEVERITY_INFO:
			return "Info";
		case SEVERITY_DEBUG:
			return "Debug";
		default:
			return "Unknown";
	}
}

void syslogSetOutputFile(const char* filename)
{
	StrCpyLen(g_syslogFileName, filename, sizeof(g_syslogFileName));
	g_syslogFileNameInited = true;
	
	syslogWrite(FACILITY_SYSLOG, SEVERITY_NOTICE, "syslog", "Inception");
}

void syslogSetRemoteHost(const char* host)
{
	StrCpyLen(g_syslogRemoteHost, host, sizeof(g_syslogRemoteHost));
	g_syslogRemoteHostInited = true;
}

void syslogSetRemotePort(int port)
{
	g_syslogRemotePort = port;
}

void syslogWrite(enum _Facility f, enum _Severity s, const char* system, const char* message)
{
	syslogWriteEx(f, s, "", "", "", system, message);
}

void syslogWriteEx(enum _Facility f, enum _Severity s, const char* host, const char* proc, const char* user, const char* system, const char* message)
{
	int pri = (int)f * 8 + (int)s;
	datetime* dtm = datetimeNew();
	string* sdtm = datetimeToString(dtm);
	stringbuffer* buf;
	FILE* fp;
	
	if (! g_syslogFileNameInited)
	{
		syslogSetOutputFile("syslog.txt");
	}
	
	fp = fopen(g_syslogFileName, "a");
	
	datetimeDelete(dtm);
	
	if (fp == NULL)
	{
		stringDelete(sdtm);
		return;
	}
	
	if (NULL == (buf = stringbufferNew()))
	{
		fclose(fp);
		stringDelete(sdtm);
		return;
	}
	
	stringbufferAppendCh(buf, '<');
	stringbufferAppendInt(buf, pri);
	stringbufferAppendCh(buf, '>');

	stringbufferAppendString(buf, sdtm);
	stringDelete(sdtm);
	
	stringbufferAppendCh(buf, ' ');
	stringbufferAppend(buf, host);
	
	stringbufferAppendCh(buf, ' ');
	stringbufferAppend(buf, user);
	
	stringbufferAppendCh(buf, '@');
	stringbufferAppend(buf, proc);
	
	stringbufferAppend(buf, ": ");
	stringbufferAppend(buf, message);
	
	fprintf(fp, "%s\n", buf->m_buf);
	
	fclose(fp);
	
	if (g_syslogRemoteHostInited)
	{
		udpsocketSend(g_syslogRemoteHost, g_syslogRemotePort, buf->m_buf, buf->m_used);
	}

	stringbufferDelete(buf);
}
