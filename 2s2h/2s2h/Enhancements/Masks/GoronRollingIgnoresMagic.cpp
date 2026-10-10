#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

#define CVAR_NAME "gEnhancements.Masks.GoronRollingIgnoresMagic"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

#define CVAR_R_NAME "gEnhancements.Masks.GoronRollingSpikesRequireShield"
#define CVAR_R CVarGetInteger(CVAR_R_NAME, 0)

extern "C" {
#include "macros.h"
#include "variables.h"
extern PlayState* gPlayState;
extern Input* sPlayerControlInput;
}

void RegisterGoronRollingIgnoresMagic() {
    // Disable magic consumption
    COND_VB_SHOULD(VB_GORON_ROLL_CONSUME_MAGIC, CVAR, { *should = false; });

    // Disable check for if the player has magic to increase spike level
    COND_VB_SHOULD(VB_GORON_ROLL_INCREASE_SPIKE_LEVEL, CVAR || CVAR_R, {
        if (CVAR) {
            Player* player = GET_PLAYER(gPlayState);
            *should = player->av2.actionVar2 >= 0x36B0;
        }

        *should = *should && (!CVAR_R || CHECK_BTN_ALL(sPlayerControlInput->cur.button, BTN_R));
    });

    // Mimicking the vanilla condition minus the magic check
    COND_VB_SHOULD(VB_GORON_ROLL_DISABLE_SPIKE_MODE, CVAR || CVAR_R, {
        Player* player = GET_PLAYER(gPlayState);
        bool disableSpikes = !CVAR || ((player->stateFlags3 & PLAYER_STATE3_80000) &&
                                       (!CHECK_BTN_ALL(sPlayerControlInput->cur.button, BTN_A) ||
                                        ((player->av1.actionVar1 == 4) && (player->unk_B08 < 12.0f))));
        if (!disableSpikes) {
            *should = false;
        }
        if (CVAR_R && !CHECK_BTN_ALL(sPlayerControlInput->cur.button, BTN_R)) {
            *should = true;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterGoronRollingIgnoresMagic, { CVAR_NAME, CVAR_R_NAME });
