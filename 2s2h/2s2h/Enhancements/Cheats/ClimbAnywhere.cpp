#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

#define CVAR_NAME "gCheats.ClimbAnywhere"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterClimbAnywhere() {
    COND_VB_SHOULD(VB_BE_CLIMBABLE_SURFACE, CVAR, { *should = true; });
}

static RegisterShipInitFunc initFunc(RegisterClimbAnywhere, { CVAR_NAME });
