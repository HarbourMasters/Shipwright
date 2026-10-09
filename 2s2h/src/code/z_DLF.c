#include "global.h"
#include "system_malloc.h"
#include "loadfragment.h"

void Overlay_LoadGameState(GameStateOverlay* overlayEntry) {
    void* vramStart;

    if (overlayEntry->loadedRamAddr != NULL) {
        return;
    }

    vramStart = overlayEntry->vramStart;
    if (vramStart == NULL) {
        overlayEntry->unk_28 = 0;
        return;
    }

    overlayEntry->loadedRamAddr =
        Overlay_AllocateAndLoad(overlayEntry->vromStart, overlayEntry->vromEnd, vramStart, overlayEntry->vramEnd);

    if (overlayEntry->loadedRamAddr != NULL) {
        overlayEntry->unk_14 = (void*)(uintptr_t)((overlayEntry->unk_14 != NULL)
                                                      ? (void*)((uintptr_t)overlayEntry->unk_14 -
                                                                (intptr_t)((uintptr_t)overlayEntry->vramStart -
                                                                           (uintptr_t)overlayEntry->loadedRamAddr))
                                                      : NULL);

        overlayEntry->init = (void*)(uintptr_t)((overlayEntry->init != NULL)
                                                    ? (void*)((uintptr_t)overlayEntry->init -
                                                              (intptr_t)((uintptr_t)overlayEntry->vramStart -
                                                                         (uintptr_t)overlayEntry->loadedRamAddr))
                                                    : NULL);

        overlayEntry->destroy = (void*)(uintptr_t)((overlayEntry->destroy != NULL)
                                                       ? (void*)((uintptr_t)overlayEntry->destroy -
                                                                 (intptr_t)((uintptr_t)overlayEntry->vramStart -
                                                                            (uintptr_t)overlayEntry->loadedRamAddr))
                                                       : NULL);

        overlayEntry->unk_20 = (void*)(uintptr_t)((overlayEntry->unk_20 != NULL)
                                                      ? (void*)((uintptr_t)overlayEntry->unk_20 -
                                                                (intptr_t)((uintptr_t)overlayEntry->vramStart -
                                                                           (uintptr_t)overlayEntry->loadedRamAddr))
                                                      : NULL);

        overlayEntry->unk_24 = (void*)(uintptr_t)((overlayEntry->unk_24 != NULL)
                                                      ? (void*)((uintptr_t)overlayEntry->unk_24 -
                                                                (intptr_t)((uintptr_t)overlayEntry->vramStart -
                                                                           (uintptr_t)overlayEntry->loadedRamAddr))
                                                      : NULL);

        overlayEntry->unk_28 = 0;
    }
}

void Overlay_FreeGameState(GameStateOverlay* overlayEntry) {
    s32 var_v0;

    if (overlayEntry->loadedRamAddr != NULL) {
        var_v0 = (overlayEntry->unk_28 != 0) ? -1 : 0;
        if (var_v0 == 0) {
            overlayEntry->unk_14 = (void*)(uintptr_t)((overlayEntry->unk_14 != NULL)
                                                          ? (void*)((uintptr_t)overlayEntry->unk_14 +
                                                                    (intptr_t)((uintptr_t)overlayEntry->vramStart -
                                                                               (uintptr_t)overlayEntry->loadedRamAddr))
                                                          : NULL);

            overlayEntry->init = (void*)(uintptr_t)((overlayEntry->init != NULL)
                                                        ? (void*)((uintptr_t)overlayEntry->init +
                                                                  (intptr_t)((uintptr_t)overlayEntry->vramStart -
                                                                             (uintptr_t)overlayEntry->loadedRamAddr))
                                                        : NULL);

            overlayEntry->destroy = (void*)(uintptr_t)((overlayEntry->destroy != NULL)
                                                           ? (void*)((uintptr_t)overlayEntry->destroy +
                                                                     (intptr_t)((uintptr_t)overlayEntry->vramStart -
                                                                                (uintptr_t)overlayEntry->loadedRamAddr))
                                                           : NULL);

            overlayEntry->unk_20 = (void*)(uintptr_t)((overlayEntry->unk_20 != NULL)
                                                          ? (void*)((uintptr_t)overlayEntry->unk_20 +
                                                                    (intptr_t)((uintptr_t)overlayEntry->vramStart -
                                                                               (uintptr_t)overlayEntry->loadedRamAddr))
                                                          : NULL);

            overlayEntry->unk_24 = (void*)(uintptr_t)((overlayEntry->unk_24 != NULL)
                                                          ? (void*)((uintptr_t)overlayEntry->unk_24 +
                                                                    (intptr_t)((uintptr_t)overlayEntry->vramStart -
                                                                               (uintptr_t)overlayEntry->loadedRamAddr))
                                                          : NULL);

            SystemArena_Free(overlayEntry->loadedRamAddr);
            overlayEntry->loadedRamAddr = NULL;
        }
    }
}
