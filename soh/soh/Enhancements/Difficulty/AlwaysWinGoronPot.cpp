#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"

#define CVAR_WIN_GORON_POT_NAME CVAR_ENHANCEMENT("GoronPot")
#define CVAR_WIN_GORON_POT_VALUE CVarGetInteger(CVAR_WIN_GORON_POT_NAME, 0)

static void RegisterWinGoronPot() {
    COND_VB_SHOULD(VB_WIN_GORON_POT, CVAR_WIN_GORON_POT_VALUE, { *should = true; });
}

static RegisterShipInitFunc initFunc(RegisterWinGoronPot, { CVAR_WIN_GORON_POT_NAME });
