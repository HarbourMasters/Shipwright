#ifndef CRASH_HANDLER_H
#define CRASH_HANDLER_H

#ifdef _MSC_VER
#define NOGDI
#include <Windows.h>
#include <DbgHelp.h>
#include <inttypes.h>
#include <winnt.h>

#ifdef __cplusplus
extern "C" {
#endif
int seh_filter(EXCEPTION_POINTERS* ex);
#ifdef __cplusplus
}
#endif

#endif // _MSC_VER

#endif // CRASH_HANDLER_H
