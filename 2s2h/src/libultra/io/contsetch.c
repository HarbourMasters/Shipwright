#include "ultra64.h"
#include "PR/controller.h"

s32 osContSetCh(u8 ch) {
    __osSiGetAccess();

    if (ch > MAXCONTROLLERS) {
        __osMaxControllers = MAXCONTROLLERS;
    } else {
        __osMaxControllers = ch;
    }
    __osContLastPoll = CONT_CMD_END;

    __osSiRelAccess();
    return 0;
}
