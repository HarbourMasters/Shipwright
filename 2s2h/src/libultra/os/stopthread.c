#include "ultra64.h"

void osStopThread(OSThread* t) {
    register u32 saveMask = __osDisableInt();
    register u16 state;

    if (t == NULL) {
        state = OS_STATE_RUNNING;
    } else {
        state = t->state;
    }

    switch (state) {
        case OS_STATE_RUNNING:
            __osRunningThread->state = OS_STATE_STOPPED;
            __osEnqueueAndYield(NULL);
            break;

        case OS_STATE_RUNNABLE:
        case OS_STATE_WAITING:
            t->state = OS_STATE_STOPPED;
            __osDequeueThread(t->queue, t);
            break;
    }

    __osRestoreInt(saveMask);
}
