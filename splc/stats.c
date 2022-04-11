#include <spl/datetime.h>
#include <spl/stringbuffer.h>

#include "stats.h"

extern datetime g_dtminception;
extern byte g_buf[1024];

#define HISTLEN 500

static clock_t g_start;
static int g_calls;
static double g_hist[HISTLEN];
static double g_recvlen[HISTLEN];
static double g_sendlen[HISTLEN];
static double g_shortest;
static double g_longest;

void statsInit()
{
	int x;
	
	g_calls = 0;
	g_shortest = 1000000;
	g_longest = 0;
	
	for (x = 0; x < HISTLEN; x++)
	{
		g_hist[x] = -1;
		g_recvlen[x] = -1;
		g_sendlen[x] = -1;
	}
}

void statsBeginCall(const int recvlen)
{
	g_start = clock();
	g_recvlen[g_calls % HISTLEN] = recvlen;
}

void statsEndCall(const int sendlen)
{
	double ticks = (clock() - g_start) / (double)CLOCKS_PER_SEC;
	g_sendlen[g_calls % HISTLEN] = sendlen;
	g_hist[g_calls++ % HISTLEN] = ticks;
	
	if (ticks < g_shortest)
	{
		g_shortest = ticks;
	}
	if (ticks > g_longest)
	{
		g_longest = ticks;
	}
}

void statsWriteRecord(stringbuffer* buf)
{
	double avg = 0;
	int x;
	double avgrecv = 0;
	double avgsend = 0;
	
	datetimeToStringBuf(&g_dtminception, g_buf, sizeof(g_buf));
	
	stringbufferAppend(buf, "<row inception='");
	stringbufferAppend(buf, g_buf);
	stringbufferAppend(buf, "' calls='");
	stringbufferAppendInt(buf, g_calls);
	stringbufferAppend(buf, "' longest='");
	stringbufferAppendReal(buf, g_longest);
	stringbufferAppend(buf, "' shortest='");
	stringbufferAppendReal(buf, g_shortest);
	stringbufferAppend(buf, "' average='");
	
	for (x = 0; x < HISTLEN; x++)
	{
		if (g_hist[x] == -1)
		{
			break;
		}
		avg += g_hist[x];
		avgrecv += g_recvlen[x];
		avgsend += g_sendlen[x];
	}
	
	if (avg != 0 && x != 0)
	{
		stringbufferAppendReal(buf, avg / x);
	}
	else
	{
		stringbufferAppend(buf, "0");
	}
	
	if (x > 0)
	{
		stringbufferAppend(buf, "' averagerecv='");
		stringbufferAppendReal(buf, avgrecv / x);
		stringbufferAppend(buf, "' averagesend='");
		stringbufferAppendReal(buf, avgsend / x);
	}
	else
	{
		stringbufferAppend(buf, "' averagerecv='0' averagesend='0");
	}
		
	stringbufferAppend(buf, "' />\r\n");
}

#ifdef __TANDEM
#include <sql.h>
EXEC SQL INCLUDE STRUCTURES SQLSA VERSION 300;
EXEC SQL INCLUDE SQLSA;
#else
#include "preparedstatement.h"
#endif

static long _records_used;
static long _records_accessed;
static long _disc_reads;
static long _messages;
static long _message_bytes;
static int _waits;
static int _escalations;

void clearStatsAcc()
{
	_records_used = 0;
	_records_accessed = 0;
	_disc_reads = 0;
	_messages = 0;
	_message_bytes = 0;
	_waits = 0;
	_escalations = 0;
}

void statsAcc(void* _sqlsa)
{
	int x;
	struct SQLSA_TYPE *sqlsa = (struct SQLSA_TYPE *)_sqlsa;
	
	for (x = 0; x < sqlsa->u.dml.num_tables; x++)
	{
		_records_used += sqlsa->u.dml.stats[x].records_used;
		_records_accessed += sqlsa->u.dml.stats[x].records_accessed;
		_disc_reads += sqlsa->u.dml.stats[x].disc_reads;
		_messages += sqlsa->u.dml.stats[x].messages;
		_message_bytes += sqlsa->u.dml.stats[x].message_bytes;
		_waits += sqlsa->u.dml.stats[x].waits;
		_escalations += sqlsa->u.dml.stats[x].escalations;
	}
}

void writeStatsAcc(stringbuffer* xml, void* _sqlsa)
{
#ifdef __TANDEM
	stringbufferAppend(xml, "<stats>\r\n");

	stringbufferAppend(xml, "<stat table_name='' ");
	stringbufferAppend(xml, "records_used='");
	stringbufferAppendLong(xml, _records_used);
	stringbufferAppend(xml, "' records_accessed='");
	stringbufferAppendLong(xml, _records_accessed);
	stringbufferAppend(xml, "' disc_reads='");
	stringbufferAppendLong(xml, _disc_reads);
	stringbufferAppend(xml, "' messages='");
	stringbufferAppendLong(xml, _messages);
	stringbufferAppend(xml, "' message_bytes='");
	stringbufferAppendLong(xml, _message_bytes);
	stringbufferAppend(xml, "' waits='");
	stringbufferAppendInt(xml, _waits);
	stringbufferAppend(xml, "' escalations='");
	stringbufferAppendInt(xml, _escalations);
	stringbufferAppend(xml, "' />");

	stringbufferAppend(xml, "</stats>\r\n");
#else
	stringbufferAppend(xml, "<stats><stat records_used='0' /></stats>");
#endif
}

void writeStats(stringbuffer* xml, void* _sqlsa)
{
#ifdef __TANDEM
	int x;
	struct SQLSA_TYPE *sqlsa = (struct SQLSA_TYPE *)_sqlsa;
	
	stringbufferAppend(xml, "<stats>\r\n");
	
	for (x = 0; x < sqlsa->u.dml.num_tables; x++)
	{
		stringbufferAppend(xml, "<stat ");
		stringbufferAppend(xml, "table_name='");
		stringbufferAppend(xml, &sqlsa->u.dml.stats[x].table_name[2]);
		stringbufferAppend(xml, "' records_used='");
		stringbufferAppendLong(xml, sqlsa->u.dml.stats[x].records_used);
		stringbufferAppend(xml, "' records_accessed='");
		stringbufferAppendLong(xml, sqlsa->u.dml.stats[x].records_accessed);
		stringbufferAppend(xml, "' disc_reads='");
		stringbufferAppendLong(xml, sqlsa->u.dml.stats[x].disc_reads);
		stringbufferAppend(xml, "' messages='");
		stringbufferAppendLong(xml, sqlsa->u.dml.stats[x].messages);
		stringbufferAppend(xml, "' message_bytes='");
		stringbufferAppendLong(xml, sqlsa->u.dml.stats[x].message_bytes);
		stringbufferAppend(xml, "' waits='");
		stringbufferAppendInt(xml, (int)sqlsa->u.dml.stats[x].waits);
		stringbufferAppend(xml, "' escalations='");
		stringbufferAppendInt(xml, (int)sqlsa->u.dml.stats[x].escalations);
		stringbufferAppend(xml, "' />");
	}
	stringbufferAppend(xml, "</stats>\r\n");
#else
	stringbufferAppend(xml, "<stats><stat records_used='0' /></stats>");
#endif
}
