#include "ultra64.h"
#include "PR/controller.h"

void __osPfsRequestOneChannel(s32 channel, u8 poll);
void __osPfsGetOneChannelData(s32 channel, OSContStatus* contData);

s32 __osPfsGetStatus(OSMesgQueue* queue, s32 channel) {
    s32 ret = 0;
    OSMesg msg;
    OSContStatus data;

    __osPfsInodeCacheBank = 250;

    __osPfsRequestOneChannel(channel, CONT_CMD_REQUEST_STATUS);
    ret = __osSiRawStartDma(OS_WRITE, &__osPfsPifRam);
    osRecvMesg(queue, &msg, OS_MESG_BLOCK);

    ret = __osSiRawStartDma(OS_READ, &__osPfsPifRam);
    osRecvMesg(queue, &msg, OS_MESG_BLOCK);

    __osPfsGetOneChannelData(channel, &data);
    if (((data.status & CONT_CARD_ON) != 0) && ((data.status & CONT_CARD_PULL) != 0)) {
        return PFS_ERR_NEW_PACK;
    } else if (data.errno || ((data.status & CONT_CARD_ON) == 0)) {
        return PFS_ERR_NOPACK;
    } else if ((data.status & CONT_ADDR_CRC_ER) != 0) {
        return PFS_ERR_CONTRFAIL;
    }
    return ret;
}

void __osPfsRequestOneChannel(s32 channel, u8 poll) {
    u8* bufptr;
    __OSContRequestHeaderAligned req;
    s32 idx;

    __osContLastPoll = CONT_CMD_END;
    __osPfsPifRam.status = CONT_CMD_READ_BUTTON;

    bufptr = (u8*)&__osPfsPifRam;

    req.txsize = 1;
    req.rxsize = 3;
    req.poll = poll;
    req.typeh = 0xFF;
    req.typel = 0xFF;
    req.status = 0xFF;

    for (idx = 0; idx < channel; idx++) {
        *bufptr++ = 0;
    }

    *((__OSContRequestHeaderAligned*)bufptr) = req;
    bufptr += sizeof(req);
    *((u8*)bufptr) = CONT_CMD_END;
}

void __osPfsGetOneChannelData(s32 channel, OSContStatus* contData) {
    u8* bufptr = (u8*)&__osPfsPifRam;
    __OSContRequestHeaderAligned req;
    s32 idx;

    for (idx = 0; idx < channel; idx++) {
        bufptr++;
    }

    req = *((__OSContRequestHeaderAligned*)bufptr);
    contData->errno = (req.rxsize & 0xC0) >> 4;
    if (contData->errno) {
        return;
    }

    contData->type = (req.typel << 8) | req.typeh;
    contData->status = req.status;
}
