#include "global.h"

#define BANK_ADDR 0x400
#define MOTOR_ID 0x80

OSPifRam osPifBuffers[MAXCONTROLLERS];

s32 __osMotorAccess(OSPfs* pfs, u32 vibrate) {
    s32 i;
    s32 ret;
    u8* buf = (u8*)&osPifBuffers[pfs->channel];

    if (!(pfs->status & 8)) {
        return 5;
    }

    __osSiGetAccess();
    osPifBuffers[pfs->channel].status = 1;
    buf += pfs->channel;
    for (i = 0; i < BLOCKSIZE; i++) {
        ((__OSContRamHeader*)buf)->data[i] = vibrate;
    }

    __osContLastPoll = CONT_CMD_END;
    __osSiRawStartDma(OS_WRITE, &osPifBuffers[pfs->channel]);
    osRecvMesg(pfs->queue, NULL, OS_MESG_BLOCK);
    __osSiRawStartDma(OS_READ, &osPifBuffers[pfs->channel]);
    osRecvMesg(pfs->queue, NULL, OS_MESG_BLOCK);

    ret = ((__OSContRamHeader*)buf)->rxsize & 0xC0;
    if (!ret) {
        if (!vibrate) {
            if (((__OSContRamHeader*)buf)->datacrc != 0) {
                ret = PFS_ERR_CONTRFAIL;
            }
        } else {
            if (((__OSContRamHeader*)buf)->datacrc != 0xEB) {
                ret = PFS_ERR_CONTRFAIL;
            }
        }
    }

    __osSiRelAccess();

    return ret;
}

void _MakeMotorData(s32 channel, OSPifRam* buf) {
    u8* bufptr = (u8*)buf;
    __OSContRamHeader mempakwr;
    s32 i;

    mempakwr.unk_00 = 0xFF;
    mempakwr.txsize = 0x23;
    mempakwr.rxsize = 1;
    mempakwr.poll = 3; // write mempak
    mempakwr.hi = 0x600 >> 3;
    mempakwr.lo = (u8)(__osContAddressCrc(0x600) | (0x600 << 5));

    if (channel != 0) {
        for (i = 0; i < channel; ++i) {
            *bufptr++ = 0;
        }
    }

    *(__OSContRamHeader*)bufptr = mempakwr;
    bufptr += sizeof(mempakwr);
    *bufptr = 0xFE;
}

s32 osMotorInit(OSMesgQueue* ctrlrqueue, OSPfs* pfs, s32 channel) {
    s32 ret;
    u8 sp24[BLOCKSIZE];

    pfs->queue = ctrlrqueue;
    pfs->channel = channel;
    pfs->activebank = 0xFF;
    pfs->status = 0;

    ret = __osPfsSelectBank(pfs, 0xFE);
    if (ret == 2) {
        ret = __osPfsSelectBank(pfs, MOTOR_ID);
    }
    if (ret != 0) {
        return ret;
    }
    ret = __osContRamRead(ctrlrqueue, channel, BANK_ADDR, sp24);
    if (ret == 2) {
        ret = 4; // "Controller pack communication error"
    }
    if (ret != 0) {
        return ret;
    }
    if (sp24[BLOCKSIZE - 1] == 0xFE) {
        return 0xB;
    }
    ret = __osPfsSelectBank(pfs, MOTOR_ID);
    if (ret == 2) {
        ret = 4; // "Controller pack communication error"
    }
    if (ret != 0) {
        return ret;
    }
    ret = __osContRamRead(ctrlrqueue, channel, BANK_ADDR, sp24);
    if (ret == 2) {
        ret = 4; // "Controller pack communication error"
    }
    if (ret != 0) {
        return ret;
    }
    if (sp24[BLOCKSIZE - 1] != MOTOR_ID) {
        return 0xB;
    }
    if ((pfs->status & PFS_MOTOR_INITIALIZED) == 0) {
        _MakeMotorData(channel, &osPifBuffers[channel]);
    }
    pfs->status = PFS_MOTOR_INITIALIZED;

    return 0; // "Recognized rumble pak"
}
