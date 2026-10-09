#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"
#include "2s2h/Rando/Rando.h"
#include "2s2h/Rando/Logic/Logic.h"

extern "C" {
#include "functions.h"
}

#define CVAR_NAME "gEnhancements.Cutscenes.SkipMiscInteractions"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterSkipPushingMikau() {
    COND_ID_HOOK(ShouldActorInit, ACTOR_EN_ZOG, CVAR, [](Actor* actor, bool* should) {
        if (IS_RANDO && !CAN_USE_ABILITY(SWIM)) {
            // This skip would circumvent logical requirements if the player cannot swim yet
            return;
        }
        Flags_SetWeekEventReg(WEEKEVENTREG_88_10);
        Flags_SetWeekEventReg(WEEKEVENTREG_29_20);
        Flags_SetWeekEventReg(WEEKEVENTREG_91_01);
    });
}

static RegisterShipInitFunc initFunc(RegisterSkipPushingMikau, { CVAR_NAME });
