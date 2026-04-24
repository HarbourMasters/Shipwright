#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"

static void RegisterNoFishDespawn() {
    COND_VB_SHOULD(VB_FISH_TIMER_TICK, CVarGetInteger(CVAR_CHEAT("NoFishDespawn"), 0), { *should = false; });
}

static RegisterShipInitFunc initFunc(RegisterNoFishDespawn, { CVAR_CHEAT("NoFishDespawn") });
