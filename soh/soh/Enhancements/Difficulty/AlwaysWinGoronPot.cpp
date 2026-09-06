#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"
#include "soh/cvar_prefixes.h"

#define CVAR_WIN_GORON_POT_NAME CVAR_ENHANCEMENT("GoronPot")
#define CVAR_WIN_GORON_POT_VALUE CVarGetInteger(CVAR_WIN_GORON_POT_NAME, 0)

static void RegisterWinGoronPot() {
    COND_VB_SHOULD(VB_WIN_GORON_POT, CVAR_WIN_GORON_POT_VALUE, { *should = true; });
}

static RegisterShipInitFunc initFunc(RegisterWinGoronPot, { CVAR_WIN_GORON_POT_NAME });
