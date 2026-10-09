#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

#define CVAR_NAME "gCheats.InfiniteEponaCarrots"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterInfiniteEponaCarrots() {
    COND_VB_SHOULD(VB_CONSUME_EPONA_CARROT, CVAR, { *should = false; });
}

static RegisterShipInitFunc initFunc(RegisterInfiniteEponaCarrots, { CVAR_NAME });