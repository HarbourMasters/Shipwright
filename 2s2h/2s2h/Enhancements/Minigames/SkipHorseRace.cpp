#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/Enhancements/Enhancements.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "functions.h"
#include "variables.h"
#include "z64horse.h"
}

#define CVAR_NAME "gEnhancements.Minigames.SkipHorseRace"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterSkipHorseRace() {

    COND_VB_SHOULD(VB_PLAY_TRANSITION_CS, CVAR, {
        if (gSaveContext.save.cutsceneIndex == 0x0 && gSaveContext.save.entrance == ENTRANCE(GORMAN_TRACK, 5)) {
            gSaveContext.save.entrance = ENTRANCE(GORMAN_TRACK, 2);
            SET_WEEKEVENTREG_HORSE_RACE_STATE(WEEKEVENTREG_HORSE_RACE_STATE_2);
            // prevent horse race start music
            gSaveContext.forcedSeqId = NA_BGM_GENERAL_SFX;
            gHorseIsMounted = false;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterSkipHorseRace, { CVAR_NAME });
