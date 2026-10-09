#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "variables.h"
}

#define CVAR_NAME "gEnhancements.Restorations.ConstantFlipsHops"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterVariableFlipHop() {
    COND_VB_SHOULD(VB_APPLY_AIR_CONTROL, CVAR, {
        Player* player = GET_PLAYER(gPlayState);

        // actionVar1 == 5 is frontflips when running off a ledge
        if (player->stateFlags2 & PLAYER_STATE2_80000 && player->av1.actionVar1 != 5 &&
            // Deku Hopping last hop is considered a backflip/sidehop so we make sure they aren't deku hopping
            !(player->transformation == PLAYER_FORM_DEKU && player->stateFlags3 & PLAYER_STATE3_200000)) {
            *should = false;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterVariableFlipHop, { CVAR_NAME });
