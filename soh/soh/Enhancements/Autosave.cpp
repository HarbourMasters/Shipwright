#include <libultraship/bridge.h>
#include "AutoSave.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Notification/Notification.h"
#include "soh/ShipInit.hpp"

extern "C" {
extern PlayState* gPlayState;
#include "functions.h"
#include "variables.h"
}

static uint32_t autosaveInterval = 0;
static uint64_t currentTimestamp = 0;
static uint64_t lastSaveTimestamp = GetUnixTimestamp();

#define CVAR_AUTOSAVE_NAME CVAR_ENHANCEMENT("Autosave")
#define CVAR_AUTOSAVE_DEFAULT AUTOSAVE_OFF
#define CVAR_AUTOSAVE_VALUE CVarGetInteger(CVAR_AUTOSAVE_NAME, CVAR_AUTOSAVE_DEFAULT)
#define CVAR_AUTOSAVE_INTERVAL_NAME CVAR_ENHANCEMENT("AutosaveInterval")
#define CVAR_AUTOSAVE_INTERVAL_DEFAULT 5
#define CVAR_AUTOSAVE_INTERVAL_VALUE CVarGetInteger(CVAR_AUTOSAVE_INTERVAL_NAME, CVAR_AUTOSAVE_INTERVAL_DEFAULT)

bool Autosave_CanSave() {

    // Regular checks
    if (!GameInteractor::IsSaveLoaded(true) || GameInteractor::IsGameplayPaused()) {
        return false;
    }

    // Don't autosave in Ganon's fight to not have Master sword shenanigans
    if (gPlayState->sceneNum == SCENE_GANON_BOSS) {
        return false;
    }

    // Don't autosave immediately after buying items from shops to prevent getting them for free
    if (gSaveContext.ship.pendingSale != ITEM_NONE) {
        return false;
    }

    return true;
}

void Autosave_PerformSave() {
    // Check if the interval has passed in minutes.
    autosaveInterval = CVAR_AUTOSAVE_INTERVAL_VALUE * 60000;
    currentTimestamp = GetUnixTimestamp();
    if ((currentTimestamp - lastSaveTimestamp) < autosaveInterval) {
        return;
    }

    // If save available to create, do it and reset the interval.
    if (Autosave_CanSave()) {

        // Reset timestamp, set icon timer to show autosave icon for 5 seconds (100 frames)
        lastSaveTimestamp = currentTimestamp;

        Play_PerformSave(gPlayState);

        // Send notification
        Notification::Emit({
            .message = "Game autosaved",
        });
    }
}

void RegisterAutosave() {
    COND_HOOK(OnGameFrameUpdate, CVAR_AUTOSAVE_VALUE, Autosave_PerformSave);
}

static RegisterShipInitFunc initFunc(RegisterAutosave, { CVAR_AUTOSAVE_NAME });
