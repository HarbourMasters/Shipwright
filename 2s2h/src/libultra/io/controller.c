#include "ultra64.h"
#include "PR/controller.h"
#include "libc/stdbool.h"
#include "alignment.h"
#include "macros.h"

s32 __osContinitialized = false;

OSPifRam __osContPifRam ALIGNED(16);
u8 __osContLastPoll;
u8 __osMaxControllers;

OSTimer __osEepromTimer;
OSMesgQueue __osEepromTimerQ ALIGNED(8);
OSMesg __osEepromTimerMsg[1];

s32 osContInit(OSMesgQueue* mq, u8* bitpattern, OSContStatus* data) {
    OSMesg dummy;
    s32 ret = 0;
    OSTime t;
    OSTimer mytimer;
    OSMesgQueue timerMesgQueue;

    if (__osContinitialized) {
        return 0;
    }

    __osContinitialized = true;

    t = osGetTime();
    if (t < OS_USEC_TO_CYCLES(500000)) {
        osCreateMesgQueue(&timerMesgQueue, &dummy, 1);
        osSetTimer(&mytimer, OS_USEC_TO_CYCLES(500000) - t, 0, &timerMesgQueue, &dummy);
        osRecvMesg(&timerMesgQueue, &dummy, OS_MESG_BLOCK);
    }

    __osMaxControllers = 4;

    __osPackRequestData(CONT_CMD_REQUEST_STATUS);

    ret = __osSiRawStartDma(OS_WRITE, &__osContPifRam);
    osRecvMesg(mq, &dummy, OS_MESG_BLOCK);

    ret = __osSiRawStartDma(OS_READ, &__osContPifRam);
    osRecvMesg(mq, &dummy, OS_MESG_BLOCK);

    __osContGetInitData(bitpattern, data);
    __osContLastPoll = 0;
    __osSiCreateAccessQueue();
    osCreateMesgQueue(&__osEepromTimerQ, __osEepromTimerMsg, ARRAY_COUNT(__osEepromTimerMsg));

    return ret;
}

void __osContGetInitData(u8* pattern, OSContStatus* data) {
    u8* ptr;
    __OSContRequesFormat requestHeader;
    s32 i;
    u8 bits;

    bits = 0;
    ptr = (u8*)__osContPifRam.ramarray;
    for (i = 0; i < __osMaxControllers; i++, ptr += sizeof(requestHeader), data++) {
        requestHeader = *(__OSContRequesFormat*)ptr;
        data->errno = CHNL_ERR(requestHeader);

        if (data->errno == 0) {
            data->type = requestHeader.typel << 8 | requestHeader.typeh;
            data->status = requestHeader.status;

            bits |= 1 << i;
        }
    }
    *pattern = bits;
}

void __osPackRequestData(u8 poll) {
    u8* ptr;
    __OSContRequesFormat requestHeader;
    s32 i;

    for (i = 0; i < ARRAY_COUNT(__osContPifRam.ramarray); i++) {
        __osContPifRam.ramarray[i] = 0;
    }

    __osContPifRam.status = CONT_CMD_READ_BUTTON;
    ptr = (u8*)__osContPifRam.ramarray;
    requestHeader.align = CONT_CMD_NOP;
    requestHeader.txsize = CONT_CMD_RESET_TX;
    requestHeader.rxsize = CONT_CMD_RESET_RX;
    requestHeader.poll = poll;
    requestHeader.typeh = CONT_CMD_NOP;
    requestHeader.typel = CONT_CMD_NOP;
    requestHeader.status = CONT_CMD_NOP;
    requestHeader.align1 = CONT_CMD_NOP;

    for (i = 0; i < __osMaxControllers; i++) {
        *(__OSContRequesFormat*)ptr = requestHeader;
        ptr += sizeof(requestHeader);
    }
    *ptr = CONT_CMD_END;
}
