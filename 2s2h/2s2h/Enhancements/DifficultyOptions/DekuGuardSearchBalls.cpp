#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "Enhancements/Enhancements.h"
#include "2s2h/ShipInit.hpp"

#define CVAR_NAME "gEnhancements.DifficultyOptions.DekuGuardSearchBalls"
#define CVAR CVarGetInteger(CVAR_NAME, DEKU_GUARD_SEARCH_BALLS_NIGHT_ONLY)

void RegisterShowDekuGuardSearchBalls() {
    COND_VB_SHOULD(VB_DEKU_GUARD_SHOW_SEARCH_BALLS, CVAR != DEKU_GUARD_SEARCH_BALLS_NIGHT_ONLY, {
        switch (CVAR) {
            case DEKU_GUARD_SEARCH_BALLS_NEVER:
                *should = false;
                break;
            case DEKU_GUARD_SEARCH_BALLS_ALWAYS:
                *should = true;
                break;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterShowDekuGuardSearchBalls, { CVAR_NAME });
