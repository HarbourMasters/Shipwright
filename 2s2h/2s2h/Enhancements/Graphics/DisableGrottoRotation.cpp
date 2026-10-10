#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

#define CVAR_NAME "gEnhancements.Mods.DisableGrottoRotation"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterDisableGrottoRotation() {
    COND_VB_SHOULD(VB_ROTATE_GROTTO_ENTRANCE, CVAR, { *should = false; });
}

static RegisterShipInitFunc initFunc(RegisterDisableGrottoRotation, { CVAR_NAME });
