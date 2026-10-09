#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

#define CVAR_NAME "gEnhancements.Timesavers.FasterBottles"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterFasterBottles() {
    COND_VB_SHOULD(VB_USE_BOTTLE_ITEM, CVAR, {
        Player* player = va_arg(args, Player*);
        player->skelAnime.playSpeed = player->skelAnime.curFrame <= 60.0f ? 3.0f : 1.0f;
    });
}

static RegisterShipInitFunc initFunc(RegisterFasterBottles, { CVAR_NAME });