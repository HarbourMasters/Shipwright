#ifndef CRASH_HANDLER_H
#define CRASH_HANDLER_H

#ifdef _linux_

#ifdef __cplusplus
extern "C" {
#endif

void ErrorHandler(int sig);
void SetupHandlerLinux(void);

#ifdef __cplusplus
}
#endif

#endif // _linux)

#endif // CRASH_HANDLER_H