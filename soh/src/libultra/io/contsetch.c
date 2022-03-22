#include "ultra64.h"
#include "global.h"

/*
 * s32 osContSetCh(u8 ch)
 * This function specifies the number of devices for the functions to access when those functions access to multiple
 * direct SI devices.
 */
s32 osContSetCh(u8 ch) {
    __osSiGetAccess();

    if (ch > MAXCONTROLLERS) {
        __osMaxControllers = MAXCONTROLLERS;
    } else {
        __osMaxControllers = ch;
    }

    __osContLastPoll = -2;
    __osSiRelAccess();
    return 0;
}
