#include "ultra64.h"

OSYieldResult osSpTaskYielded(OSTask* task) {
    s32 status = __osSpGetStatus();
    OSYieldResult result;

    if (status & SP_STATUS_YIELDED) {
        result = OS_TASK_YIELDED;
    } else {
        result = 0;
    }
    if (status & SP_STATUS_YIELD) {
        task->t.flags |= result;
        task->t.flags &= ~OS_TASK_DP_WAIT;
    }
    return result;
}
