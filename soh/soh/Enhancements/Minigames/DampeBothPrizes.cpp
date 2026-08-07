#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "functions.h"
#include "macros.h"
extern SaveContext gSaveContext;
extern PlayState* gPlayState;
}

#define CVAR_DAMPE_BOTH_PRIZES_NAME CVAR_ENHANCEMENT("DampeBothPrizes")
#define CVAR_DAMPE_BOTH_PRIZES_VALUE CVarGetInteger(CVAR_DAMPE_BOTH_PRIZES_NAME, 0)

static void RegisterDampeBothPrizes() {
    COND_VB_SHOULD(VB_DAMPE_AWARD_SECOND_PRIZE, CVAR_DAMPE_BOTH_PRIZES_VALUE || IS_RANDO, {
        if (!*should) {
            Flags_SetTempClear(gPlayState, 4);
            HIGH_SCORE(HS_DAMPE_RACE) = gSaveContext.timerSeconds;
            if (IS_RANDO) {
                Flags_SetTreasure(gPlayState, 0x1E);
            }
            *should = true;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterDampeBothPrizes, { CVAR_DAMPE_BOTH_PRIZES_NAME, "IS_RANDO" });
