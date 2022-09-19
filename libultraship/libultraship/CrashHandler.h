#ifndef CRASH_HANDLER_H
#define CRASH_HANDLER_H

#ifdef __linux__

#ifdef __cplusplus
extern "C" {
#endif

void SetupHandlerLinux(void);

#ifdef __cplusplus
}
#endif

#elif _WIN32 // __linux__ ^^^^ _WIN32 vvvvv
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOGDI
#define NOGDI
#endif

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
