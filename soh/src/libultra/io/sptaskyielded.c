#include "global.h"

u32 osSpTaskYielded(OSTask* task) {
    u32 ret;
    u32 status = __osSpGetStatus();

    if (status & SP_STATUS_YIELDED) {
        ret = OS_TASK_YIELDED;
    } else {
        ret = 0;
    }

    if (status & SP_STATUS_YIELD) {
        task->t.flags |= ret;
        task->t.flags &= ~OS_TASK_DP_WAIT;
    }

    return ret;
}
