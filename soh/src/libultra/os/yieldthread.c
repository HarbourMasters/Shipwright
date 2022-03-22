#include "global.h"

void osYieldThread(void) {
    register u32 prevInt = __osDisableInt();

    __osRunningThread->state = OS_STATE_RUNNABLE;
    __osEnqueueAndYield(&__osRunQueue);
    __osRestoreInt(prevInt);
}
