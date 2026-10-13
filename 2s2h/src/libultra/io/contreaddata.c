#include "ultra64.h"
#include "PR/controller.h"
#include "macros.h"

void __osPackReadData(void);

s32 osContStartReadData(OSMesgQueue* mq) {
    s32 ret;
    int i;

    __osSiGetAccess();

    if (__osContLastPoll != 1) {
        __osPackReadData();
        __osSiRawStartDma(OS_WRITE, &__osContPifRam);
        osRecvMesg(mq, NULL, OS_MESG_BLOCK);
    }

    ret = __osSiRawStartDma(OS_READ, &__osContPifRam);
    __osContLastPoll = 1;

    __osSiRelAccess();

    return ret;
}

void osContGetReadData(OSContPad* data) {
    u8* ptr;
    __OSContReadFormat readformat;
    int i;

    ptr = (u8*)__osContPifRam.ramarray;
    for (i = 0; i < __osMaxControllers; i++, ptr += sizeof(__OSContReadFormat), data++) {
        readformat = *(__OSContReadFormat*)ptr;
        data->errno = (readformat.rxsize & 0xC0) >> 4;
        if (data->errno == 0) {
            data->button = readformat.button;
            data->stick_x = readformat.stick_x;
            data->stick_y = readformat.stick_y;
        }
    }
}

void __osPackReadData(void) {
    u8* ptr;
    __OSContReadFormat readformat;
    int i;

    ptr = (u8*)__osContPifRam.ramarray;
    for (i = 0; i < ARRAY_COUNT(__osContPifRam.ramarray); i++) {
        __osContPifRam.ramarray[i] = 0;
    }

    __osContPifRam.status = CONT_CMD_READ_BUTTON;
    readformat.dummy = 255;
    readformat.txsize = 1;
    readformat.rxsize = 4;
    readformat.cmd = 1;
    readformat.button = 65535;
    readformat.stick_x = -1;
    readformat.stick_y = -1;
    for (i = 0; i < __osMaxControllers; i++) {
        *(__OSContReadFormat*)ptr = readformat;
        ptr += sizeof(__OSContReadFormat);
    }
    *ptr = 254;
}
