#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "variables.h"
}

#define CVAR_NAME "gEnhancements.Cutscenes.SkipStoryCutscenes"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

/**
 *  Skips only the visible cutscenes (Clock Tower destruction and Link's terrible fate with the Fire Wall).
 *  Note: There is a cutscene command SPECIFICALLY for resetting game state related to the moon crash,
 *        CS_MISC_RESET_SAVE_FROM_MOON_CRASH, that MUST NOT be skipped, or else
 *        it would break the vanilla functionality of a Moon Crash reset.
 */
void RegisterSkipMoonCrash() {
    // Skips initial cutscene in Termina Field where the moon falls, goes straight to clock tower cutscene
    COND_VB_SHOULD(VB_PLAY_TRANSITION_CS, CVAR, {
        // The respawn flag check here is so that we don't skip if the player is trying to pull off the 4th day
        // glitch from the telescope
        if (gSaveContext.save.cutsceneIndex == 0x0 && gSaveContext.save.entrance == ENTRANCE(TERMINA_FIELD, 12) &&
            gSaveContext.respawnFlag == 0) {
            // This cutscene command would otherwise be run as part of the Fire Wall cutscene that gets skipped.
            // IT MUST BE CALLED HERE IF THAT CUTSCENE IS SKIPPED OR ELSE THE GAME STATE WILL NOT RESET CORRECTLY!
            Sram_ResetSaveFromMoonCrash(&gPlayState->sramCtx);

            gSaveContext.save.entrance = ENTRANCE(CLOCK_TOWER_INTERIOR, 3);
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterSkipMoonCrash, { CVAR_NAME });
