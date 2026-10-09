#include "ultra64.h"

OSTime osGetTime(void) {
    u32 CurrentCount;
    u32 elapseCount;
    OSTime currentCount;
    register u32 savedMask;

    savedMask = __osDisableInt();

    CurrentCount = osGetCount();
    elapseCount = CurrentCount - __osBaseCounter;
    currentCount = __osCurrentTime;

    __osRestoreInt(savedMask);

    return elapseCount + currentCount;
}
