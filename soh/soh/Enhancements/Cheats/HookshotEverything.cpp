#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"
#include "soh/cvar_prefixes.h"

static void RegisterHookshotEverything() {
    COND_VB_SHOULD(VB_SURFACE_IS_HOOKSHOT, CVarGetInteger(CVAR_CHEAT("HookshotEverything"), 0), { *should = true; });
}

static RegisterShipInitFunc initFunc(RegisterHookshotEverything, { CVAR_CHEAT("HookshotEverything") });
