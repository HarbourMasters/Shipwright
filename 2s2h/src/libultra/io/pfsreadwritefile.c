#include "ultra64.h"
#include "PR/controller.h"

#define CHECK_IPAGE(p, pfs)                                                                                   \
    (((p).ipage >= (pfs).inodeStartPage) && ((p).inode_t.bank < (pfs).banks) && ((p).inode_t.page >= 0x01) && \
     ((p).inode_t.page < 0x80))

s32 __osPfsGetNextPage(OSPfs* pfs, u8* bank, __OSInode* inode, __OSInodeUnit* page) {
    s32 ret;

    if (page->inode_t.bank != *bank) {
        *bank = page->inode_t.bank;
        if ((ret = __osPfsRWInode(pfs, inode, PFS_READ, *bank)) != 0) {
            return ret;
        }
    }
    *page = inode->inodePage[page->inode_t.page];

    if (!CHECK_IPAGE(*page, *pfs)) {
        if (page->ipage == PFS_EOF) {
            return PFS_ERR_INVALID;
        }
        return PFS_ERR_INCONSISTENT;
    }
    return 0;
}

s32 osPfsReadWriteFile(OSPfs* pfs, s32 fileNo, u8 flag, s32 offset, s32 size, u8* data) {
    s32 ret;
    __OSDir dir;
    __OSInode inode;
    __OSInodeUnit curPage;
    s32 curBlock;
    s32 blockSize;
    u8* buffer;
    u8 bank;
    u16 blockno;

    if ((fileNo >= pfs->dir_size) || (fileNo < 0)) {
        return PFS_ERR_INVALID;
    }
    if ((size <= 0) || ((size % BLOCKSIZE) != 0)) {
        return PFS_ERR_INVALID;
    }
    if ((offset < 0) || ((offset % BLOCKSIZE) != 0)) {
        return PFS_ERR_INVALID;
    }
    if (!(pfs->status & PFS_INITIALIZED)) {
        return PFS_ERR_INVALID;
    }
    if (__osCheckId(pfs) == PFS_ERR_NEW_PACK) {
        return PFS_ERR_NEW_PACK;
    }
    if (pfs->activebank != PFS_ID_BANK_256K && (ret = __osPfsSelectBank(pfs, PFS_ID_BANK_256K)) != 0) {
        return ret;
    }
    if ((ret = __osContRamRead(pfs->queue, pfs->channel, pfs->dir_table + fileNo, (u8*)&dir)) != 0) {
        return ret;
    }
    if ((dir.company_code == 0) || (dir.game_code == 0)) {
        return PFS_ERR_INVALID;
    }
    if (!CHECK_IPAGE(dir.start_page, *pfs)) {
        if (dir.start_page.ipage == PFS_EOF) {
            return PFS_ERR_INVALID;
        }
        return PFS_ERR_INCONSISTENT;
    }
    if ((flag == PFS_READ) && ((dir.status & PFS_WRITTEN) == 0)) {
        return PFS_ERR_BAD_DATA;
    }

    bank = 255;
    curBlock = offset / BLOCKSIZE;
    curPage = dir.start_page;

    while (curBlock >= 8) {
        if ((ret = __osPfsGetNextPage(pfs, &bank, &inode, &curPage)) != 0) {
            return ret;
        }
        curBlock -= 8;
    }

    blockSize = size / BLOCKSIZE;
    buffer = data;

    while (blockSize > 0) {
        if (curBlock == 8) {
            if ((ret = __osPfsGetNextPage(pfs, &bank, &inode, &curPage)) != 0) {
                return ret;
            }
            curBlock = 0;
        }
        if (pfs->activebank != curPage.inode_t.bank && (ret = __osPfsSelectBank(pfs, curPage.inode_t.bank)) != 0) {
            return ret;
        }

        blockno = curPage.inode_t.page * PFS_ONE_PAGE + curBlock;
        if (flag == PFS_READ) {
            ret = __osContRamRead(pfs->queue, pfs->channel, blockno, buffer);
        } else {
            ret = __osContRamWrite(pfs->queue, pfs->channel, blockno, buffer, 0);
        }
        if (ret != 0) {
            return ret;
        }

        buffer += BLOCKSIZE;
        curBlock++;
        blockSize--;
    }

    if (flag == PFS_WRITE && !(dir.status & PFS_WRITTEN)) {
        dir.status |= PFS_WRITTEN;
        if (pfs->activebank != PFS_ID_BANK_256K && (ret = __osPfsSelectBank(pfs, PFS_ID_BANK_256K)) != 0) {
            return ret;
        }
        if ((ret = __osContRamWrite(pfs->queue, pfs->channel, pfs->dir_table + fileNo, (u8*)&dir, 0)) != 0) {
            return ret;
        }
    }

    return __osPfsGetStatus(pfs->queue, pfs->channel);
}
