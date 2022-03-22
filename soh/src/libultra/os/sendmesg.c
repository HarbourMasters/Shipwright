#include "global.h"

s32 osSendMesg(OSMesgQueue* mq, OSMesg mesg, s32 flag) {
    register u32 prevInt = __osDisableInt();
    register u32 index;

    while (mq->validCount >= mq->msgCount) {
        if (flag == OS_MESG_BLOCK) {
            __osRunningThread->state = 8;
            __osEnqueueAndYield(&mq->fullqueue);
        } else {
            __osRestoreInt(prevInt);
            return -1;
        }
    }

    index = (mq->first + mq->validCount) % mq->msgCount;
    mq->msg[index] = mesg;
    mq->validCount++;

    if (mq->mtqueue->next != NULL) {
        osStartThread(__osPopThread(&mq->mtqueue));
    }

    __osRestoreInt(prevInt);

    return 0;
}
