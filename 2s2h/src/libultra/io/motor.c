#include "ultra64.h"
#include "PR/os_motor.h"
#include "PR/controller.h"
#include "alignment.h"

#define BANK_ADDR 0x400
#define MOTOR_ID 0x80

OSPifRam __MotorDataBuf[MAXCONTROLLERS] ALIGNED(16);

s32 __osPfsSelectBank(OSPfs* pfs, u8 bank);

s32 __osMotorAccess(OSPfs* pfs, s32 flag) {
    s32 i;
    s32 ret;
    u8* ptr = (u8*)&__MotorDataBuf[pfs->channel];

    if (!(pfs->status & PFS_MOTOR_INITIALIZED)) {
        return PFS_ERR_INVALID;
    }

    __osSiGetAccess();
    __MotorDataBuf[pfs->channel].status = CONT_CMD_EXE;
    ptr += pfs->channel;
    for (i = 0; i < BLOCKSIZE; i++) {
        READFORMAT(ptr)->data[i] = flag;
    }

    __osContLastPoll = CONT_CMD_END;
    __osSiRawStartDma(OS_WRITE, &__MotorDataBuf[pfs->channel]);
    osRecvMesg(pfs->queue, NULL, OS_MESG_BLOCK);
    __osSiRawStartDma(OS_READ, &__MotorDataBuf[pfs->channel]);
    osRecvMesg(pfs->queue, NULL, OS_MESG_BLOCK);

    ret = READFORMAT(ptr)->rxsize & CHNL_ERR_MASK;
    if (!ret) {
        if (!flag) {
            if (READFORMAT(ptr)->datacrc != 0) {
                ret = PFS_ERR_CONTRFAIL;
            }
        } else {
            if (READFORMAT(ptr)->datacrc != 0xEB) {
                ret = PFS_ERR_CONTRFAIL;
            }
        }
    }

    __osSiRelAccess();

    return ret;
}

void _MakeMotorData(s32 channel, OSPifRam* mdata) {
    u8* ptr = (u8*)mdata->ramarray;
    __OSContRamReadFormat ramreadformat;
    s32 i;

    ramreadformat.dummy = CONT_CMD_NOP;
    ramreadformat.txsize = CONT_CMD_WRITE_PAK_TX;
    ramreadformat.rxsize = CONT_CMD_WRITE_PAK_RX;
    ramreadformat.cmd = CONT_CMD_WRITE_PAK;
    ramreadformat.hi = CONT_BLOCK_RUMBLE >> 3;
    ramreadformat.lo = (u8)(__osContAddressCrc(CONT_BLOCK_RUMBLE) | (CONT_BLOCK_RUMBLE << 5));

    if (channel != 0) {
        for (i = 0; i < channel; i++) {
            *ptr++ = 0;
        }
    }

    *READFORMAT(ptr) = ramreadformat;
    ptr += sizeof(ramreadformat);
    *ptr = CONT_CMD_END;
}

s32 osMotorInit(OSMesgQueue* mq, OSPfs* pfs, s32 channel) {
    s32 ret;
    u8 temp[BLOCKSIZE];

    pfs->queue = mq;
    pfs->channel = channel;
    pfs->activebank = 0xFF;
    pfs->status = 0;

    ret = __osPfsSelectBank(pfs, 0xFE);
    if (ret == PFS_ERR_NEW_PACK) {
        ret = __osPfsSelectBank(pfs, MOTOR_ID);
    }
    if (ret != 0) {
        return ret;
    }
    ret = __osContRamRead(mq, channel, CONT_BLOCK_DETECT, temp);
    if (ret == PFS_ERR_NEW_PACK) {
        ret = PFS_ERR_CONTRFAIL;
    }
    if (ret != 0) {
        return ret;
    }
    if (temp[BLOCKSIZE - 1] == 0xFE) {
        return PFS_ERR_DEVICE;
    }
    ret = __osPfsSelectBank(pfs, MOTOR_ID);
    if (ret == 2) {
        ret = PFS_ERR_CONTRFAIL;
    }
    if (ret != 0) {
        return ret;
    }
    ret = __osContRamRead(mq, channel, BANK_ADDR, temp);
    if (ret == PFS_ERR_NEW_PACK) {
        ret = PFS_ERR_CONTRFAIL;
    }
    if (ret != 0) {
        return ret;
    }
    if (temp[BLOCKSIZE - 1] != MOTOR_ID) {
        return PFS_ERR_DEVICE;
    }
    if (!(pfs->status & PFS_MOTOR_INITIALIZED)) {
        _MakeMotorData(channel, &__MotorDataBuf[channel]);
    }
    pfs->status = PFS_MOTOR_INITIALIZED;
    return 0;
}
