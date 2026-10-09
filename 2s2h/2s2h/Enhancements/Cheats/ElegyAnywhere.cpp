#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

#define CVAR_NAME "gCheats.ElegyAnywhere"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterElegyAnywhere() {
    COND_VB_SHOULD(VB_ELEGY_CHECK_SCENE, CVAR, { *should = true; });
}

static RegisterShipInitFunc initFunc(RegisterElegyAnywhere, { CVAR_NAME });
