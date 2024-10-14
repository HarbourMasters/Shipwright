#include "global.h"
#include <stdlib.h>
#include "soh/framebuffer_effects.h"

uintptr_t sSysCfbFbPtr[2];
uintptr_t sSysCfbEnd;

void SysCfb_Init(s32 n64dd) {
    u32 screenSize;
    uintptr_t tmpFbEnd;

    /*
    if (osMemSize >= 0x800000) {
        // "8MB or more memory is installed"
        osSyncPrintf("８Ｍバイト以上のメモリが搭載されています\n");
        tmpFbEnd = 0x8044BE80;
        if (n64dd == 1) {
            osSyncPrintf("RAM 8M mode (N64DD対応)\n"); // "RAM 8M mode (N64DD compatible)"
            sSysCfbEnd = 0x805FB000;
        } else {
            // "The margin for this version is %dK bytes"
            osSyncPrintf("このバージョンのマージンは %dK バイトです\n", (0x4BC00 / 1024));
            sSysCfbEnd = tmpFbEnd;
        }
    } else if (osMemSize >= 0x400000) {
        osSyncPrintf("RAM4M mode\n");
        sSysCfbEnd = 0x80400000;
    } else {
        LOG_HUNGUP_THREAD();
    }
    */

    screenSize = SCREEN_WIDTH * SCREEN_HEIGHT;
    //sSysCfbEnd &= ~0x3F;
    // "The final address used by the system is %08x"
    osSyncPrintf("システムが使用する最終アドレスは %08x です\n", sSysCfbEnd);
    //sSysCfbFbPtr[0] = sSysCfbEnd - (screenSize * 4);
    //sSysCfbFbPtr[1] = sSysCfbEnd - (screenSize * 2);
    sSysCfbFbPtr[0] = malloc(screenSize * 4);
    sSysCfbFbPtr[1] = malloc(screenSize * 4);

    // "Frame buffer addresses are %08x and %08x"
    //osSyncPrintf("フレームバッファのアドレスは %08x と %08x です\n", sSysCfbFbPtr[0], sSysCfbFbPtr[1]);

    // SOH [Port] Inform LUS on resolution changes
    FB_CreateFramebuffers();
}

void SysCfb_Reset() {
    sSysCfbFbPtr[0] = 0;
    sSysCfbFbPtr[1] = 0;
    sSysCfbEnd = 0;
}

uintptr_t SysCfb_GetFbPtr(s32 idx) {
    if (idx < 2) {
        return sSysCfbFbPtr[idx];
    }
    return 0;
}

uintptr_t SysCfb_GetFbEnd() {
    return sSysCfbEnd;
}
