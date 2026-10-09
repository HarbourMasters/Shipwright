#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

#define CVAR_NAME "gEnhancements.DifficultyOptions.DisableTakkuriSteal"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterDisableTakkuriSteal() {
    COND_VB_SHOULD(VB_THIEF_BIRD_STEAL, CVAR, { *should = false; });
}

static RegisterShipInitFunc initFunc(RegisterDisableTakkuriSteal, { CVAR_NAME });
