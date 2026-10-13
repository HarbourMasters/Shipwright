#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "functions.h"
}

#define CVAR_NAME "gEnhancements.Cutscenes.SkipMiscInteractions"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterSkipTingleTime() {
    COND_ID_HOOK(ShouldActorInit, ACTOR_EN_BAL, CVAR,
                 [](Actor* actor, bool* should) { SET_WEEKEVENTREG(WEEKEVENTREG_TALKED_TINGLE); });
}

static RegisterShipInitFunc initFunc(RegisterSkipTingleTime, { CVAR_NAME });
