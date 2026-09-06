#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"
#include "soh/cvar_prefixes.h"

static void RegisterFireproofDekuShield() {
    COND_VB_SHOULD(VB_BURN_SHIELD, CVarGetInteger(CVAR_CHEAT("FireproofDekuShield"), 0), { *should = false; });
}

static RegisterShipInitFunc initFunc(RegisterFireproofDekuShield, { CVAR_CHEAT("FireproofDekuShield") });
