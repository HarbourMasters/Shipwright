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

#endif // __linux__

#endif // CRASH_HANDLER_H