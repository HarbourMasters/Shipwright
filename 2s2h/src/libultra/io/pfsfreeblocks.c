#include "ultra64.h"
#include "PR/controller.h"

s32 osPfsFreeBlocks(OSPfs* pfs, s32* leftoverBytes) {
    s32 j;
    s32 pages = 0;
    __OSInode inode;
    s32 ret = 0;
    u8 bank;
    s32 offset;

    if (!(pfs->status & PFS_INITIALIZED)) {
        return PFS_ERR_INVALID;
    }

    ret = __osCheckId(pfs);
    if (ret != 0) {
        return ret;
    }

    for (bank = PFS_ID_BANK_256K; bank < pfs->banks; bank++) {
        ret = __osPfsRWInode(pfs, &inode, PFS_READ, bank);
        if (ret != 0) {
            return ret;
        }

        offset = ((bank > PFS_ID_BANK_256K) ? 1 : pfs->inodeStartPage);
        for (j = offset; j < PFS_INODE_SIZE_PER_PAGE; j++) {
            if (inode.inodePage[j].ipage == PFS_PAGE_NOT_USED) {
                pages++;
            }
        }
    }

    *leftoverBytes = pages * PFS_ONE_PAGE * BLOCKSIZE;
    return 0;
}
