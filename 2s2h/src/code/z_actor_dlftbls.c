#include "prevent_bss_reordering.h"
#include "global.h"
#include "fault.h"

// Segment and Profile declarations (also used in the table below)
// 2S2H Added columns to actor table: _humanName
#define DEFINE_ACTOR(name, _enumValue, _allocType, _debugName, _humanName) extern ActorProfile name##_Profile;
#define DEFINE_ACTOR_INTERNAL(name, _enumValue, _allocType, _debugName, _humanName) extern ActorProfile name##_Profile;
#define DEFINE_ACTOR_UNSET(_enumValue)

#include "tables/actor_table.h"

#undef DEFINE_ACTOR
#undef DEFINE_ACTOR_INTERNAL
#undef DEFINE_ACTOR_UNSET

//// Actor Overlay Table definition
//#define DEFINE_ACTOR(name, _enumValue, allocType, _debugName) \
//    { SEGMENT_ROM_START(ovl_##name),                          \
//      SEGMENT_ROM_END(ovl_##name),                            \
//      SEGMENT_START(ovl_##name),                              \
//      SEGMENT_END(ovl_##name),                                \
//      NULL,                                                   \
//      &name##_Profile,                                       \
//      NULL,                                                   \
//      allocType,                                              \
//      0 },

// 2S2H Added columns to actor table: _humanName
#define DEFINE_ACTOR_INTERNAL(name, _enumValue, allocType, _debugName, _humanName) \
    { 0, 0, NULL, NULL, NULL, &name##_Profile, NULL, allocType, 0 },

#define DEFINE_ACTOR(name, _enumValue, allocType, _debugName, _humanName) \
    DEFINE_ACTOR_INTERNAL(name, _enumValue, allocType, _debugName, _humanName)

#define DEFINE_ACTOR_UNSET(_enumValue) { 0 },

ActorOverlay gActorOverlayTable[] = {
#include "tables/actor_table.h"
};

#undef DEFINE_ACTOR
#undef DEFINE_ACTOR_INTERNAL
#undef DEFINE_ACTOR_UNSET

ActorId gMaxActorId = 0;

FaultClient sActorOverlayTableFaultClient;
FaultAddrConvClient sActorOverlayTableFaultAddrConvClient;

void ActorOverlayTable_FaultClient(void* arg0, void* arg1) {
    ActorOverlay* overlayEntry;
    u32 overlaySize;
    ActorId actorId;

    FaultDrawer_SetCharPad(-2, 0);

    FaultDrawer_Printf("actor_dlftbls %u\n", gMaxActorId);
    FaultDrawer_Printf("No. RamStart- RamEnd cn  Name\n");

    for (actorId = 0, overlayEntry = &gActorOverlayTable[0]; actorId < gMaxActorId; actorId++, overlayEntry++) {
        overlaySize = (uintptr_t)overlayEntry->vramEnd - (uintptr_t)overlayEntry->vramStart;
        if (overlayEntry->loadedRamAddr != NULL) {
            FaultDrawer_Printf("%3d %08x-%08x %3d %s\n", actorId, overlayEntry->loadedRamAddr,
                               (uintptr_t)overlayEntry->loadedRamAddr + overlaySize, overlayEntry->numLoaded, "");
        }
    }
}

uintptr_t ActorOverlayTable_FaultAddrConv(uintptr_t address, void* param) {
    uintptr_t addr = address;
    ActorOverlay* actorOvl = &gActorOverlayTable[0];
    uintptr_t ramConv;
    void* ramStart;
    size_t diff;
    ActorId actorId;

    for (actorId = 0; actorId < gMaxActorId; actorId++, actorOvl++) {
        diff = (uintptr_t)actorOvl->vramEnd - (uintptr_t)actorOvl->vramStart;
        ramStart = actorOvl->loadedRamAddr;
        ramConv = (uintptr_t)actorOvl->vramStart - (uintptr_t)ramStart;

        if (ramStart != NULL) {
            if ((addr >= (uintptr_t)ramStart) && (addr < (uintptr_t)ramStart + diff)) {
                return addr + ramConv;
            }
        }
    }

    return 0;
}

void ActorOverlayTable_Init(void) {
    gMaxActorId = ACTOR_ID_MAX;
    Fault_AddClient(&sActorOverlayTableFaultClient, ActorOverlayTable_FaultClient, NULL, NULL);
    Fault_AddAddrConvClient(&sActorOverlayTableFaultAddrConvClient, ActorOverlayTable_FaultAddrConv, NULL);
}

void ActorOverlayTable_Cleanup(void) {
    Fault_RemoveClient(&sActorOverlayTableFaultClient);
    Fault_RemoveAddrConvClient(&sActorOverlayTableFaultAddrConvClient);
    gMaxActorId = 0;
}
