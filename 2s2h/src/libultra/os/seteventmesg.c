#include "ultra64.h"
#include "PR/osint.h"
#include "libc/stdbool.h"
#include "alignment.h"

u32 __osPreNMI = false;

__OSEventState __osEventStateTab[OS_NUM_EVENTS] ALIGNED(8);

void osSetEventMesg(OSEvent e, OSMesgQueue* mq, OSMesg m) {
    register u32 saveMask = __osDisableInt();
    __OSEventState* es;

    es = &__osEventStateTab[e];

    es->messageQueue = mq;
    es->message = m;

    if (e == OS_EVENT_PRENMI) {
        if (__osShutdown && !__osPreNMI) {
            osSendMesg(mq, m, OS_MESG_NOBLOCK);
        }
        __osPreNMI = true;
    }

    __osRestoreInt(saveMask);
}
