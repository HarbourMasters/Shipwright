#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "functions.h"
#include "variables.h"
}

#define CVAR_NAME "gEnhancements.Cutscenes.SkipMiscInteractions"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterSkipAlienWarning() {
    COND_ID_HOOK(ShouldActorInit, ACTOR_EN_MA4, CVAR, [](Actor* actor, bool* should) {
        if (gPlayState->sceneId != SCENE_F01) { // Romani Ranch
            return;
        }

        SET_WEEKEVENTREG(WEEKEVENTREG_21_40);
    });
}

static RegisterShipInitFunc initFunc(RegisterSkipAlienWarning, { CVAR_NAME });
