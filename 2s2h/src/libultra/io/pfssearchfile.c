#include "ultra64.h"
#include "PR/controller.h"

s32 osPfsFindFile(OSPfs* pfs, u16 companyCode, u32 gameCode, u8* gameName, u8* extName, s32* fileNo) {
    s32 j;
    s32 i;
    __OSDir dir;
    s32 ret = 0;
    s32 err;

    if (!(pfs->status & PFS_INITIALIZED)) {
        return PFS_ERR_INVALID;
    }

    if ((ret = __osCheckId(pfs)) != 0) {
        return ret;
    }

    for (j = 0; j < pfs->dir_size; j++) {
        if ((ret = __osContRamRead(pfs->queue, pfs->channel, pfs->dir_table + j, (u8*)&dir)) != 0) {
            return ret;
        }
        if ((ret = __osPfsGetStatus(pfs->queue, pfs->channel)) != 0) {
            return ret;
        }

        if ((dir.company_code == companyCode) && (dir.game_code == gameCode)) {
            err = 0;
            if (gameName != 0) {
                for (i = 0; i < PFS_FILE_NAME_LEN; i++) {
                    if (dir.game_name[i] != gameName[i]) {
                        err = 1;
                        break;
                    }
                }
            }
            if ((extName != 0) && (err == 0)) {
                for (i = 0; i < PFS_FILE_EXT_LEN; i++) {
                    if (dir.ext_name[i] != extName[i]) {
                        err = 1;
                        break;
                    }
                }
            }
            if (err == 0) {
                *fileNo = j;
                return ret;
            }
        }
    }
    *fileNo = -1;
    return PFS_ERR_INVALID;
}
