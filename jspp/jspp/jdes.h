#ifndef _jdes_h
#define _jdes_h

#ifdef _WIN32
#include <spl/configwin32.h>
#endif

#include <spl/des.h>
#include <jspp/variant.h>
#include <jspp/interp.h>

#ifdef __cplusplus
extern "C" {
#endif

enum InterpErrors des_encrypt(qinterp *qip, const void *mp, variant *ret);
enum InterpErrors des_decrypt(qinterp *qip, const void *mp, variant *ret);

#ifdef __cplusplus
}
#endif

#endif
