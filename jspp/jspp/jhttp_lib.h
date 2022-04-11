#ifndef _jhttp_lib_h
#define _jhttp_lib_h

#ifdef _WIN32
#include <spl/configwin32.h>
#endif

#include <spl/net/http_lib.h>
#include <jspp/variant.h>
#include <jspp/interp.h>

#define HTTPREQ_MAJIC 0x22
enum InterpErrors httprequest_httprequest(qinterp *qip, const void *mp, variant *ret);
enum InterpErrors httprequest_get(qinterp *qip, const void *mp, variant *ret);

#endif
