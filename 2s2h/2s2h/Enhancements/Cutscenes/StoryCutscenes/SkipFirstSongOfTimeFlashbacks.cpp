#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "variables.h"
}

#define CVAR_NAME "gEnhancements.Cutscenes.SkipStoryCutscenes"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterSkipFirstSongOfTimeFlashbacks() {
    COND_VB_SHOULD(VB_PLAY_TRANSITION_CS, CVAR, {
        if (gSaveContext.save.cutsceneIndex == 0 && gSaveContext.save.entrance == ENTRANCE(LOST_WOODS, 1)) {
            *should = false;
            // Override the destination
            // Ideally it should be ENTRANCE(SOUTH_CLOCK_TOWN, 10), but that plays text with Talt and I'm not sure how
            // to skip that part of the entrance cutscene
            gSaveContext.save.entrance = ENTRANCE(SOUTH_CLOCK_TOWN, 0);
            // Normally happens during the flashbacks
            gSaveContext.save.playerForm = PLAYER_FORM_DEKU;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterSkipFirstSongOfTimeFlashbacks, { CVAR_NAME });
