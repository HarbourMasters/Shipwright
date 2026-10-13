#include "ultra64.h"

OSThread* __osGetCurrFaultedThread(void) {
    return __osFaultedThread;
}
