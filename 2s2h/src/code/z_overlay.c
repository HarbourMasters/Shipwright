/**
 * @file z_overlay.c
 *
 * Functions for handling transition overlays in memory
 */

#include "z64transition.h"

#include "loadfragment.h"
#include "z64lib.h"
#include "z64malloc.h"

void* TransitionOverlay_VramToRam(TransitionOverlay* overlayEntry, void* vramAddr) {
    void* loadedRamAddr = Lib_PhysicalToVirtual(overlayEntry->loadInfo.addr);

    if ((loadedRamAddr != NULL) && (vramAddr >= overlayEntry->vramStart) && (vramAddr < overlayEntry->vramEnd)) {
        return (void*)(((uintptr_t)loadedRamAddr - (uintptr_t)overlayEntry->vramStart) + (uintptr_t)vramAddr);
    }
    return vramAddr;
}

void TransitionOverlay_VramToRamArray(TransitionOverlay* overlayEntry, void** vramAddrs, s32 count) {
    s32 i;

    for (i = 0; i < count; i++) {
        vramAddrs[i] = TransitionOverlay_VramToRam(overlayEntry, vramAddrs[i]);
    }
}

TransitionOverlayStatus TransitionOverlay_Load(TransitionOverlay* overlayEntry) {
    s32 count;
    void* loadedRamAddr;
    return 3;
#if 0
    if (overlayEntry->vromStart == 0) {
        return TRANSITION_OVERLAY_STATUS_INTERNAL;
    }

    if (Lib_PhysicalToVirtual(overlayEntry->loadInfo.addr) == NULL) {
        loadedRamAddr = ZeldaArena_Malloc((uintptr_t)overlayEntry->vramEnd - (uintptr_t)overlayEntry->vramStart);

        if (loadedRamAddr == NULL) {
            return TRANSITION_OVERLAY_STATUS_FAILED;
        }

        Overlay_Load(overlayEntry->vromStart, overlayEntry->vromEnd, overlayEntry->vramStart, overlayEntry->vramEnd,
                     loadedRamAddr);
        overlayEntry->loadInfo.addr = Lib_VirtualToPhysical(loadedRamAddr);
        overlayEntry->loadInfo.count = 1;
        return TRANSITION_OVERLAY_STATUS_LOAD_FREE;
    }

    count = overlayEntry->loadInfo.count;
    if (count != 0) {
        count++;
        overlayEntry->loadInfo.count = count;

        if (count == 0) {
            return TRANSITION_OVERLAY_STATUS_LOADED_NO_INSTANCES;
        } else {
            return TRANSITION_OVERLAY_STATUS_ADD_REMOVAL;
        }
    }

    return TRANSITION_OVERLAY_STATUS_LOADED_NO_INSTANCES;
#endif
}

TransitionOverlayStatus TransitionOverlay_Free(TransitionOverlay* overlayEntry) {
    s32 count;
    void* loadedRamAddr;

    if (overlayEntry->vromStart == 0) {
        return TRANSITION_OVERLAY_STATUS_INTERNAL;
    }

    loadedRamAddr = Lib_PhysicalToVirtual(overlayEntry->loadInfo.addr);
    if (loadedRamAddr != NULL) {
        count = overlayEntry->loadInfo.count;
        if (count != 0) {
            count--;
            overlayEntry->loadInfo.count = count;
            if (count == 0) {
                ZeldaArena_Free(loadedRamAddr);
                overlayEntry->loadInfo.addr = Lib_VirtualToPhysical(NULL);
                return TRANSITION_OVERLAY_STATUS_LOAD_FREE;
            }
            return TRANSITION_OVERLAY_STATUS_ADD_REMOVAL;
        }
        return TRANSITION_OVERLAY_STATUS_LOADED_NO_INSTANCES;
    }
    return TRANSITION_OVERLAY_STATUS_FAILED;
}

void TransitionOverlay_ClearLoadInfo(TransitionOverlay* overlayEntry) {
    overlayEntry->loadInfo.word = 0; // Equivalent to overlayEntry->loadInfo.count = 0, overlayEntry->loadInfo.addr = 0;
}

void TransitionOverlay_SetSegment(TransitionOverlay* overlayEntry, void* vramStart, void* vramEnd, uintptr_t vromStart,
                                  uintptr_t vromEnd) {
    overlayEntry->vramStart = vramStart;
    overlayEntry->vramEnd = vramEnd;
    overlayEntry->vromStart = vromStart;
    overlayEntry->vromEnd = vromEnd;
}
