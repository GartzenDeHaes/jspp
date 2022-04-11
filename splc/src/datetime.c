#include <spl/datetime.h>
#include <stdio.h>

void datetimeInit(datetime* dtm)
{
	time(&dtm->m_timet);
	dtm->m_dtm = *localtime(&dtm->m_timet);	
}

datetime* datetimeNew()
{
	datetime* dtm = (datetime *)malloc(sizeof(datetime));
	if (dtm == NULL)
	{
		return NULL;
	}

	datetimeInit(dtm);	
	
	return dtm;
}

void datetimeDelete(datetime* dtm)
{
	free(dtm);
}

int datetimeYear(datetime* dtm)
{
	return dtm->m_dtm.tm_year + 1900;
}

int datetimeMonth(datetime* dtm)
{
	return dtm->m_dtm.tm_mon + 1;
}

int datetimeDay(datetime* dtm)
{
	return dtm->m_dtm.tm_mday;
}

int datetimeHour(datetime* dtm)
{
	return dtm->m_dtm.tm_hour;
}

int datetimeMinute(datetime* dtm)
{
	return dtm->m_dtm.tm_min;
}

int datetimeSecond(datetime* dtm)
{
	return dtm->m_dtm.tm_sec;
}

string* datetimeToString(datetime* dtm)
{
	char buf[32];
	sprintf(buf, "%d/%d/%d %d:%d:%d", datetimeMonth(dtm), datetimeDay(dtm), datetimeYear(dtm), datetimeHour(dtm), datetimeMinute(dtm), datetimeSecond(dtm));
	return stringNew(buf);
}

void datetimeToStringBuf(datetime* dtm, char* buf, const int buflen)
{
	if (buflen < 18)
	{
		*buf = '\0';
		return;
	}
	sprintf(buf, "%d/%d/%d %d:%d:%d", datetimeYear(dtm), datetimeMonth(dtm), datetimeDay(dtm), datetimeHour(dtm), datetimeMinute(dtm), datetimeSecond(dtm));
}
