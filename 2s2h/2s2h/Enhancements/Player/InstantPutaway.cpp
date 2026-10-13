#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

#define CVAR_NAME "gEnhancements.Player.InstantPutaway"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterInstantPutaway() {
    COND_VB_SHOULD(VB_RESET_PUTAWAY_TIMER, CVAR, { *should = false; });
}

static RegisterShipInitFunc initFunc(RegisterInstantPutaway, { CVAR_NAME });
