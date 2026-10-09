#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "variables.h"
#include "overlays/gamestates/ovl_file_choose/z_file_select.h"
#include "overlays/gamestates/ovl_title/z_title.h"
}

#define CVAR_NAME "gEnhancements.Cutscenes.SkipToFileSelect"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

static constexpr int32_t OPENING_CS_INDEX = 0xFFFA;

// Normally the following is called by the opening cutscene
static void SkipToFileSelect() {
    Sram_InitNewSave();
    gSaveContext.save.time = CLOCK_TIME(8, 0);
    gSaveContext.save.day = 1;
    gSaveContext.save.playerForm = PLAYER_FORM_HUMAN;
    gSaveContext.gameMode = GAMEMODE_FILE_SELECT;

    STOP_GAMESTATE(gGameState);
    SET_NEXT_GAMESTATE(gGameState, FileSelect_Init, sizeof(FileSelectState));
}

static void RegisterSkipToFileSelect() {
    COND_VB_SHOULD(VB_PLAY_TRANSITION_CS, CVAR, {
        if ((gSaveContext.save.entrance == ENTRANCE(CUTSCENE, 0) ||
             gSaveContext.save.entrance == ENTRANCE(CUTSCENE, 1)) &&
            gSaveContext.save.cutsceneIndex == OPENING_CS_INDEX) {
            SkipToFileSelect();
        }
    });

    COND_HOOK(OnConsoleLogoUpdate, CVAR, []() {
        ConsoleLogoState* consoleLogoState = (ConsoleLogoState*)gGameState;

        // Wait for the console logo to fade out
        if (consoleLogoState->exit) {
            // Normally the PRNG seed is set at least once from the title opening running Play_Init
            // We need to call it manually before file select creates RNG values for new saves
            Rand_Seed(osGetTime());
            // Normally called on console logo screen
            gSaveContext.seqId = NA_BGM_DISABLED;
            gSaveContext.ambienceId = AMBIENCE_ID_DISABLED;

            SkipToFileSelect();
        }
    });

    // Allows pressing A to skip the boot logo and go to the next state (opening or file select)
    COND_HOOK(OnConsoleLogoUpdate, true, []() {
        ConsoleLogoState* consoleLogoState = (ConsoleLogoState*)gGameState;

        if (CHECK_BTN_ANY(consoleLogoState->state.input->press.button, BTN_A | BTN_B | BTN_START)) {
            // Force the title state to start fading to black and to last roughly 5 frames based on current fade in/out
            consoleLogoState->visibleDuration = 0;
            consoleLogoState->addAlpha = std::max<int16_t>((255 - consoleLogoState->coverAlpha) / 5, 1);
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterSkipToFileSelect, { CVAR_NAME });
