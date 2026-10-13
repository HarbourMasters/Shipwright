#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

#define CVAR_NAME "gEnhancements.A11y.NoFinalDayQuakes"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

static void RegisterDisableFinalDayQuakes() {
    COND_VB_SHOULD(VB_EARTHQUAKE_ON_DAY_3, CVAR, { *should = false; });
}

static RegisterShipInitFunc initFunc(RegisterDisableFinalDayQuakes, { CVAR_NAME });
