#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

#define CVAR_NAME "gEnhancements.Restorations.TatlISG"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterTatlISG() {
    COND_VB_SHOULD(VB_TATL_CONVERSATION_AVAILABLE, CVAR, { *should = false; });
}

static RegisterShipInitFunc initFunc(RegisterTatlISG, { CVAR_NAME });
