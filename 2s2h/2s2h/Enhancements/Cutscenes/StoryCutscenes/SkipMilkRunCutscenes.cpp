#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "variables.h"
}

#define CVAR_NAME "gEnhancements.Cutscenes.SkipStoryCutscenes"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterSkipMilkRunCutscenes() {
    // Skip initial ride cutscene to Gorman Track
    COND_VB_SHOULD(VB_PLAY_TRANSITION_CS, CVAR, {
        if (gSaveContext.save.cutsceneIndex == 0x0 && gSaveContext.save.entrance == ENTRANCE(ROMANI_RANCH, 11)) {
            gSaveContext.save.cutsceneIndex = 0;
            gSaveContext.save.entrance = ENTRANCE(GORMAN_TRACK, 4);
            // Add time that occurs during skipped cutscenes
            if (gSaveContext.save.timeSpeedOffset == -2) {
                gSaveContext.save.time += 0x50a; // ~30 min
            } else {
                gSaveContext.save.time += 0xf1e; // ~1 hour 24 min
            }
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterSkipMilkRunCutscenes, { CVAR_NAME });
