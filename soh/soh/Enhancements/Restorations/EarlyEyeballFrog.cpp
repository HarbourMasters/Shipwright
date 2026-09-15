#include <libultraship/bridge/consolevariablebridge.h>

#include "soh/cvar_prefixes.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"

static constexpr const char* CVAR_EARLY_EYEBALL_FROG = CVAR_ENHANCEMENT("EarlyEyeballFrog");

static void RegisterEarlyEyeballFrog() {
    COND_VB_SHOULD(VB_GIVE_EYEBALL_FROG_EARLY, CVarGetInteger(CVAR_EARLY_EYEBALL_FROG, 0), { *should = true; });
}

static RegisterShipInitFunc initFunc(RegisterEarlyEyeballFrog, { CVAR_EARLY_EYEBALL_FROG });
