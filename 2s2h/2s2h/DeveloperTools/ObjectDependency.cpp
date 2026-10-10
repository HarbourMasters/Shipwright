#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "variables.h"
}

#define CVAR_NAME "gDeveloperTools.DisableObjectDependency"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterObjectDependency() {
    COND_VB_SHOULD(VB_ENABLE_OBJECT_DEPENDENCY, CVAR, { *should = false; });
}

static RegisterShipInitFunc initFunc(RegisterObjectDependency, { CVAR_NAME });
