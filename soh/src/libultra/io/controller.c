#include "global.h"

OSPifRam __osPifInternalBuff;
u8 __osContLastPoll;
u8 __osMaxControllers; // always 4

OSTimer __osEepromTimer;
OSMesgQueue __osEepromTimerMsgQ;
OSMesg __osEepromTimerMsg;

u32 gOSContInitialized = 0;

#define HALF_SECOND OS_USEC_TO_CYCLES(500000)

s32 osContInit(OSMesgQueue* mq, u8* ctlBitfield, OSContStatus* status) {
    OSMesg mesg;
    s32 ret = 0;
    OSTime currentTime;
    OSTimer timer;
    OSMesgQueue timerqueue;

    if (gOSContInitialized) {
        return 0;
    }

    gOSContInitialized = 1;
    currentTime = osGetTime();
    if (HALF_SECOND > currentTime) {
        osCreateMesgQueue(&timerqueue, &mesg, 1);
        osSetTimer(&timer, HALF_SECOND - currentTime, 0, &timerqueue, &mesg);
        osRecvMesg(&timerqueue, &mesg, OS_MESG_BLOCK);
    }
    __osMaxControllers = MAXCONTROLLERS;
    __osPackRequestData(CONT_CMD_REQUEST_STATUS);
    ret = __osSiRawStartDma(OS_WRITE, &__osPifInternalBuff);
    osRecvMesg(mq, &mesg, OS_MESG_BLOCK);
    ret = __osSiRawStartDma(OS_READ, &__osPifInternalBuff);
    osRecvMesg(mq, &mesg, OS_MESG_BLOCK);
    __osContGetInitData(ctlBitfield, status);
    __osContLastPoll = CONT_CMD_REQUEST_STATUS;
    __osSiCreateAccessQueue();
    osCreateMesgQueue(&__osEepromTimerMsgQ, &__osEepromTimerMsg, 1);

    return ret;
}

void __osContGetInitData(u8* ctlBitfield, OSContStatus* status) {
    u8* bufptr;
    __OSContRequestHeader req;
    s32 i;
    u8 bitfieldTemp = 0;

    bufptr = (u8*)(&__osPifInternalBuff);

    for (i = 0; i < __osMaxControllers; i++, bufptr += sizeof(req), status++) {
        req = *((__OSContRequestHeader*)bufptr);
        status->errno = (req.rxsize & 0xC0) >> 4;
        if (status->errno) {
            continue;
        }
        status->type = req.typel << 8 | req.typeh;
        status->status = req.status;
        bitfieldTemp |= 1 << i;
    }
    *ctlBitfield = bitfieldTemp;
}

void __osPackRequestData(u8 poll) {
    u8* bufptr;
    __OSContRequestHeader req;
    s32 i;

    for (i = 0; i < 0xF; i++) {
        __osPifInternalBuff.ram[i] = 0;
    }
    __osPifInternalBuff.status = 1;

    bufptr = (u8*)(&__osPifInternalBuff);

    req.align = 0xFF;
    req.txsize = 1;
    req.rxsize = 3;
    req.poll = poll;
    req.typeh = 0xFF;
    req.typel = 0xFF;
    req.status = 0xFF;
    req.align1 = 0xFF;

    for (i = 0; i < __osMaxControllers; i++) {
        *((__OSContRequestHeader*)bufptr) = req;
        bufptr += sizeof(req);
    }
    *((u8*)bufptr) = 254;
}
