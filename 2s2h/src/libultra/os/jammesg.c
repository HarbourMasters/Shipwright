#include "ultra64.h"

s32 osJamMesg(OSMesgQueue* mq, OSMesg msg, s32 flag) {
    register u32 saveMask = __osDisableInt();

    while (mq->validCount >= mq->msgCount) {
        if (flag == OS_MESG_BLOCK) {
            __osRunningThread->state = OS_STATE_WAITING;
            __osEnqueueAndYield(&mq->fullQueue);
        } else {
            __osRestoreInt(saveMask);
            return -1;
        }
    }

    mq->first = (mq->first + mq->msgCount - 1) % mq->msgCount;

    mq->msg[mq->first] = msg;
    mq->validCount++;

    if (mq->mtQueue->next != NULL) {
        osStartThread(__osPopThread(&mq->mtQueue));
    }

    __osRestoreInt(saveMask);
    return 0;
}
