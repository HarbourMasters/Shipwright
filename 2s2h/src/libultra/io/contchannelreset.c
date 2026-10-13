#include "ultra64.h"
#include "PR/controller.h"

s32 __osContChannelReset(OSMesgQueue* mq, s32 channel) {
    s32 i;
    s32 ret;
    u8* bufptr = (u8*)&__osPfsPifRam;

    __osSiGetAccess();

    __osPfsPifRam.status = CONT_CMD_EXE;

    for (i = 0; i < channel; i++) {
        *bufptr++ = CONT_CMD_REQUEST_STATUS;
    }

    *bufptr++ = CONT_CMD_CHANNEL_RESET;
    *bufptr = CONT_CMD_END;

    __osSiRawStartDma(OS_WRITE, &__osPfsPifRam);
    osRecvMesg(mq, NULL, OS_MESG_BLOCK);
    ret = __osSiRawStartDma(OS_READ, &__osPfsPifRam);
    osRecvMesg(mq, NULL, OS_MESG_BLOCK);

    __osSiRelAccess();

    return ret;
}
