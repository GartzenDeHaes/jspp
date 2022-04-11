#ifndef _stats_h
#define _stats_h

#include <spl/stringbuffer.h>

void statsInit();
void statsBeginCall(const int recvlen);
void statsEndCall(const int sendlen);
void statsWriteRecord(stringbuffer* buf);

void clearStatsAcc();
void statsAcc(void *sqlsa);
void writeStatsAcc(stringbuffer* xml, void *sqlsa);
void writeStats(stringbuffer* xml, void *sqlsa);

#endif
