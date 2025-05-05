#include <libultraship/bridge.h>
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Notification/Notification.h"
#include "soh/ShipInit.hpp"
#include "soh/SaveManager.h"

extern "C" {
extern PlayState* gPlayState;
#include "functions.h"
#include "variables.h"
}

static uint64_t lastSaveTimestamp = GetUnixTimestamp();

#define CVAR_AUTOSAVE_NAME CVAR_ENHANCEMENT("Autosave")
#define CVAR_AUTOSAVE_DEFAULT AUTOSAVE_OFF
#define CVAR_AUTOSAVE_VALUE CVarGetInteger(CVAR_AUTOSAVE_NAME, CVAR_AUTOSAVE_DEFAULT)
#define THREE_MINUTES_IN_UNIX 3 * 60000

typedef enum {
    AUTOSAVE_OFF,
    AUTOSAVE_ON,
} AutosaveOptions;

bool Autosave_CanSave() {

    // Don't save when in title screen or debug file
    // Don't save the first 60 frames to not save the magic meter when it's still in the animation of filling it.
    // Don't save in Chamber of Sages and the Cutscene map because of remember save location and cutscene item gives.
    if (!GameInteractor::IsSaveLoaded(false) || gPlayState->gameplayFrames < 60 ||
        gPlayState->sceneNum == SCENE_CHAMBER_OF_THE_SAGES || gPlayState->sceneNum == SCENE_CUTSCENE_MAP) {
        return false;
    }

    return true;
}

void Autosave_PerformSave() {
    Play_PerformSave(gPlayState);

    // Send notification
    Notification::Emit({
        .message = "Game autosaved",
    });
}

void Autosave_IntervalSave() {
    // Check if the interval has passed in minutes.
    uint64_t currentTimestamp = GetUnixTimestamp();
    if ((currentTimestamp - lastSaveTimestamp) < THREE_MINUTES_IN_UNIX) {
        return;
    }

    // If save available to create, do it and reset the interval.
    // Interval gets extra check for being paused to avoid rare issues like bypassing shop
    // rupees draining after buying an item. Since the interval can just retry until it
    // passes, it can use more conditions without hampering the player experience.
    if (Autosave_CanSave() && !GameInteractor::IsGameplayPaused()) {

        // Reset timestamp, set icon timer to show autosave icon for 5 seconds (100 frames)
        lastSaveTimestamp = currentTimestamp;

        Autosave_PerformSave();
    }
}

void Autosave_SoftResetSave() {
    if (Autosave_CanSave()) {
        Autosave_PerformSave();
    }
}

void RegisterAutosave() {
    COND_HOOK(GameInteractor::OnGameFrameUpdate, CVAR_AUTOSAVE_VALUE, Autosave_IntervalSave);
    COND_HOOK(GameInteractor::OnExitGame, CVAR_AUTOSAVE_VALUE, [](int32_t fileNum) { Autosave_SoftResetSave(); });
}

static RegisterShipInitFunc initFunc(RegisterAutosave, { CVAR_AUTOSAVE_NAME });
