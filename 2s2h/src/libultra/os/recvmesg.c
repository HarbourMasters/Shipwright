#include "ultra64.h"

s32 osRecvMesg(OSMesgQueue* mq, OSMesg* msg, s32 flags) {
    register u32 saveMask = __osDisableInt();

    while (MQ_IS_EMPTY(mq)) {
        if (flags == OS_MESG_NOBLOCK) {
            __osRestoreInt(saveMask);
            return -1;
        }
        __osRunningThread->state = 8;
        __osEnqueueAndYield(&mq->mtQueue);
    }

    if (msg != NULL) {
        *msg = mq->msg[mq->first];
    }

    mq->first = (mq->first + 1) % mq->msgCount;
    mq->validCount--;

    if (mq->fullQueue->next != NULL) {
        osStartThread(__osPopThread(&mq->fullQueue));
    }

    __osRestoreInt(saveMask);
    return 0;
}
