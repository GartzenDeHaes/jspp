#ifndef _parse_h
#define _parse_h

#ifdef _WIN32
#include <spl/configwin32.h>
#endif

#include <jspp/program.h>

#ifdef __cplusplus
extern "C" {
#endif

program *parseFile( const char *filename, vector *intrinsics );
program *parseChar( const char *buf, vector *intrinsics );

#ifdef __cplusplus
}
#endif

#endif
