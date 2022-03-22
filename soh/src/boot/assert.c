#include "global.h"

void __assert(const char* exp, const char* file, s32 line) {
    char msg[256];

    osSyncPrintf("Assertion failed: %s, file %s, line %d, thread %d\n", exp, file, line, osGetThreadId(NULL));
    sprintf(msg, "ASSERT: %s:%d(%d)", file, line, osGetThreadId(NULL));
    Fault_AddHungupAndCrashImpl(msg, exp);
}
