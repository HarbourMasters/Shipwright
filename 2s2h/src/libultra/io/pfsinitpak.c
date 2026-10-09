#include "ultra64.h"
#include "PR/controller.h"

s32 __osPfsCheckRamArea(OSPfs* pfs);

s32 osPfsInitPak(OSMesgQueue* queue, OSPfs* pfs, s32 channel) {
    s32 ret;
    u16 sum;
    u16 isum;
    u8 buf[BLOCKSIZE];
    __OSPackId* id;
    __OSPackId newid;

    __osSiGetAccess();

    ret = __osPfsGetStatus(queue, channel);

    __osSiRelAccess();

    if (ret != 0) {
        return ret;
    }

    pfs->queue = queue;
    pfs->channel = channel;
    pfs->status = 0;

    ret = __osPfsCheckRamArea(pfs);
    if (ret != 0) {
        return ret;
    }

    ret = __osPfsSelectBank(pfs, PFS_ID_BANK_256K);
    if (ret != 0) {
        return ret;
    }

    ret = __osContRamRead(pfs->queue, pfs->channel, PFS_ID_0AREA, buf);
    if (ret != 0) {
        return ret;
    }

    __osIdCheckSum((u16*)buf, &sum, &isum);
    id = (__OSPackId*)buf;
    if ((id->checksum != sum) || (id->invertedChecksum != isum)) {
        ret = __osCheckPackId(pfs, id);
        if (ret != 0) {
            pfs->status |= PFS_ID_BROKEN;
            return ret;
        }
    }

    if (!(id->deviceid & 1)) {
        ret = __osRepairPackId(pfs, id, &newid);
        if (ret != 0) {
            if (ret == PFS_ERR_ID_FATAL) {
                pfs->status |= PFS_ID_BROKEN;
            }
            return ret;
        }
        id = &newid;
        if (!(id->deviceid & 1)) {
            return PFS_ERR_DEVICE;
        }
    }

    bcopy(id, pfs->id, BLOCKSIZE);

    pfs->version = id->version;
    pfs->banks = id->banks;
    pfs->inodeStartPage = 1 + DEF_DIR_PAGES + (2 * pfs->banks);
    pfs->dir_size = DEF_DIR_PAGES * PFS_ONE_PAGE;
    pfs->inode_table = 1 * PFS_ONE_PAGE;
    pfs->minode_table = (1 + pfs->banks) * PFS_ONE_PAGE;
    pfs->dir_table = pfs->minode_table + (pfs->banks * PFS_ONE_PAGE);

    ret = __osContRamRead(pfs->queue, pfs->channel, PFS_LABEL_AREA, pfs->label);
    if (ret != 0) {
        return ret;
    }

    ret = osPfsChecker(pfs);
    pfs->status |= PFS_INITIALIZED;

    return ret;
}

s32 __osPfsCheckRamArea(OSPfs* pfs) {
    s32 i = 0;
    s32 ret = 0;
    u8 writeBuf[BLOCKSIZE];
    u8 readBuf[BLOCKSIZE];
    u8 saveReg[BLOCKSIZE];

    ret = __osPfsSelectBank(pfs, PFS_ID_BANK_256K);
    if (ret != 0) {
        return ret;
    }

    ret = __osContRamRead(pfs->queue, pfs->channel, 0, saveReg);
    if (ret != 0) {
        return ret;
    }

    for (i = 0; i < BLOCKSIZE; i++) {
        writeBuf[i] = i;
    }

    ret = __osContRamWrite(pfs->queue, pfs->channel, 0, writeBuf, 0);
    if (ret != 0) {
        return ret;
    }

    ret = __osContRamRead(pfs->queue, pfs->channel, 0, readBuf);
    if (ret != 0) {
        return ret;
    }

    if (bcmp(writeBuf, readBuf, BLOCKSIZE) != 0) {
        return PFS_ERR_DEVICE;
    }

    return __osContRamWrite(pfs->queue, pfs->channel, 0, saveReg, 0);
}
