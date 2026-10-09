#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "functions.h"
}

#define CVAR_NAME "gEnhancements.Cutscenes.SkipMiscInteractions"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterSkipGateGuards() {
    COND_ID_HOOK(OnActorInit, ACTOR_EN_STOP_HEISHI, CVAR, [](Actor* actor) { SET_WEEKEVENTREG(WEEKEVENTREG_12_20); });
}

static RegisterShipInitFunc initFunc(RegisterSkipGateGuards, { CVAR_NAME });
