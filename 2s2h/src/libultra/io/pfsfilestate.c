#include "ultra64.h"
#include "PR/controller.h"
#include "libc/stdbool.h"

s32 osPfsFileState(OSPfs* pfs, s32 fileNo, OSPfsState* state) {
    s32 ret;
    s32 pages;
    __OSInode inode;
    __OSDir dir;
    __OSInodeUnit page;
    u8 bank;

    if (fileNo >= pfs->dir_size || fileNo < 0) {
        return PFS_ERR_INVALID;
    }

    if (!(pfs->status & PFS_INITIALIZED)) {
        return PFS_ERR_INVALID;
    }

    ret = __osCheckId(pfs);
    if (ret != 0) {
        return ret;
    }

    if (pfs->activebank != 0) {
        ret = __osPfsSelectBank(pfs, PFS_ID_BANK_256K);
        if (ret != 0) {
            return ret;
        }
    }

    ret = __osContRamRead(pfs->queue, pfs->channel, pfs->dir_table + fileNo, (u8*)&dir);
    if (ret != 0) {
        return ret;
    }

    if ((dir.company_code == 0) || (dir.game_code == 0)) {
        return PFS_ERR_INVALID;
    }

    page = dir.start_page;
    pages = 0;
    bank = 0xFF;

    while (true) {
        if (page.ipage < pfs->inodeStartPage) {
            break;
        }
        if (page.inode_t.bank != bank) {
            bank = page.inode_t.bank;

            ret = __osPfsRWInode(pfs, &inode, PFS_READ, bank);
            if (ret != 0) {
                return ret;
            }
        }
        pages++;
        page = inode.inodePage[page.inode_t.page];
    }
    if (page.ipage != PFS_EOF) {
        return PFS_ERR_INCONSISTENT;
    }

    state->file_size = pages * PFS_ONE_PAGE * BLOCKSIZE;
    state->company_code = dir.company_code;
    state->game_code = dir.game_code;
    bcopy(&dir.game_name, state->game_name, PFS_FILE_NAME_LEN);
    bcopy(&dir.ext_name, state->ext_name, PFS_FILE_EXT_LEN);

    return __osPfsGetStatus(pfs->queue, pfs->channel);
}
