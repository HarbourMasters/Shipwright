#include "ultra64.h"

s32 osSendMesg(OSMesgQueue* mq, OSMesg msg, s32 flags) {
    register u32 saveMask = __osDisableInt();
    register s32 last;

    while (MQ_IS_FULL(mq)) {
        if (flags == OS_MESG_BLOCK) {
            __osRunningThread->state = OS_STATE_WAITING;
            __osEnqueueAndYield(&mq->fullQueue);
        } else {
            __osRestoreInt(saveMask);
            return -1;
        }
    }

    last = (mq->first + mq->validCount) % mq->msgCount;

    mq->msg[last] = msg;
    mq->validCount++;

    if (mq->mtQueue->next != NULL) {
        osStartThread(__osPopThread(&mq->mtQueue));
    }

    __osRestoreInt(saveMask);
    return 0;
}
