#include <libultraship/bridge.h>
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"
#include "soh/OTRGlobals.h"

extern "C" {
extern SaveContext gSaveContext;
#include "macros.h"
}

#define CVAR_PERMANENT_HEART_LOSS_NAME CVAR_ENHANCEMENT("PermanentHeartLoss")
#define CVAR_PERMANENT_HEART_LOSS_DEFAULT 0
#define CVAR_PERMANENT_HEART_LOSS_VALUE CVarGetInteger(CVAR_PERMANENT_HEART_LOSS_NAME, CVAR_PERMANENT_HEART_LOSS_DEFAULT)

static bool hasAffectedHealth = false;

void UpdatePermanentHeartLossState() {
    if (!GameInteractor::IsSaveLoaded()) return;

    if (!CVAR_PERMANENT_HEART_LOSS_VALUE && hasAffectedHealth) {
        uint8_t heartContainers = gSaveContext.ship.stats.heartContainers; // each worth 16 health
        uint8_t heartPieces = gSaveContext.ship.stats.heartPieces; // each worth 4 health, but only in groups of 4
        uint8_t startingHealth = 16 * (IS_RANDO ? (OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_STARTING_HEARTS) + 1) : 3);

        uint8_t newCapacity = startingHealth + (heartContainers * 16) + ((heartPieces - (heartPieces % 4)) * 4);
        gSaveContext.healthCapacity = MAX(newCapacity, gSaveContext.healthCapacity);
        gSaveContext.health = MIN(gSaveContext.health, gSaveContext.healthCapacity);
        hasAffectedHealth = false;
    }
}

void OnPlayerUpdatePermanentHeartLoss() {
    if (!GameInteractor::IsSaveLoaded()) {
        return;
    }

    if (gSaveContext.healthCapacity > 16 && gSaveContext.healthCapacity - gSaveContext.health >= 16) {
        gSaveContext.healthCapacity -= 16;
        gSaveContext.health = MIN(gSaveContext.health, gSaveContext.healthCapacity);
        hasAffectedHealth = true;
    }
}

void OnLoadGamePermanentHeartLoss(int16_t fileNum) {
    hasAffectedHealth = false;
    UpdatePermanentHeartLossState();
}

void RegisterPermanentHeartLoss() {
    COND_HOOK(OnLoadGame, CVAR_PERMANENT_HEART_LOSS_VALUE, OnLoadGamePermanentHeartLoss);
    COND_HOOK(OnPlayerUpdate, CVAR_PERMANENT_HEART_LOSS_VALUE, OnPlayerUpdatePermanentHeartLoss);
    UpdatePermanentHeartLossState();
}

static RegisterShipInitFunc initFunc(RegisterPermanentHeartLoss, { CVAR_PERMANENT_HEART_LOSS_NAME });
