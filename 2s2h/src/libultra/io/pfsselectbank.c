#include "ultra64.h"
#include "PR/controller.h"

s32 __osPfsSelectBank(OSPfs* pfs, u8 bank) {
    u8 buf[BLOCKSIZE];
    s32 i;
    s32 ret = 0;

    for (i = 0; i < BLOCKSIZE; i++) {
        buf[i] = bank;
    }

    ret = __osContRamWrite(pfs->queue, pfs->channel, 0x8000 / BLOCKSIZE, buf, 0);
    if (ret == 0) {
        pfs->activebank = bank;
    }
    return ret;
}
