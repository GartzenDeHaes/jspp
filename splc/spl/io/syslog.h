#ifndef _syslog_h
#define _syslog_h

enum _Severity
{
	SEVERTITY_EMERGENCY = 0,	/* System is unusable */
	SEVERITY_ALERT = 1,			/* Action must be taken immediately */
	SEVERITY_CRIT = 2,			/* Critical condition */
	SEVERITY_ERROR = 3,			/* Error condition */
	SEVERITY_WARN = 4,			/* Warning */
	SEVERITY_NOTICE = 5,		/* Normal but significant condition */
	SEVERITY_INFO = 6,			/* General information */
	SEVERITY_DEBUG = 7			/* Debugging */
};

enum _Facility
{
	FACILITY_USER = 1,
	FACILITY_AUTHENTICATION = 4,
	FACILITY_SYSLOG = 5,
	FACILITY_AUTHORIZATION = 10,
	FACILITY_LOGAUDIT = 13,
	FACILITY_LOGALERT = 14,
	FACILITY_LOCAL0 = 16,
	FACILITY_LOCAL1 = 17,
	FACILITY_LOCAL2 = 18,
	FACILITY_LOCAL3 = 19,
	FACILITY_LOCAL4 = 20,
	FACILITY_LOCAL5 = 21,
	FACILITY_LOCAL6 = 22,
	FACILITY_LOCAL7 = 23
};

const char* syslogSeverityLit(enum _Severity s);

void syslogSetOutputFile(const char* filename);
void syslogSetRemoteHost(const char* host);
void syslogSetRemotePort(int port);

void syslogWrite(enum _Facility f, enum _Severity s, const char* system, const char* message);
void syslogWriteEx(enum _Facility f, enum _Severity s, const char* host, const char* proc, const char* user, const char* system, const char* message);


#endif
