#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "variables.h"
#include "functions.h"
}

#define CVAR_NAME "gEnhancements.Cutscenes.SkipStoryCutscenes"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterSkipClockTowerOpen() {
    // This will handle skipping if you are around the Clock Town area, but not directly in south clock town
    COND_VB_SHOULD(VB_CLOCK_TOWER_OPENING_CONSIDER_THIS_FIRST_CYCLE, CVAR, {
        if (CVarGetInteger("gEnhancements.Cutscenes.SkipStoryCutscenes", 0)) {
            *should = false;
        }
    });

    // This will handle skipping if you are directly in South Clock Town
    COND_VB_SHOULD(VB_PLAY_TRANSITION_CS, CVAR, {
        if ((gSaveContext.save.entrance == ENTRANCE(SOUTH_CLOCK_TOWN, 0) ||
             gSaveContext.save.entrance == ENTRANCE(TERMINA_FIELD, 0)) &&
            gSaveContext.save.cutsceneIndex == 0xFFF1 &&
            CVarGetInteger("gEnhancements.Cutscenes.SkipStoryCutscenes", 0)) {
            // Copied from ObjTokeidai_TowerOpening_EndCutscene
            SET_WEEKEVENTREG(WEEKEVENTREG_CLOCK_TOWER_OPENED);
            gSaveContext.save.cutsceneIndex = 0;
            gSaveContext.respawnFlag = 2;
            gSaveContext.save.entrance = gSaveContext.respawn[RESPAWN_MODE_RETURN].entrance;
            if (gSaveContext.respawn[RESPAWN_MODE_RETURN].playerParams ==
                PLAYER_PARAMS(0xFF, PLAYER_START_MODE_TELESCOPE)) {
                gSaveContext.nextTransitionType = TRANS_TYPE_CIRCLE;
            } else {
                gSaveContext.nextTransitionType = TRANS_TYPE_FADE_BLACK;
            }
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterSkipClockTowerOpen, { CVAR_NAME });
