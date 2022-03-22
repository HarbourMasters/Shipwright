#include "global.h"

void* Overlay_AllocateAndLoad(uintptr_t vRomStart, uintptr_t vRomEnd, void* vRamStart, void* vRamEnd) {
    void* allocatedVRamAddr = SystemArena_MallocRDebug((intptr_t)vRamEnd - (intptr_t)vRamStart, "../loadfragment2.c", 31);

    if (gOverlayLogSeverity >= 3) {
        osSyncPrintf("OVL:SPEC(%08x-%08x) REAL(%08x-%08x) OFFSET(%08x)\n", vRamStart, vRamEnd, allocatedVRamAddr,
                     ((uintptr_t)vRamEnd - (uintptr_t)vRamStart) + (uintptr_t)allocatedVRamAddr, (uintptr_t)vRamStart - (uintptr_t)allocatedVRamAddr);
    }

    if (allocatedVRamAddr != NULL) {
        Overlay_Load(vRomStart, vRomEnd, vRamStart, vRamEnd, allocatedVRamAddr);
    }

    return allocatedVRamAddr;
}
