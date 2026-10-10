#include "ultra64.h"

void osViSetEvent(OSMesgQueue* mq, OSMesg m, u32 retraceCount) {
    register u32 saveMask = __osDisableInt();

    __osViNext->mq = mq;
    __osViNext->msg = m;
    __osViNext->retraceCount = retraceCount;

    __osRestoreInt(saveMask);
}
