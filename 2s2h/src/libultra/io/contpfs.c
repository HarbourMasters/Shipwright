#include "ultra64.h"
#include "PR/controller.h"
#include "alignment.h"

__OSInode __osPfsInodeCache ALIGNED(8);

s32 __osPfsInodeCacheChannel = -1;
u8 __osPfsInodeCacheBank = 250;

u16 __osSumcalc(u8* ptr, s32 length) {
    s32 i;
    u32 sum = 0;
    u8* temp = ptr;

    for (i = 0; i < length; i++) {
        sum += *temp++;
    }
    return sum & 0xFFFF;
}

s32 __osIdCheckSum(u16* ptr, u16* checkSum, u16* idSum) {
    u16 data = 0;
    u32 i;

    *checkSum = *idSum = 0;
    for (i = 0; i < ((sizeof(__OSPackId) - sizeof(u32)) / sizeof(u8)); i += 2) {
        data = *((u16*)((u32)ptr + i));
        *checkSum += data;
        *idSum += ~data;
    }
    return 0;
}

s32 __osRepairPackId(OSPfs* pfs, __OSPackId* badid, __OSPackId* newid) {
    s32 ret = 0;
    u8 buf[BLOCKSIZE];
    u8 comp[BLOCKSIZE];
    u8 mask = 0;
    s32 i = 0;
    s32 j = 0;
    u16 index[4];

    newid->repaired = 0xFFFFFFFF;
    newid->random = osGetCount();
    newid->serialMid = badid->serialMid;
    newid->serialLow = badid->serialLow;

    if (pfs->activebank != 0) {
        ret = __osPfsSelectBank(pfs, PFS_ID_BANK_256K);
        if (ret != 0) {
            return ret;
        }
    }

    do {
        ret = __osPfsSelectBank(pfs, j);
        if (ret != 0) {
            return ret;
        }

        ret = __osContRamRead(pfs->queue, pfs->channel, 0, buf);
        if (ret != 0) {
            return ret;
        }
        buf[0] = j | 0x80;
        for (i = 1; i < BLOCKSIZE; i++) {
            buf[i] = ~buf[i];
        }

        ret = __osContRamWrite(pfs->queue, pfs->channel, 0, buf, 0);
        if (ret != 0) {
            return ret;
        }

        ret = __osContRamRead(pfs->queue, pfs->channel, 0, comp);
        if (ret != 0) {
            return (ret);
        }

        for (i = 0; i < BLOCKSIZE; i++) {
            if (comp[i] != buf[i]) {
                break;
            }
        }

        if (i != BLOCKSIZE) {
            break;
        }

        if (j > 0) {
            ret = __osPfsSelectBank(pfs, PFS_ID_BANK_256K);
            if (ret != 0) {
                return ret;
            }

            ret = __osContRamRead(pfs->queue, pfs->channel, 0, buf);
            if (ret != 0) {
                return ret;
            }

            if (buf[0] != 0x80) {
                break;
            }
        }

        j++;
    } while (j < PFS_MAX_BANKS);

    if (pfs->activebank != 0) {
        ret = __osPfsSelectBank(pfs, PFS_ID_BANK_256K);
        if (ret != 0) {
            return ret;
        }
    }

    mask = (j > 0) ? 1 : 0;
    newid->deviceid = (badid->deviceid & 0xFFFE) | mask;
    newid->banks = j;
    newid->version = badid->version;
    __osIdCheckSum((u16*)newid, &newid->checksum, &newid->invertedChecksum);

    index[0] = PFS_ID_0AREA;
    index[1] = PFS_ID_1AREA;
    index[2] = PFS_ID_2AREA;
    index[3] = PFS_ID_3AREA;

    for (i = 0; i < 4; i++) {
        ret = __osContRamWrite(pfs->queue, pfs->channel, index[i], (u8*)newid, PFS_FORCE);
        if (ret != 0) {
            return ret;
        }
    }

    ret = __osContRamRead(pfs->queue, pfs->channel, PFS_ID_0AREA, buf);
    if (ret != 0) {
        return ret;
    }

    for (i = 0; i < BLOCKSIZE; i++) {
        if (buf[i] != *(u8*)((s32)newid + i)) {
            return PFS_ERR_DEVICE;
        }
    }
    return 0;
}

s32 __osCheckPackId(OSPfs* pfs, __OSPackId* check) {
    u16 index[4];
    s32 ret = 0;
    u16 sum;
    u16 idSum;
    s32 i;
    s32 j;

    if (pfs->activebank != 0) {
        ret = __osPfsSelectBank(pfs, PFS_ID_BANK_256K);
        if (ret != 0) {
            return ret;
        }
    }

    index[0] = PFS_ID_0AREA;
    index[1] = PFS_ID_1AREA;
    index[2] = PFS_ID_2AREA;
    index[3] = PFS_ID_3AREA;
    for (i = 1; i < 4; i++) {
        ret = __osContRamRead(pfs->queue, pfs->channel, index[i], (u8*)check);
        if (ret != 0) {
            return ret;
        }
        __osIdCheckSum((u16*)check, &sum, &idSum);
        if ((check->checksum == sum) && (check->invertedChecksum == idSum)) {
            break;
        }
    }
    if (i == 4) {
        return PFS_ERR_ID_FATAL;
    }

    for (j = 0; j < 4; j++) {
        if (j != i) {
            ret = __osContRamWrite(pfs->queue, pfs->channel, index[j], (u8*)check, PFS_FORCE);
            if (ret != 0) {
                return ret;
            }
        }
    }
    return 0;
}

