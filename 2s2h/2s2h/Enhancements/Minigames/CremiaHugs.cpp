#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/Enhancements/Enhancements.h"
#include "2s2h/ShipInit.hpp"

#define CVAR_NAME "gEnhancements.Minigames.CremiaHugs"
#define CVAR CVarGetInteger(CVAR_NAME, CREMIA_REWARD_RANDOM)

void RegisterCremiaHugs() {
    COND_VB_SHOULD(VB_PLAY_CREMIA_HUG_CUTSCENE, CVAR != CREMIA_REWARD_RANDOM, {
        if (CVAR == CREMIA_REWARD_ALWAYS_HUG) {
            *should = true;
        } else if (CVAR == CREMIA_REWARD_ALWAYS_RUPEE) {
            *should = false;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterCremiaHugs, { CVAR_NAME });
