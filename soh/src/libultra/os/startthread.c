#include "global.h"

void osStartThread(OSThread* thread) {
    register u32 prevInt = __osDisableInt();

    switch (thread->state) {
        case 8:
            thread->state = 2;
            __osEnqueueThread(&__osRunQueue, thread);
            break;
        case 1:
            if (thread->queue == NULL || thread->queue == &__osRunQueue) {
                thread->state = 2;
                __osEnqueueThread(&__osRunQueue, thread);
            } else {
                thread->state = 8;
                __osEnqueueThread(thread->queue, thread);
                __osEnqueueThread(&__osRunQueue, __osPopThread(thread->queue));
            }
            break;
    }

    if (__osRunningThread == NULL) {
        __osDispatchThread();
    } else {
        if (__osRunningThread->priority < __osRunQueue->priority) {
            __osRunningThread->state = 2;
            __osEnqueueAndYield(&__osRunQueue);
        }
    }

    __osRestoreInt(prevInt);
}
