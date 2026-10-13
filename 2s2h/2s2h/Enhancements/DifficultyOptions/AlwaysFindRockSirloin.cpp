#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

#define CVAR_NAME "gEnhancements.DifficultyOptions.AlwaysFindRockSirloin"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterAlwaysFindRockSirloin() {
    COND_VB_SHOULD(VB_FIND_ROCK_SIRLOIN, CVAR, { *should = true; });
}

static RegisterShipInitFunc initFunc(RegisterAlwaysFindRockSirloin, { CVAR_NAME });
