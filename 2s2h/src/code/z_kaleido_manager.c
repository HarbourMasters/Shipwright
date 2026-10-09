#include "z64pause_menu.h"

#include "global.h"
#include "fault.h"
#include "loadfragment.h"
#include <string.h>

//#define KALEIDO_OVERLAY(name)                                                                        \
//    {                                                                                                \
//        NULL, SEGMENT_ROM_START(ovl_##name), SEGMENT_ROM_END(ovl_##name), SEGMENT_START(ovl_##name), \
//            SEGMENT_END(ovl_##name), 0, #name,                                                       \
//    }

#define KALEIDO_OVERLAY(name) \
    { NULL, 0, 0, 0, 0, 0, #name, }

KaleidoMgrOverlay gKaleidoMgrOverlayTable[KALEIDO_OVL_MAX] = {
    KALEIDO_OVERLAY(kaleido_scope),
    KALEIDO_OVERLAY(player_actor),
};

void* sKaleidoAreaPtr = NULL;
KaleidoMgrOverlay* gKaleidoMgrCurOvl = NULL;

FaultAddrConvClient sKaleidoMgrFaultAddrConvClient;

uintptr_t KaleidoManager_FaultAddrConv(uintptr_t address, void* param) {
    uintptr_t addr = address;
    KaleidoMgrOverlay* kaleidoMgrOvl = gKaleidoMgrCurOvl;
    uintptr_t ramConv;
    void* ramStart;
    size_t diff;

    if (kaleidoMgrOvl != NULL) {
        diff = (uintptr_t)kaleidoMgrOvl->vramEnd - (uintptr_t)kaleidoMgrOvl->vramStart;
        ramStart = kaleidoMgrOvl->loadedRamAddr;
        ramConv = (uintptr_t)kaleidoMgrOvl->vramStart - (uintptr_t)ramStart;

        if (ramStart != NULL) {
            if ((addr >= (uintptr_t)ramStart) && (addr < (uintptr_t)ramStart + diff)) {
                return addr + ramConv;
            }
        }
    }
    return 0;
}

void KaleidoManager_LoadOvl(KaleidoMgrOverlay* ovl) {
    ovl->loadedRamAddr = sKaleidoAreaPtr;
    Overlay_Load(ovl->vromStart, ovl->vromEnd, ovl->vramStart, ovl->vramEnd, ovl->loadedRamAddr);
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
    size_t largestSize = 0;
    size_t size;
    u32 i;

    for (i = 0; i < ARRAY_COUNT(gKaleidoMgrOverlayTable); i++) {
        size = (uintptr_t)gKaleidoMgrOverlayTable[i].vramEnd - (uintptr_t)gKaleidoMgrOverlayTable[i].vramStart;
        if (size > largestSize) {
            largestSize = size;
        }
    }

    sKaleidoAreaPtr = THA_AllocTailAlign16(&play->state.tha, largestSize);
    gKaleidoMgrCurOvl = NULL;
    Fault_AddAddrConvClient(&sKaleidoMgrFaultAddrConvClient, KaleidoManager_FaultAddrConv, NULL);
}

void KaleidoManager_Destroy(void) {
    Fault_RemoveAddrConvClient(&sKaleidoMgrFaultAddrConvClient);

    if (gKaleidoMgrCurOvl != NULL) {
        KaleidoManager_ClearOvl(gKaleidoMgrCurOvl);
        gKaleidoMgrCurOvl = NULL;
    }

    sKaleidoAreaPtr = NULL;
}

void* KaleidoManager_GetRamAddr(void* vram) {
    return vram;
#if 0
    if (gKaleidoMgrCurOvl == NULL) {
        s32 pad[2];
        KaleidoMgrOverlay* ovl = &gKaleidoMgrOverlayTable[0];

        do {
            if (((uintptr_t)vram >= (uintptr_t)ovl->vramStart) && ((uintptr_t)ovl->vramEnd >= (uintptr_t)vram)) {
                KaleidoManager_LoadOvl(ovl);
                return (void*)((uintptr_t)vram + ovl->offset);
            }
            ovl++;
        } while (ovl != (KaleidoMgrOverlay*)&sKaleidoAreaPtr);

        return NULL;
    } else if (((uintptr_t)vram < (uintptr_t)gKaleidoMgrCurOvl->vramStart) ||
               ((uintptr_t)vram >= (uintptr_t)gKaleidoMgrCurOvl->vramEnd)) {
        return NULL;
    }

    return (void*)((uintptr_t)vram + gKaleidoMgrCurOvl->offset);
#endif
}
