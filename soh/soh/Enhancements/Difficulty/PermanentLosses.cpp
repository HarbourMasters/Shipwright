#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/OTRGlobals.h"
#include "soh/SaveManager.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "functions.h"
#include "macros.h"
#include "variables.h"
#include "z64save.h"
extern SaveContext gSaveContext;
extern PlayState* gPlayState;
}

static constexpr int32_t CVAR_PERM_HEART_LOSS_DEFAULT = 0;
#define CVAR_PERM_HEART_LOSS_NAME CVAR_ENHANCEMENT("PermanentHeartLoss")
#define CVAR_PERM_HEART_LOSS_VALUE CVarGetInteger(CVAR_PERM_HEART_LOSS_NAME, CVAR_PERM_HEART_LOSS_DEFAULT)

static constexpr int32_t CVAR_DELETE_FILE_DEFAULT = 0;
#define CVAR_DELETE_FILE_NAME CVAR_ENHANCEMENT("DeleteFileOnDeath")
#define CVAR_DELETE_FILE_VALUE CVarGetInteger(CVAR_DELETE_FILE_NAME, CVAR_DELETE_FILE_DEFAULT)

static bool hasAffectedHealth = false;

static void UpdatePermanentHeartLossState() {
    // Reset Link's hearts to the normal value without permanent losses. Only applies if all of the following are true:
    // - A saved game is playing
    // - The "Permanent Heart Loss" setting is turned off
    // - The player has lost at least one Heart Container
    if (!GameInteractor::IsSaveLoaded() || !hasAffectedHealth || CVAR_PERM_HEART_LOSS_VALUE) {
        return;
    }

    uint8_t heartContainers = gSaveContext.ship.stats.heartContainers; // each worth 16 health
    uint8_t heartPieces = gSaveContext.ship.stats.heartPieces;         // each worth 4 health, but only in groups of 4
    uint8_t startingHealth =
        16 * (IS_RANDO ? (OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_STARTING_HEARTS) + 1) : 3);

    uint8_t newCapacity = startingHealth + (heartContainers * 16) + ((heartPieces - (heartPieces % 4)) * 4);
    gSaveContext.healthCapacity = MAX(newCapacity, gSaveContext.healthCapacity);
    gSaveContext.health = MIN(gSaveContext.health, gSaveContext.healthCapacity);
    hasAffectedHealth = false;
}

static void UpdateHealthCapacity() {
    // Applies permanent losses of Heart Containers to Link's health. Only applies when a saved game is playing.
    if (!GameInteractor::IsSaveLoaded()) {
        return;
    }

    if (gSaveContext.healthCapacity > 16 && gSaveContext.healthCapacity - gSaveContext.health >= 16) {
        gSaveContext.healthCapacity -= 16;
        gSaveContext.health = MIN(gSaveContext.health, gSaveContext.healthCapacity);
        hasAffectedHealth = true;
    }
}

static void DeleteFileOnDeath() {
    if (!GameInteractor::IsSaveLoaded() || gPlayState == NULL) {
        return;
    }

    if (gPlayState->gameOverCtx.state == GAMEOVER_DEATH_MENU && gPlayState->pauseCtx.state == 9) {
        SaveManager::Instance->DeleteZeldaFile(gSaveContext.fileNum);
        hasAffectedHealth = false;
        std::reinterpret_pointer_cast<Ship::ConsoleWindow>(
            Ship::Context::GetInstance()->GetWindow()->GetGui()->GetGuiWindow("Console"))
            ->Dispatch("reset");
    }
}

static void RegisterPermanentHeartLoss() {
    UpdatePermanentHeartLossState();
    COND_HOOK(OnPlayerUpdate, CVAR_PERM_HEART_LOSS_VALUE, UpdateHealthCapacity);
}

static void RegisterDeleteFileOnDeath() {
    COND_HOOK(OnGameFrameUpdate, CVAR_DELETE_FILE_VALUE, DeleteFileOnDeath);
}

static void RegisterResetAffectedHealthOnLoad() {
    COND_HOOK(OnLoadGame, true, [](int16_t) { hasAffectedHealth = false; });
}

static RegisterShipInitFunc initFunc_PermanentHeartLoss(RegisterPermanentHeartLoss, { CVAR_PERM_HEART_LOSS_NAME });
static RegisterShipInitFunc initFunc_DeleteFileOnDeath(RegisterDeleteFileOnDeath, { CVAR_DELETE_FILE_NAME });
static RegisterShipInitFunc initFunc_ResetAffectedHealth(RegisterResetAffectedHealthOnLoad);
