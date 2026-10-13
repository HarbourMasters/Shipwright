#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

#define CVAR_NAME "gEnhancements.Cutscenes.HideTitleCards"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterHideTitleCards() {
    COND_VB_SHOULD(VB_SHOW_TITLE_CARD, CVAR, { *should = false; });
}

static RegisterShipInitFunc initFunc(RegisterHideTitleCards, { CVAR_NAME });
