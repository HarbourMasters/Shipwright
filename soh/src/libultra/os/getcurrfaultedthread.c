// clang-format off

#include "global.h"

OSThread* __osGetCurrFaultedThread(void) {
    return __osFaultedThread;
}
