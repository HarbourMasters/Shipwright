#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"
#include "variables.h"

#define CVAR_NAME "gEnhancements.DifficultyOptions.JinxedTimer"
#define CVAR CVarGetInteger(CVAR_NAME, 60)

void RegisterJinxedTimer() {
    COND_ID_HOOK(ShouldActorUpdate, ACTOR_PLAYER, CVAR < 60, [](Actor* actor, bool* should) {
        if (gSaveContext.jinxTimer == 0) {
            return;
        }

        if (gSaveContext.jinxTimer > CVAR * 20) {
            gSaveContext.jinxTimer = CVAR * 20;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterJinxedTimer, { CVAR_NAME });
