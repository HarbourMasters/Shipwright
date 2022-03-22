#include "global.h"

// Linker symbol declarations (used in the table below)
#define DEFINE_ACTOR(name, _1, _2) DECLARE_OVERLAY_SEGMENT(name)
#define DEFINE_ACTOR_INTERNAL(_0, _1, _2)
#define DEFINE_ACTOR_UNSET(_0)

#include "tables/actor_table.h"

#undef DEFINE_ACTOR
#undef DEFINE_ACTOR_INTERNAL
#undef DEFINE_ACTOR_UNSET

// Init Vars declarations (also used in the table below)
#define DEFINE_ACTOR(name, _1, _2) extern ActorInit name##_InitVars;
#define DEFINE_ACTOR_INTERNAL(name, _1, _2) extern ActorInit name##_InitVars;
#define DEFINE_ACTOR_UNSET(_0)

#include "tables/actor_table.h"

#undef DEFINE_ACTOR
#undef DEFINE_ACTOR_INTERNAL
#undef DEFINE_ACTOR_UNSET

// Actor Overlay Table definition
//#define DEFINE_ACTOR(name, _1, allocType) \
//    { (uintptr_t)_ovl_##name##SegmentRomStart,  \
//      (uintptr_t)_ovl_##name##SegmentRomEnd,    \
//      _ovl_##name##SegmentStart,          \
//      _ovl_##name##SegmentEnd,            \
//      NULL,                               \
//      &name##_InitVars,                   \
//      #name,                              \
//      allocType,                          \
//      0 },

#define DEFINE_ACTOR_INTERNAL(name, _1, allocType) { 0, 0, NULL, NULL, NULL, &name##_InitVars, #name, allocType, 0 },

#define DEFINE_ACTOR_UNSET(_0) { 0 },

#define DEFINE_ACTOR(name, _1, allocType) { 0, 0, NULL, NULL, NULL, &name##_InitVars, #name, allocType, 0 },

ActorOverlay gActorOverlayTable[] = {
#include "tables/actor_table.h"
};

#undef DEFINE_ACTOR
#undef DEFINE_ACTOR_INTERNAL
#undef DEFINE_ACTOR_UNSET

s32 gMaxActorId = 0;

static FaultClient sFaultClient;

void ActorOverlayTable_LogPrint(void) {
    ActorOverlay* overlayEntry;
    u32 i;

    osSyncPrintf("actor_dlftbls %u\n", gMaxActorId);
    osSyncPrintf("RomStart RomEnd   SegStart SegEnd   allocp   profile  segname\n");

    for (i = 0, overlayEntry = &gActorOverlayTable[0]; i < (u32)gMaxActorId; i++, overlayEntry++) {
        osSyncPrintf("%08x %08x %08x %08x %08x %08x %s\n", overlayEntry->vromStart, overlayEntry->vromEnd,
                     overlayEntry->vramStart, overlayEntry->vramEnd, overlayEntry->loadedRamAddr,
                     &overlayEntry->initInfo->id, overlayEntry->name != NULL ? overlayEntry->name : "?");
    }
}

void ActorOverlayTable_FaultPrint(void* arg0, void* arg1) {
    ActorOverlay* overlayEntry;
    u32 overlaySize;
    s32 i;

    FaultDrawer_SetCharPad(-2, 0);

    FaultDrawer_Printf("actor_dlftbls %u\n", gMaxActorId);
    FaultDrawer_Printf("No. RamStart- RamEnd cn  Name\n");

    for (i = 0, overlayEntry = &gActorOverlayTable[0]; i < gMaxActorId; i++, overlayEntry++) {
        overlaySize = (uintptr_t)overlayEntry->vramEnd - (uintptr_t)overlayEntry->vramStart;
        if (overlayEntry->loadedRamAddr != NULL) {
            FaultDrawer_Printf("%3d %08x-%08x %3d %s\n", i, overlayEntry->loadedRamAddr,
                               (uintptr_t)overlayEntry->loadedRamAddr + overlaySize, overlayEntry->numLoaded,
                               overlayEntry->name != NULL ? overlayEntry->name : "");
        }
    }
}

void ActorOverlayTable_Init(void) {
    gMaxActorId = ACTOR_ID_MAX;
    Fault_AddClient(&sFaultClient, ActorOverlayTable_FaultPrint, NULL, NULL);
}

void ActorOverlayTable_Cleanup(void) {
    Fault_RemoveClient(&sFaultClient);
    gMaxActorId = 0;
}
