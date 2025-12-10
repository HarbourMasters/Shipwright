#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/OTRGlobals.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "src/overlays/actors/ovl_En_Door/z_en_door.h"
extern SaveContext gSaveContext;
extern PlayState* gPlayState;
}

static constexpr int32_t CVAR_DAMPE_ALL_NIGHT_DEFAULT = 0;
#define CVAR_DAMPE_ALL_NIGHT_NAME CVAR_ENHANCEMENT("DampeAllNight")
#define CVAR_DAMPE_ALL_NIGHT_VALUE CVarGetInteger(CVAR_DAMPE_ALL_NIGHT_NAME, CVAR_DAMPE_ALL_NIGHT_DEFAULT)

static constexpr s16 DAMPE_HUT_DOOR_OPEN = 447;
static constexpr s16 DAMPE_HUT_DOOR_CLOSED = 774;

static bool DampeIsResting() {
    return LINK_IS_ADULT || gPlayState->sceneNum != SCENE_GRAVEYARD;
}

static void OpenDampeHutDoor(void* refActor) {
    EnDoor* enDoor = static_cast<EnDoor*>(refActor);
    s16* params = &enDoor->actor.params;

    if (*params == DAMPE_HUT_DOOR_CLOSED && !DampeIsResting()) {
        *params = DAMPE_HUT_DOOR_OPEN;
        EnDoor_SetupType(enDoor, gPlayState);
    }
}

static void RegisterDampeAllNight() {
    COND_VB_SHOULD(VB_DAMPE_IN_GRAVEYARD_DESPAWN, CVAR_DAMPE_ALL_NIGHT_VALUE, { *should = DampeIsResting(); });
    COND_ID_HOOK(OnActorInit, ACTOR_EN_DOOR, CVAR_DAMPE_ALL_NIGHT_VALUE, OpenDampeHutDoor);
}

static RegisterShipInitFunc initFunc(RegisterDampeAllNight, { CVAR_DAMPE_ALL_NIGHT_NAME });
