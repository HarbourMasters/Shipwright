#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"
#include "2s2h/Rando/Rando.h"

#define CVAR_NAME "gEnhancements.Timesavers.FastChests"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterFastChests() {
    COND_VB_SHOULD(VB_PLAY_SLOW_CHEST_CS, CVAR && !IS_RANDO, { *should = false; });
}

static RegisterShipInitFunc initFunc(RegisterFastChests, { CVAR_NAME, "IS_RANDO" });