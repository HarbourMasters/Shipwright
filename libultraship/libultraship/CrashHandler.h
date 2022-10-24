#ifndef CRASH_HANDLER_H
#define CRASH_HANDLER_H

#include <stddef.h>

typedef void(*CrashHandlerCallback)(char*, size_t*);

#ifdef __cplusplus
extern "C" {
#endif

void CrashHandler_Init(CrashHandlerCallback callback);

#ifdef __cplusplus
}
#endif

#ifdef _WIN32
#include <windows.h>
#include <DbgHelp.h>

#include <inttypes.h>
#include <excpt.h>


#ifdef __cplusplus
extern "C" {
#endif

LONG seh_filter(struct _EXCEPTION_POINTERS* ex);

#ifdef __cplusplus
}
#endif

#pragma comment(lib, "Dbghelp.lib")
#endif

#endif // CRASH_HANDLER_H
