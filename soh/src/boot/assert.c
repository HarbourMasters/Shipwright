#include "global.h"
#include <stdio.h>

#ifndef __SWITCH__
#ifdef __WIIU__
void _assert(const char* exp, const char* file, s32 line) {
#else
void __assert(const char* exp, const char* file, s32 line) {
#endif
    char msg[256];

    osSyncPrintf("Assertion failed: %s, file %s, line %d, thread %d\n", exp, file, line, osGetThreadId(NULL));
    snprintf(msg, sizeof(msg), "ASSERT: %s:%d(%d)", file, line, osGetThreadId(NULL));
    Fault_AddHungupAndCrashImpl(msg, exp);
}
#endif