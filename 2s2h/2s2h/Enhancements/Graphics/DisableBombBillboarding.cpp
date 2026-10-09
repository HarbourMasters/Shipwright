#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

#define CVAR_NAME "gEnhancements.Mods.DisableBombBillboarding"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterDisableBombBillboarding() {
    COND_VB_SHOULD(VB_APPLY_BOMB_BILLBOARDING, CVAR, { *should = false; });
}

static RegisterShipInitFunc initFunc(RegisterDisableBombBillboarding, { CVAR_NAME });
