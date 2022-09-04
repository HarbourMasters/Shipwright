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
#ifdef __cplusplus
extern "C" {
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

LONG seh_filter(struct _EXCEPTION_POINTERS* ex);

#ifdef __cplusplus
}
#endif

#pragma comment(lib, "Dbghelp.lib")
#endif

#endif // CRASH_HANDLER_H
