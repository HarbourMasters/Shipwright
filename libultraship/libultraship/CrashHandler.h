#ifndef CRASH_HANDLER_H
#define CRASH_HANDLER_H

#include <signal.h>

#ifdef __cplusplus
extern "C" {
#endif

void ErrorHandler(int sig);

#ifdef __cplusplus
}
#endif

#endif // CRASH_HANDLER_H