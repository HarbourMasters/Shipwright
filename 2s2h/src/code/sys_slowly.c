/**
 * @file sys_slowly.c
 *
 * This file implements a manager for running an asynchronous task on a thread with the lowest priority.
 *
 * The task callback is expected to have up to 2 void* arguments and have a void return. Setting `argCount` will adjust
 * how many args the callback gets called with, but defaults to 2 and using the 2 argument callback.
 *
 * @note: `argCount` must be set manually, as this file implements no way to configure it.
 */

#include "slowly.h"
#include "global.h"
#include "stackcheck.h"
#include "z64thread.h"
#include <string.h>

void Slowly_Main(SlowlyMgr* slowly) {
    slowly->status |= SLOWLY_STATUS_STARTED;

    switch (slowly->argCount) {
        case 0:
            slowly->callback.zero();
            break;

        case 1:
            slowly->callback.one(slowly->arg0);
            break;

        case 2:
            slowly->callback.two(slowly->arg0, slowly->arg1);
            break;

        default:
            break;
    }

    slowly->status |= SLOWLY_STATUS_DONE;
}

void Slowly_ThreadEntry(void* arg) {
    SlowlyMgr* slowly = (SlowlyMgr*)arg;

    Slowly_Main(slowly);
}

void Slowly_Init(SlowlyMgr* slowly, void* stack, SlowlyCallbackTwo callback, void* arg0, void* arg1) {
    memset(slowly, 0, sizeof(SlowlyMgr));

    slowly->argCount = 2;
    slowly->status = 0;
    slowly->callback.two = callback;
    slowly->arg0 = arg0;
    slowly->arg1 = arg1;

    osCreateThread(&slowly->thread, Z_THREAD_ID_SLOWLY, Slowly_ThreadEntry, slowly, stack, Z_PRIORITY_SLOWLY);
    osStartThread(&slowly->thread);
}

void Slowly_Destroy(SlowlyMgr* slowly) {
    osDestroyThread(&slowly->thread);
}
