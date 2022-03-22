#include "ultra64.h"
#include "global.h"

s32 osPfsIsPlug(OSMesgQueue* mq, u8* pattern) {
    s32 ret = 0;
    OSMesg msg;
    u8 bitpattern;
    OSContStatus contData[MAXCONTROLLERS];
    s32 channel;
    u8 bits = 0;
    s32 crcErrorCount = 3;

    __osSiGetAccess();

    do {
        __osPfsRequestData(CONT_CMD_REQUEST_STATUS);

        ret = __osSiRawStartDma(OS_WRITE, &gPifMempakBuf);
        osRecvMesg(mq, &msg, OS_MESG_BLOCK);

        ret = __osSiRawStartDma(OS_READ, &gPifMempakBuf);
        osRecvMesg(mq, &msg, OS_MESG_BLOCK);

        __osPfsGetInitData(&bitpattern, &contData[0]);

        for (channel = 0; channel < __osMaxControllers; channel++) {
            if ((contData[channel].status & CONT_ADDR_CRC_ER) == 0) {
                crcErrorCount--;
                break;
            }
        }
        if (channel == __osMaxControllers) {
            crcErrorCount = 0;
        }
    } while (crcErrorCount > 0);

    for (channel = 0; channel < __osMaxControllers; channel++) {
        if ((contData[channel].errno == 0) && ((contData[channel].status & CONT_CARD_ON) != 0)) {
            bits |= (1 << channel);
        }
    }
    __osSiRelAccess();
    *pattern = bits;
    return ret;
}

void __osPfsRequestData(u8 poll) {
    u8* bufPtr = (u8*)&gPifMempakBuf;
    __OSContRequestHeader req;
    s32 i;

    __osContLastPoll = poll;

    gPifMempakBuf.status = 1;

    req.align = 0xFF;
    req.txsize = 1;
    req.rxsize = 3;
    req.poll = poll;
    req.typeh = 0xFF;
    req.typel = 0xFF;
    req.status = 0xFF;
    req.align1 = 0xFF;

    for (i = 0; i < __osMaxControllers; i++) {
        *((__OSContRequestHeader*)bufPtr) = req;
        bufPtr += sizeof(req);
    }
    *((u8*)bufPtr) = CONT_CMD_END;
}

void __osPfsGetInitData(u8* pattern, OSContStatus* contData) {
    u8* bufptr;
    __OSContRequestHeader req;
    s32 i;
    u8 bits = 0;

    bufptr = (u8*)&gPifMempakBuf;

    for (i = 0; i < __osMaxControllers; i++, bufptr += sizeof(req), contData++) {
        req = *((__OSContRequestHeader*)bufptr);
        contData->errno = ((req.rxsize & 0xC0) >> 4);

        if (contData->errno) {
            continue;
        }

        contData->type = ((req.typel << 8) | req.typeh);
        contData->status = req.status;
        bits |= (1 << i);
    }
    *pattern = bits;
}
