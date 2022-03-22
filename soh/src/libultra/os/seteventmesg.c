#include "global.h"
#include "ultra64/internal.h"

__OSEventState __osEventStateTab[OS_NUM_EVENTS + 1];

u32 __osPreNMI = false;

void osSetEventMesg(OSEvent e, OSMesgQueue* mq, OSMesg msg) {
    register u32 prevInt = __osDisableInt();
    __OSEventState* msgs = &__osEventStateTab[e];

    msgs->queue = mq;
    msgs->msg = msg;

    if (e == OS_EVENT_PRENMI) {
        if (__osShutdown && !__osPreNMI) {
            osSendMesg(mq, msg, OS_MESG_NOBLOCK);
        }
        __osPreNMI = true;
    }
    __osRestoreInt(prevInt);
}
