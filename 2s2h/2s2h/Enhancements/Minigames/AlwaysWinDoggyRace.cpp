#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/Enhancements/Enhancements.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "variables.h"
}

#define CVAR_NAME "gEnhancements.Minigames.AlwaysWinDoggyRace"
#define CVAR CVarGetInteger(CVAR_NAME, ALWAYS_WIN_DOGGY_RACE_OFF)

void RegisterAlwaysWinDoggyRace() {
    COND_VB_SHOULD(VB_DOGGY_RACE_SET_MAX_SPEED, CVAR != ALWAYS_WIN_DOGGY_RACE_OFF, {
        if (CVAR == ALWAYS_WIN_DOGGY_RACE_ALWAYS ||
            (CVAR == ALWAYS_WIN_DOGGY_RACE_MASKOFTRUTH && (INV_CONTENT(ITEM_MASK_TRUTH) == ITEM_MASK_TRUTH))) {
            *should = true;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterAlwaysWinDoggyRace, { CVAR_NAME });
