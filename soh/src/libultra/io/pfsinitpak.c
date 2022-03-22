#include "ultra64.h"
#include "global.h"

s32 osPfsInitPak(OSMesgQueue* queue, OSPfs* pfs, s32 channel) {
    s32 ret;
    u16 sum;
    u16 isum;
    u8 temp[BLOCKSIZE];
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

    if ((ret = __osPfsCheckRamArea(pfs)) != 0) {
        return ret;
    }
    if ((ret = __osPfsSelectBank(pfs, 0)) != 0) {
        return ret;
    }
    if ((ret = __osContRamRead(pfs->queue, pfs->channel, PFS_ID_0AREA, temp)) != 0) {
        return (ret);
    }

    __osIdCheckSum((u16*)temp, &sum, &isum);
    id = (__OSPackId*)temp;
    if ((id->checksum != sum) || (id->invertedChecksum != isum)) {
        if ((ret = __osCheckPackId(pfs, id)) != 0) {
            pfs->status |= PFS_ID_BROKEN;
            return ret;
        }
    }

    if ((id->deviceid & 0x01) == 0) {
        ret = __osRepairPackId(pfs, id, &newid);
        if (ret) {
            if (ret == PFS_ERR_ID_FATAL) {
                pfs->status |= PFS_ID_BROKEN;
            }
            return ret;
        }
        id = &newid;
        if ((id->deviceid & 0x01) == 0) {
            return PFS_ERR_DEVICE;
        }
    }

    bcopy(id, pfs->id, BLOCKSIZE);

    if (1) {}

    pfs->version = id->version;
    pfs->banks = id->banks;
    pfs->inodeStartPage = 1 + DEF_DIR_PAGES + (2 * pfs->banks);
    pfs->dir_size = DEF_DIR_PAGES * PFS_ONE_PAGE;
    pfs->inode_table = 1 * PFS_ONE_PAGE;
    pfs->minode_table = (1 + pfs->banks) * PFS_ONE_PAGE;
    pfs->dir_table = pfs->minode_table + (pfs->banks * PFS_ONE_PAGE);

    if ((ret = __osContRamRead(pfs->queue, pfs->channel, PFS_LABEL_AREA, pfs->label)) != 0) {
        return ret;
    }

    ret = osPfsChecker(pfs);
    pfs->status |= PFS_INITIALIZED;

    return ret;
}

s32 __osPfsCheckRamArea(OSPfs* pfs) {
    s32 i = 0;
    s32 ret = 0;
    u8 temp1[BLOCKSIZE];
    u8 temp2[BLOCKSIZE];
    u8 saveReg[BLOCKSIZE];

    if ((ret = __osPfsSelectBank(pfs, PFS_ID_BANK_256K)) != 0) {
        return ret;
    }
    if ((ret = __osContRamRead(pfs->queue, pfs->channel, 0, saveReg)) != 0) {
        return ret;
    }
    for (i = 0; i < BLOCKSIZE; i++) {
        temp1[i] = i;
    }
    if ((ret = __osContRamWrite(pfs->queue, pfs->channel, 0, temp1, 0)) != 0) {
        return ret;
    }
    if ((ret = __osContRamRead(pfs->queue, pfs->channel, 0, temp2)) != 0) {
        return ret;
    }
    if (bcmp(temp1, temp2, BLOCKSIZE) != 0) {
        return PFS_ERR_DEVICE;
    }
    return __osContRamWrite(pfs->queue, pfs->channel, 0, saveReg, 0);
}
