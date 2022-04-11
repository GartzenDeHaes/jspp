#include <spl/types.h>
#include <spl/debug.h>
#include <spl/stringbuffer.h>
#include "request.h"

#ifdef __TANDEM
#include <sql.h>
#endif

extern bool _writeError(stringbuffer* xml, const char* cp1, const char* cp2);
extern bool _writeErrorCode(stringbuffer* xml, const char* cp1, const int i);

void serviceRequest2(request *req, stringbuffer* xml)
{
	_writeError(xml, "unknown procedure ", req->m_name->m_cstr);
}
