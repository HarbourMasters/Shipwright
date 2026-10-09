#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "variables.h"
}

#define CVAR_NAME "gEnhancements.Cutscenes.SkipMiscInteractions"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterSkipEvansSong() {
    // Skips only the actual music portion
    COND_VB_SHOULD(VB_PLAY_TRANSITION_CS, CVAR, {
        if (gSaveContext.save.cutsceneIndex == 0xFFF3 && gSaveContext.save.entrance == ENTRANCE(CUTSCENE, 2)) {
            gSaveContext.save.cutsceneIndex = 0;
            gSaveContext.save.entrance = ENTRANCE(ZORA_HALL_ROOMS, 6);
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterSkipEvansSong, { CVAR_NAME });
