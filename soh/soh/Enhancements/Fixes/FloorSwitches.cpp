#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "macros.h"
#include "src/overlays/actors/ovl_Obj_Switch/z_obj_switch.h"
}

static constexpr int32_t CVAR_FLOOR_SWITCHES_DEFAULT = 0;
#define CVAR_FLOOR_SWITCHES_NAME CVAR_ENHANCEMENT("FixFloorSwitches")
#define CVAR_FLOOR_SWITCHES_VALUE CVarGetInteger(CVAR_FLOOR_SWITCHES_NAME, CVAR_FLOOR_SWITCHES_DEFAULT)

static void OnInitFloorSwitches(void* refActor) {
    ObjSwitch* switchActor = reinterpret_cast<ObjSwitch*>(refActor);
    s32 type = (switchActor->dyna.actor.params & 7);

    if (switchActor->dyna.actor.params == 0x1200 || switchActor->dyna.actor.params == 0x3A00) {
        switchActor->dyna.actor.world.pos.y -= 1;
    }
}

static void RegisterFloorSwitchesFix() {
    COND_ID_HOOK(OnActorInit, ACTOR_OBJ_SWITCH, CVAR_FLOOR_SWITCHES_VALUE, OnInitFloorSwitches);
}

static RegisterShipInitFunc initFunc(RegisterFloorSwitchesFix, { CVAR_FLOOR_SWITCHES_NAME });
