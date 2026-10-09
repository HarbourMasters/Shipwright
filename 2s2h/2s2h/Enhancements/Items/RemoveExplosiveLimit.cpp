#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

#define CVAR_NAME "gEnhancements.Items.RemoveExplosiveLimit"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterRemoveExplosiveLimit() {
    // Ignore vanilla limit of 3 (or 5 for Honey & Darling)
    COND_VB_SHOULD(VB_LIMIT_EXPLOSIVES, CVAR, { *should = false; });
}

static RegisterShipInitFunc initFunc(RegisterRemoveExplosiveLimit, { CVAR_NAME });
