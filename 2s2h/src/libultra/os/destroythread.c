#include "ultra64.h"

void osDestroyThread(OSThread* t) {
    register u32 saveMask = __osDisableInt();
    register OSThread* pred;
    register OSThread* succ;

    if (t == NULL) {
        t = __osRunningThread;
    } else if (t->state != OS_STATE_STOPPED) {
        __osDequeueThread(t->queue, t);
    }

    if (__osActiveQueue == t) {
        __osActiveQueue = __osActiveQueue->tlnext;
    } else {
        pred = __osActiveQueue;
        while (pred->priority != -1) {
            succ = pred->tlnext;
            if (succ == t) {
                pred->tlnext = t->tlnext;
                break;
            }
            pred = succ;
        }
    }

    if (t == __osRunningThread) {
        __osDispatchThread();
    }

    __osRestoreInt(saveMask);
}
