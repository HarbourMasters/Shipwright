#include "SavingEnhancements.h"
#include <libultraship/bridge/consolevariablebridge.h>
#include "BenPort.h"
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"
#include "2s2h/CustomMessage/CustomMessage.h"

extern "C" {
#include <variables.h>
#include <functions.h>
}

#define CVAR_AUTOSAVE_NAME "gEnhancements.Saving.Autosave"
#define CVAR_AUTOSAVE CVarGetInteger(CVAR_AUTOSAVE_NAME, 0)

static uint32_t autosaveInterval = 0;
static uint32_t iconTimer = 0;
static uint64_t currentTimestamp = 0;
static uint64_t lastSaveTimestamp = GetUnixTimestamp();

void DrawAutosaveIcon() {
    // 5 seconds (100 frames) of showing the owl save icon to signify autosave has happened.
    if (iconTimer != 0) {
        float opacity = 255.0;
        // Fade in icon
        if (iconTimer > 80) {
            opacity = 255.0 - (((iconTimer - 80.0) / 20.0) * 255);
            // Fade out icon
        } else if (iconTimer < 20) {
            opacity = (iconTimer / 20.0) * 255.0;
        }
        Interface_DrawAutosaveIcon(gPlayState, uint16_t(opacity));
        iconTimer--;
    }
}

void HandleAutoSave() {
    // Check if the interval has passed in minutes.
    autosaveInterval = CVarGetInteger("gEnhancements.Saving.AutosaveInterval", 5) * 60000;
    currentTimestamp = GetUnixTimestamp();
    if ((currentTimestamp - lastSaveTimestamp) < autosaveInterval) {
        return;
    }

    Player* player = GET_PLAYER(gPlayState);
    if (player == NULL) {
        return;
    }

    // Time must be moving for autosave to work. Fixes an issue with Time Moves When You Move.
    if (gSaveContext.save.timeSpeedOffset == -R_TIME_SPEED && R_TIME_SPEED != 0) {
        return;
    }

    // If owl save available to create, do it and reset the interval.
    if (SavingEnhancements_CanSave() && gPlayState->pauseCtx.state == 0) {

        // Reset timestamp, set icon timer to show autosave icon for 5 seconds (100 frames)
        lastSaveTimestamp = GetUnixTimestamp();
        iconTimer = 100;

        // Persist this in case the user is 0th daying
        bool currentOwlSaveState = gSaveContext.save.isOwlSave;

        // Create owl save
        gSaveContext.save.isOwlSave = true;
        SavingEnhancements_PersistSaveEntranceInfo();
        SavingEnhancements_AdvancePlaytime();
        Play_SaveCycleSceneFlags(gPlayState);
        gSaveContext.save.saveInfo.playerData.savedSceneId = gPlayState->sceneId;
        func_8014546C(&gPlayState->sramCtx);
        Sram_SetFlashPagesOwlSave(&gPlayState->sramCtx,
                                  gFlashOwlSaveStartPages[gSaveContext.fileNum * FLASH_SAVE_MAIN_MULTIPLIER],
                                  gFlashOwlSaveNumPages[gSaveContext.fileNum * FLASH_SAVE_MAIN_MULTIPLIER]);
        Sram_StartWriteToFlashOwlSave(&gPlayState->sramCtx);
        gSaveContext.save.isOwlSave = currentOwlSaveState;
        SavingEnhancements_ClearSaveEntranceInfo();
    }
}

static RegisterShipInitFunc registerAutosave(
    []() {
        COND_HOOK(OnGameStateUpdate, CVAR_AUTOSAVE, []() {
            if (gPlayState == nullptr) {
                return;
            }

            HandleAutoSave();
        });

        COND_HOOK(OnGameStateDrawFinish, CVAR_AUTOSAVE, []() {
            if (gPlayState == nullptr) {
                return;
            }

            DrawAutosaveIcon();
        });
    },
    { CVAR_AUTOSAVE_NAME });