s32 __osGetId(OSPfs* pfs) {
    u16 sum;
    u16 isum;
    u8 buf[BLOCKSIZE];
    __OSPackId* id;
    __OSPackId newid;
    s32 ret;

    if (pfs->activebank != 0) {
        ret = __osPfsSelectBank(pfs, PFS_ID_BANK_256K);
        if (ret != 0) {
            return ret;
        }
    }

    ret = __osContRamRead(pfs->queue, pfs->channel, PFS_ID_0AREA, buf);
    if (ret != 0) {
        return ret;
    }

    __osIdCheckSum((u16*)buf, &sum, &isum);
    id = (__OSPackId*)buf;
    if ((id->checksum != sum) || (id->invertedChecksum != isum)) {
        ret = __osCheckPackId(pfs, id);
        if (ret == PFS_ERR_ID_FATAL) {
            ret = __osRepairPackId(pfs, id, &newid);
            if (ret != 0) {
                return ret;
            }
            id = &newid;
        } else if (ret != 0) {
            return ret;
        }
    }

    if (!(id->deviceid & 1)) {
        ret = __osRepairPackId(pfs, id, &newid);
        if (ret) {
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

    return 0;
}

s32 __osCheckId(OSPfs* pfs) {
    u8 buf[BLOCKSIZE];
    s32 ret;

    if (pfs->activebank != 0) {
        ret = __osPfsSelectBank(pfs, PFS_ID_BANK_256K);
        if (ret == PFS_ERR_NEW_PACK) {
            ret = __osPfsSelectBank(pfs, PFS_ID_BANK_256K);
        }
        if (ret != 0) {
            return ret;
        }
    }

    ret = __osContRamRead(pfs->queue, pfs->channel, PFS_ID_0AREA, buf);
    if (ret != 0) {
        if (ret != PFS_ERR_NEW_PACK) {
            return ret;
        }

        ret = __osContRamRead(pfs->queue, pfs->channel, PFS_ID_0AREA, buf);
        if (ret != 0) {
            return ret;
        }
    }

    if (bcmp(pfs->id, buf, BLOCKSIZE) != 0) {
        return PFS_ERR_NEW_PACK;
    }

    return 0;
}

s32 __osPfsRWInode(OSPfs* pfs, __OSInode* inode, u8 flag, u8 bank) {
    u8 sum;
    s32 j;
    s32 ret;
    s32 offset;
    u8* addr;

    if (flag == PFS_READ && bank == __osPfsInodeCacheBank && (pfs->channel == __osPfsInodeCacheChannel)) {
        bcopy(&__osPfsInodeCache, inode, sizeof(__OSInode));
        return 0;
    }

    if (pfs->activebank != 0) {
        ret = __osPfsSelectBank(pfs, PFS_ID_BANK_256K);
        if (ret != 0) {
            return ret;
        }
    }

    offset = ((bank > 0) ? 1 : pfs->inodeStartPage);

    if (flag == PFS_WRITE) {
        inode->inodePage[0].inode_t.page =
            __osSumcalc((u8*)(inode->inodePage + offset), (PFS_INODE_SIZE_PER_PAGE - offset) * 2);
    }

    for (j = 0; j < PFS_ONE_PAGE; j++) {
        addr = (u8*)(((u8*)inode) + (j * BLOCKSIZE));
        if (flag == PFS_WRITE) {
            ret = __osContRamWrite(pfs->queue, pfs->channel, pfs->inode_table + (bank * PFS_ONE_PAGE) + j, addr, 0);
            ret = __osContRamWrite(pfs->queue, pfs->channel, pfs->minode_table + (bank * PFS_ONE_PAGE) + j, addr, 0);
        } else {
            ret = __osContRamRead(pfs->queue, pfs->channel, pfs->inode_table + (bank * PFS_ONE_PAGE) + j, addr);
        }
        if (ret) {
            return ret;
        }
    }

    if (flag == PFS_READ) {
        sum = __osSumcalc((u8*)(inode->inodePage + offset), (PFS_INODE_SIZE_PER_PAGE - offset) * 2);
        if (sum != inode->inodePage[0].inode_t.page) {
            for (j = 0; j < PFS_ONE_PAGE; j++) {
                addr = (u8*)(((u8*)inode) + (j * BLOCKSIZE));
                ret = __osContRamRead(pfs->queue, pfs->channel, pfs->minode_table + (bank * PFS_ONE_PAGE) + j, addr);
            }
            sum = __osSumcalc((u8*)(inode->inodePage + offset), (PFS_INODE_SIZE_PER_PAGE - offset) * 2);
            if (sum != inode->inodePage[0].inode_t.page) {
                return PFS_ERR_INCONSISTENT;
            }
            for (j = 0; j < PFS_ONE_PAGE; j++) {
                addr = (u8*)(((u8*)inode) + (j * BLOCKSIZE));
                ret = __osContRamWrite(pfs->queue, pfs->channel, pfs->inode_table + (bank * PFS_ONE_PAGE) + j, addr, 0);
            }
        }
    }
    __osPfsInodeCacheBank = bank;
    bcopy(inode, &__osPfsInodeCache, sizeof(__OSInode));
    __osPfsInodeCacheChannel = pfs->channel;

    return 0;
}
