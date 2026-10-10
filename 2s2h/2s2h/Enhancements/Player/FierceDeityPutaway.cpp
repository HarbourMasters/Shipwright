#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "variables.h"
}

#define CVAR_NAME "gEnhancements.Player.FierceDeityPutaway"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterFierceDeityPutaway() {
    COND_VB_SHOULD(VB_SHOULD_PUTAWAY, CVAR, {
        Player* player = GET_PLAYER(gPlayState);
        if (player->transformation == PLAYER_FORM_FIERCE_DEITY) {
            *should = true;
        }
    });

    COND_VB_SHOULD(VB_FD_ALWAYS_WIELD_SWORD, CVAR, { *should = false; });
}

static RegisterShipInitFunc initFunc(RegisterFierceDeityPutaway, { CVAR_NAME });
