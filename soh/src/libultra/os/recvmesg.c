#include "global.h"

s32 osRecvMesg(OSMesgQueue* mq, OSMesg* msg, s32 flag) {
    register u32 prevInt = __osDisableInt();

    while (mq->validCount == 0) {
        if (flag == OS_MESG_NOBLOCK) {
            __osRestoreInt(prevInt);
            return -1;
        }
        __osRunningThread->state = 8;
        __osEnqueueAndYield((OSThread**)mq);
    }

    if (msg != NULL) {
        *msg = mq->msg[mq->first];
    }

    mq->first = (mq->first + 1) % mq->msgCount;
    mq->validCount--;

    if (mq->fullqueue->next != NULL) {
        osStartThread(__osPopThread(&mq->fullqueue));
    }

    __osRestoreInt(prevInt);

    return 0;
}
