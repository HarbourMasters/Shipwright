#include "global.h"

/**
 * osContStartQuery:
 * Starts to read the values for SI device status and type which are connected to the controller port and joyport
 * connector.
 */
s32 osContStartQuery(OSMesgQueue* mq) {
    s32 ret = 0;

    __osSiGetAccess();
    if (__osContLastPoll != CONT_CMD_REQUEST_STATUS) {
        __osPackRequestData(CONT_CMD_REQUEST_STATUS);
        ret = __osSiRawStartDma(OS_WRITE, &__osPifInternalBuff);
        osRecvMesg(mq, NULL, OS_MESG_BLOCK);
    }
    ret = __osSiRawStartDma(OS_READ, &__osPifInternalBuff);
    __osContLastPoll = CONT_CMD_REQUEST_STATUS;
    __osSiRelAccess();
    return ret;
}

/**
 * osContGetQuery:
 * Returns the values from osContStartQuery to status. Both functions must be paired for use.
 */
void osContGetQuery(OSContStatus* data) {
    u8 pattern;
    __osContGetInitData(&pattern, data);
}
