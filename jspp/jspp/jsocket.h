#ifndef _jsocket_h
#define _jsocket_h

#ifdef _WIN32
#include <spl/configwin32.h>
#endif

#include <spl/net/csocket.h>
#include <jspp/variant.h>
#include <jspp/interp.h>

enum InterpErrors socket_socket(qinterp *qip, const void *mp, variant *ret);
enum InterpErrors socket_accept(qinterp *qip, const void *mp, variant *ret);
enum InterpErrors socket_connect(qinterp *qip, const void *mp, variant *ret);
enum InterpErrors socket_bind(qinterp *qip, const void *mp, variant *ret);
enum InterpErrors socket_listen(qinterp *qip, const void *mp, variant *ret);
enum InterpErrors socket_settimeout(qinterp *qip, const void *mp, variant *ret);
enum InterpErrors socket_setlingeron(qinterp *qip, const void *mp, variant *ret);
enum InterpErrors socket_setnodelay(qinterp *qip, const void *mp, variant *ret);
enum InterpErrors socket_send(qinterp *qip, const void *mp, variant *ret);
enum InterpErrors socket_recv(qinterp *qip, const void *mp, variant *ret);
enum InterpErrors socket_flush(qinterp *qip, const void *mp, variant *ret);
enum InterpErrors socket_error(qinterp *qip, const void *mp, variant *ret);
enum InterpErrors socket_setblocking(qinterp *qip, const void *mp, variant *ret);
enum InterpErrors socket_close(qinterp *qip, const void *mp, variant *ret);


#endif
