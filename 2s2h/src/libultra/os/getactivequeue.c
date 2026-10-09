#include "ultra64.h"

OSThread* __osGetActiveQueue(void) {
    return __osActiveQueue;
}
