#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "variables.h"
}

#define CVAR_NAME "gEnhancements.Player.InfiniteDekuHopping"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterInfiniteDekuHopping() {
    COND_VB_SHOULD(VB_DEKU_LINK_SPIN_ON_LAST_HOP, CVAR, {
        if (*should) {
            Player* player = GET_PLAYER(gPlayState);
            if (gSaveContext.save.saveInfo.playerData.health != 0) {
                player->remainingHopsCounter = 5;
            }
        }
    });

    COND_VB_SHOULD(VB_APPLY_AIR_CONTROL, CVAR, {
        f32* dekuSpeedTargetMultiplier = va_arg(args, f32*);
        Player* player = GET_PLAYER(gPlayState);

        if (player->transformation == PLAYER_FORM_DEKU && player->stateFlags3 & PLAYER_STATE3_200000) {
            // Deku Link's air control speedTarget gets halved, so we negate that by doubling it when deku hopping
            *dekuSpeedTargetMultiplier *= 2.0f;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterInfiniteDekuHopping, { CVAR_NAME });
