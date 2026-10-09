#include "ultra64.h"

OSMesgQueue* osPiGetCmdQueue(void) {
    if (!__osPiDevMgr.active) {
        return NULL;
    }

    return __osPiDevMgr.cmdQueue;
}
