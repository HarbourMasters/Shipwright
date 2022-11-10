#include "global.h"
#include "vt.h"

#include <string.h>

//#define KALEIDO_OVERLAY(name)                                                                                \
//    {                                                                                                        \
//        NULL, (uintptr_t)_ovl_##name##SegmentRomStart, (uintptr_t)_ovl_##name##SegmentRomEnd, _ovl_##name##SegmentStart, \
//            _ovl_##name##SegmentEnd, 0, #name,                                                               \
//    }

#define KALEIDO_OVERLAY(name) \
{ 0 }

KaleidoMgrOverlay gKaleidoMgrOverlayTable[] = {
    KALEIDO_OVERLAY(kaleido_scope),
    KALEIDO_OVERLAY(player_actor),
};

void* sKaleidoAreaPtr = NULL;
KaleidoMgrOverlay* gKaleidoMgrCurOvl = NULL;
u8 gBossMarkState = 0;

void KaleidoManager_LoadOvl(KaleidoMgrOverlay* ovl) {
    //LOG_CHECK_NULL_POINTER("KaleidoArea_allocp", sKaleidoAreaPtr);

    ovl->loadedRamAddr = sKaleidoAreaPtr;
    Overlay_Load(ovl->vromStart, ovl->vromEnd, ovl->vramStart, ovl->vramEnd, ovl->loadedRamAddr);

    //osSyncPrintf(VT_FGCOL(GREEN));
    //osSyncPrintf("OVL(k):Seg:%08x-%08x Ram:%08x-%08x Off:%08x %s\n", ovl->vramStart, ovl->vramEnd, ovl->loadedRamAddr,
                 //(uintptr_t)ovl->loadedRamAddr + (uintptr_t)ovl->vramEnd - (uintptr_t)ovl->vramStart,
                 //(uintptr_t)ovl->vramStart - (uintptr_t)ovl->loadedRamAddr, ovl->name);
    //osSyncPrintf(VT_RST);

    ovl->offset = (uintptr_t)ovl->loadedRamAddr - (uintptr_t)ovl->vramStart;
    gKaleidoMgrCurOvl = ovl;
}

void KaleidoManager_ClearOvl(KaleidoMgrOverlay* ovl) {
    if (ovl->loadedRamAddr != NULL) {
        ovl->offset = 0;
        memset(ovl->loadedRamAddr, 0, (uintptr_t)ovl->vramEnd - (uintptr_t)ovl->vramStart);
        ovl->loadedRamAddr = NULL;
        gKaleidoMgrCurOvl = NULL;
    }
}

void KaleidoManager_Init(PlayState* play) {
    ptrdiff_t largestSize = 0;
    ptrdiff_t size;
    u32 i;

    for (i = 0; i < ARRAY_COUNT(gKaleidoMgrOverlayTable); i++) {
        size = (uintptr_t)gKaleidoMgrOverlayTable[i].vramEnd - (uintptr_t)gKaleidoMgrOverlayTable[i].vramStart;
        if (size > largestSize) {
            largestSize = size;
        }
    }

    osSyncPrintf(VT_FGCOL(GREEN));
    osSyncPrintf("KaleidoArea の最大サイズは %d バイトを確保します\n", largestSize);
    osSyncPrintf(VT_RST);

    sKaleidoAreaPtr = GAMESTATE_ALLOC_MC(&play->state, largestSize);
    LOG_CHECK_NULL_POINTER("KaleidoArea_allocp", sKaleidoAreaPtr);

    osSyncPrintf(VT_FGCOL(GREEN));
    osSyncPrintf("KaleidoArea %08x - %08x\n", sKaleidoAreaPtr, (uintptr_t)sKaleidoAreaPtr + largestSize);
    osSyncPrintf(VT_RST);

    gKaleidoMgrCurOvl = 0;
}

void KaleidoManager_Destroy() {
    if (gKaleidoMgrCurOvl != NULL) {
        KaleidoManager_ClearOvl(gKaleidoMgrCurOvl);
        gKaleidoMgrCurOvl = NULL;
    }

    sKaleidoAreaPtr = NULL;
}

// NOTE: this function looks messed up and probably doesn't work how it was intended to
void* KaleidoManager_GetRamAddr(void* vram) {
    return vram;

#if 0
    KaleidoMgrOverlay* iter = gKaleidoMgrCurOvl;
    KaleidoMgrOverlay* ovl = iter;
    u32 i;

    if (ovl == NULL) {
        iter = &gKaleidoMgrOverlayTable[0];
        for (i = 0; i < ARRAY_COUNT(gKaleidoMgrOverlayTable); i++) {
            if (((uintptr_t)vram >= (uintptr_t)iter->vramStart) && ((uintptr_t)iter->vramEnd >= (uintptr_t)vram)) {
                KaleidoManager_LoadOvl(iter);
                ovl = iter;
                goto KaleidoManager_GetRamAddr_end;
            }
            //! @bug Probably missing iter++ here
        }

        osSyncPrintf("異常\n"); // "Abnormal"
        return NULL;
    }

KaleidoManager_GetRamAddr_end:
    if ((ovl == NULL) || ((uintptr_t)vram < (uintptr_t)ovl->vramStart) || ((uintptr_t)vram >= (uintptr_t)ovl->vramEnd)) {
        return NULL;
    }

    return (void*)((uintptr_t)vram + ovl->offset);
#endif
}
