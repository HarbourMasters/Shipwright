#include "PR/os_pfs.h"
#include "PR/controller.h"
#include "PR/siint.h"
#include "alignment.h"

OSPifRam __osPfsPifRam ALIGNED(16);

void __osPfsRequestData(u8 poll);
void __osPfsGetInitData(u8* pattern, OSContStatus* contData);

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

        ret = __osSiRawStartDma(OS_WRITE, &__osPfsPifRam);
        osRecvMesg(mq, &msg, OS_MESG_BLOCK);

        ret = __osSiRawStartDma(OS_READ, &__osPfsPifRam);
        osRecvMesg(mq, &msg, OS_MESG_BLOCK);

        __osPfsGetInitData(&bitpattern, &contData[0]);

        for (channel = 0; channel < __osMaxControllers; channel++) {
            if (!(contData[channel].status & CONT_ADDR_CRC_ER)) {
                crcErrorCount--;
                break;
            }
        }
        if (channel == __osMaxControllers) {
            crcErrorCount = 0;
        }
    } while (crcErrorCount > 0);

    for (channel = 0; channel < __osMaxControllers; channel++) {
        if ((contData[channel].errno == 0) && (contData[channel].status & CONT_CARD_ON)) {
            bits |= (1 << channel);
        }
    }
    __osSiRelAccess();
    *pattern = bits;
    return ret;
}

void __osPfsRequestData(u8 poll) {
    u8* bufPtr = (u8*)&__osPfsPifRam;
    __OSContRequesFormat req;
    s32 i;

    __osContLastPoll = poll;

    __osPfsPifRam.status = CONT_CMD_EXE;

    req.align = CONT_CMD_NOP;
    req.txsize = CONT_CMD_REQUEST_STATUS_TX;
    req.rxsize = CONT_CMD_REQUEST_STATUS_RX;
    req.poll = poll;
    req.typeh = CONT_CMD_NOP;
    req.typel = CONT_CMD_NOP;
    req.status = CONT_CMD_NOP;
    req.align1 = CONT_CMD_NOP;

    for (i = 0; i < __osMaxControllers; i++) {
        *(__OSContRequesFormat*)bufPtr = req;
        bufPtr += sizeof(__OSContRequesFormat);
    }
    *bufPtr = CONT_CMD_END;
}

void __osPfsGetInitData(u8* pattern, OSContStatus* contData) {
    u8* bufptr;
    __OSContRequesFormat req;
    s32 i;
    u8 bits = 0;

    bufptr = (u8*)&__osPfsPifRam;

    for (i = 0; i < __osMaxControllers; i++, bufptr += sizeof(req), contData++) {
        req = *(__OSContRequesFormat*)bufptr;
        contData->errno = CHNL_ERR(req);

        if (contData->errno) {
            continue;
        }

        contData->type = ((req.typel << 8) | req.typeh);
        contData->status = req.status;
        bits |= (1 << i);
    }
    *pattern = bits;
}
