#ifndef _intrinsics_h
#define _intrinsics_h

#ifdef _WIN32
#include <spl/configwin32.h>
#endif

#include <jspp/native.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  Object
 */
enum InterpErrors object_parseInt(qinterp *qip, const void *mp, variant *ret);
enum InterpErrors object_parseFloat(qinterp *qip, const void *mp, variant *ret);
enum InterpErrors object_toString(qinterp *qip, const void *mp, variant *ret);
enum InterpErrors object_toStringI(qinterp *qip, const void *mp, variant *ret);
enum InterpErrors object_typename(qinterp *qip, const void *mp, variant *ret);
enum InterpErrors object_length(qinterp *qip, const void *mp, variant *ret);

/*
 *  Array
 */
#define ARRAY_MAGIC ((byte)0x68)
#define ARRAY_MAX_DIMS 10
enum InterpErrors array_arrayX(qinterp *qip, const void *mp, variant *ret);
enum InterpErrors array_itemX(qinterp *qip, const void *mp, variant *ret);
enum InterpErrors array_length(qinterp *qip, const void *mp, variant *ret);
enum InterpErrors array_dims(qinterp *qip, const void *mp, variant *ret);

enum InterpErrors arrayHelperConstruct(jsinst *inst, int dims, ...);
enum InterpErrors arrayHelperSet(jsinst *array, variant *data, int dimcount, ...);

/*
 *  Vector
 */
#define VECTOR_MAGIC ((byte)0xF8)
enum InterpErrors vector_vector(qinterp *qip, const void *mp, variant *ret);
enum InterpErrors vector_add(qinterp *qip, const void *mp, variant *ret);
enum InterpErrors vector_size(qinterp *qip, const void *mp, variant *ret);
enum InterpErrors vector_elementat(qinterp *qip, const void *mp, variant *ret);
enum InterpErrors vector_setelementat(qinterp *qip, const void *mp, variant *ret);
enum InterpErrors vector_clear(qinterp *qip, const void *mp, variant *ret);

#if defined(_CONSOLE) && !defined(CONSOLE)  /* WIN32 defines this for console projects */
#define CONSOLE
#endif
/*
 *  console (printf console will only be added when CONSOLE is defined.  
 *  These definitions remain for other implmentations of console.)
 */
enum InterpErrors console_writeln(qinterp *qip, const void *mp, variant *ret);
enum InterpErrors console_write(qinterp *qip, const void *mp, variant *ret);
enum InterpErrors console_readln(qinterp *qip, const void *mp, variant *ret);

/*
 *  StringBuffer
 */
#define STRBUF_MAGIC ((byte)0xF8)
enum InterpErrors stringbuffer_stringbuffer(qinterp *qip, const void *mp, variant *ret);
enum InterpErrors stringbuffer_append(qinterp *qip, const void *mp, variant *ret);
enum InterpErrors stringbuffer_tostring(qinterp *qip, const void *mp, variant *ret);

/*
 *  DateTime
 */
#define DATETIME_MAGIC ((byte)0xA8)
enum InterpErrors datetime_datetime(qinterp *qip, const void *mp, variant *ret);
enum InterpErrors datetime_now(qinterp *qip, const void *mp, variant *ret);
enum InterpErrors datetime_diffms(qinterp *qip, const void *mp, variant *ret);
enum InterpErrors datetime_tomilliseconds(qinterp *qip, const void *mp, variant *ret);

/*
 *  Math
 */
enum InterpErrors math_rand(qinterp *qip, const void *mp, variant *ret);
enum InterpErrors math_exp(qinterp *qip, const void *mp, variant *ret);

/*
 *  hashtable
 */
#define HASHTABLE_MAJIC ((byte)0xB2)
enum InterpErrors hashtable_hashtable(qinterp *qip, const void *mp, variant *ret);
enum InterpErrors hashtable_put(qinterp *qip, const void *mp, variant *ret);
enum InterpErrors hashtable_get(qinterp *qip, const void *mp, variant *ret);
enum InterpErrors hashtable_clear(qinterp *qip, const void *mp, variant *ret);

/*
 *  String
 */
enum InterpErrors string_string(qinterp *qip, const void *mp, variant *ret);
enum InterpErrors string_toString(qinterp *qip, const void *mp, variant *ret);


#ifdef __cplusplus
}
#endif

#endif
