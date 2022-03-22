#include "global.h"

void osDestroyThread(OSThread* thread) {
    register u32 prevInt = __osDisableInt();
    register OSThread* s1;
    register OSThread* s2;

    if (thread == NULL) {
        thread = __osRunningThread;

    } else if (thread->state != OS_STATE_STOPPED) {
        __osDequeueThread(thread->queue, thread);
    }

    if (__osActiveQueue == thread) {
        __osActiveQueue = __osActiveQueue->tlnext;
    } else {
        s1 = __osActiveQueue;
        while (s1->priority != -1) {
            s2 = s1->tlnext;
            if (s2 == thread) {
                s1->tlnext = thread->tlnext;
                break;
            }
            s1 = s2;
        }
    }

    if (thread == __osRunningThread) {
        __osDispatchThread();
    }

    __osRestoreInt(prevInt);
}
