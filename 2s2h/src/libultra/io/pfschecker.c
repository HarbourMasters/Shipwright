#include "ultra64.h"
#include "PR/controller.h"

#define CHECK_IPAGE(p)                                                                                      \
    (((p).ipage >= pfs->inodeStartPage) && ((p).inode_t.bank < pfs->banks) && ((p).inode_t.page >= 0x01) && \
     ((p).inode_t.page < 0x80))

s32 corrupted_init(OSPfs* pfs, __OSInodeCache* cache);
s32 corrupted(OSPfs* pfs, __OSInodeUnit fpage, __OSInodeCache* cache);

s32 osPfsChecker(OSPfs* pfs) {
    s32 j;
    s32 ret;
    __OSInodeUnit next;
    __OSInode checkedInode;
    __OSInode tempInode;
    __OSDir tempDir;
    __OSInodeUnit nextNodeInFile[16];
    __OSInodeCache cache;
    s32 fixed = 0;
    u8 bank = 254;
    u8 prevBank = 254;
    s32 cc;
    s32 cl;
    s32 offset;

    ret = __osCheckId(pfs);
    if (ret == PFS_ERR_NEW_PACK) {
        ret = __osGetId(pfs);
    }

    if (ret != 0) {
        return ret;
    }

    ret = corrupted_init(pfs, &cache);
    if (ret != 0) {
        return ret;
    }

    for (j = 0; j < pfs->dir_size; j++) {
        ret = __osContRamRead(pfs->queue, pfs->channel, pfs->dir_table + j, (u8*)&tempDir);
        if (ret != 0) {
            return ret;
        }

        if ((tempDir.company_code != 0) || (tempDir.game_code != 0)) {
            if ((tempDir.company_code == 0) || (tempDir.game_code == 0)) {
                cc = -1;
            } else {
                next = tempDir.start_page;
                cl = cc = 0;
                bank = 255;

                while (CHECK_IPAGE(next)) {
                    if (bank != next.inode_t.bank) {
                        bank = next.inode_t.bank;
                        if (prevBank != bank) {
                            ret = __osPfsRWInode(pfs, &tempInode, PFS_READ, bank);
                            prevBank = bank;
                        }

                        if ((ret != 0) && (ret != PFS_ERR_INCONSISTENT)) {
                            return ret;
                        }
                    }

                    cc = corrupted(pfs, next, &cache) - cl;
                    if (cc != 0) {
                        break;
                    }

                    cl = 1;
                    next = tempInode.inodePage[next.inode_t.page];
                }
            }

            if ((cc != 0) || (next.ipage != PFS_EOF)) {
                bzero(&tempDir, sizeof(__OSDir));
                if (pfs->activebank != 0) {
                    ret = __osPfsSelectBank(pfs, PFS_ID_BANK_256K);
                    if (ret != 0) {
                        return ret;
                    }
                }

                ret = __osContRamWrite(pfs->queue, pfs->channel, pfs->dir_table + j, (u8*)&tempDir, 0);
                if (ret != 0) {
                    return ret;
                }

                fixed++;
            }
        }
    }

    for (j = 0; j < pfs->dir_size; j++) {
        ret = __osContRamRead(pfs->queue, pfs->channel, pfs->dir_table + j, (u8*)&tempDir);
        if (ret != 0) {
            return ret;
        }

        if ((tempDir.company_code != 0) && (tempDir.game_code != 0) &&
            (tempDir.start_page.ipage >= (u16)pfs->inodeStartPage)) { // cast required
            nextNodeInFile[j].ipage = tempDir.start_page.ipage;
        } else {
            nextNodeInFile[j].ipage = 0;
        }
    }

    for (bank = 0; bank < pfs->banks; bank++) {
        ret = __osPfsRWInode(pfs, &tempInode, PFS_READ, bank);
        if ((ret != 0) && (ret != PFS_ERR_INCONSISTENT)) {
            return (ret);
        }
        offset = ((bank > PFS_ID_BANK_256K) ? 1 : pfs->inodeStartPage);
        for (j = 0; j < offset; j++) {
            checkedInode.inodePage[j].ipage = tempInode.inodePage[j].ipage;
        }
        for (; j < PFS_INODE_SIZE_PER_PAGE; j++) {
            checkedInode.inodePage[j].ipage = PFS_PAGE_NOT_USED;
        }

        for (j = 0; j < pfs->dir_size; j++) {
            while (nextNodeInFile[j].inode_t.bank == bank &&
                   nextNodeInFile[j].ipage >= (u16)pfs->inodeStartPage) { // cast required
                u8 val = nextNodeInFile[j].inode_t.page;

                nextNodeInFile[j] = checkedInode.inodePage[val] = tempInode.inodePage[val];
            }
        }

        ret = __osPfsRWInode(pfs, &checkedInode, PFS_WRITE, bank);
        if (ret != 0) {
            return ret;
        }
    }

    if (fixed != 0) {
        pfs->status |= PFS_CORRUPTED;
    } else {
        pfs->status &= ~PFS_CORRUPTED;
    }

    return 0;
}

