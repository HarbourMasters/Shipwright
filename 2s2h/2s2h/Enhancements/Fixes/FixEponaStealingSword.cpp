#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"
#include "variables.h"

#define CVAR_NAME "gFixes.FixEponaStealingSword"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterFixEponaStealingSword() {
    COND_VB_SHOULD(VB_CLEAR_B_BUTTON_FOR_NO_BOW, CVAR, {
        Player* player = GET_PLAYER(gPlayState);
        // Player riding epona
        if (player->stateFlags1 & PLAYER_STATE1_800000) {
            *should = false;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterFixEponaStealingSword, { CVAR_NAME });