s32 corrupted_init(OSPfs* pfs, __OSInodeCache* cache) {
    s32 i;
    s32 n;
    s32 offset;
    u8 bank;
    __OSInodeUnit tpage;
    __OSInode tempInode;
    s32 ret;

    for (i = 0; i < PFS_INODE_DIST_MAP; i++) {
        cache->map[i] = 0;
    }
    cache->bank = 255;

    for (bank = PFS_ID_BANK_256K; bank < pfs->banks; bank++) {
        offset = ((bank > PFS_ID_BANK_256K) ? 1 : pfs->inodeStartPage);
        ret = __osPfsRWInode(pfs, &tempInode, PFS_READ, bank);
        if ((ret != 0) && (ret != PFS_ERR_INCONSISTENT)) {
            return ret;
        }

        for (i = offset; i < PFS_INODE_SIZE_PER_PAGE; i++) {
            tpage = tempInode.inodePage[i];
            if ((tpage.ipage >= pfs->inodeStartPage) && (tpage.inode_t.bank != bank)) {
                n = ((tpage.inode_t.page & 0x7F) / PFS_SECTOR_SIZE) +
                    PFS_SECTOR_PER_BANK * (tpage.inode_t.bank % PFS_BANK_LAPPED_BY);
                cache->map[n] |= (1 << (bank % PFS_BANK_LAPPED_BY));
            }
        }
    }
    return 0;
}

s32 corrupted(OSPfs* pfs, __OSInodeUnit fpage, __OSInodeCache* cache) {
    s32 j;
    s32 n;
    s32 hit = 0;
    u8 bank;
    s32 offset;
    s32 ret = 0;

    n = (fpage.inode_t.page / PFS_SECTOR_SIZE) + PFS_SECTOR_PER_BANK * (fpage.inode_t.bank % PFS_BANK_LAPPED_BY);

    for (bank = PFS_ID_BANK_256K; bank < pfs->banks; bank++) {
        offset = ((bank > PFS_ID_BANK_256K) ? 1 : pfs->inodeStartPage);
        if ((bank == fpage.inode_t.bank) || (cache->map[n] & (1 << (bank % PFS_BANK_LAPPED_BY))) != 0) {
            if (bank != cache->bank) {
                ret = __osPfsRWInode(pfs, &(cache->inode), PFS_READ, bank);
                if (ret && (ret != PFS_ERR_INCONSISTENT)) {
                    return ret;
                }
                cache->bank = bank;
            }

            for (j = offset; ((hit < 2) && (j < PFS_INODE_SIZE_PER_PAGE)); j++) {
                if (cache->inode.inodePage[j].ipage == fpage.ipage) {
                    hit++;
                }
            }

            if (hit >= 2) {
                return 2;
            }
        }
    }

    return hit;
}
