#include <libultraship/bridge.h>
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"

extern "C" {
extern SaveContext gSaveContext;
extern PlayState* gPlayState;
#include "macros.h"
}

#define CVAR_BONK_DAMAGE_NAME CVAR_ENHANCEMENT("HurtContainer")
#define CVAR_BONK_DAMAGE_DEFAULT 0
#define CVAR_BONK_DAMAGE_VALUE CVarGetInteger(CVAR_BONK_DAMAGE_NAME, CVAR_BONK_DAMAGE_DEFAULT)

void OnLoadGameHurtContainerMode(int32_t _fileNum) {
    static bool hurtEnabled = false;
    if (hurtEnabled == CVAR_BONK_DAMAGE_VALUE) {
        return;
    }

    hurtEnabled = CVAR_BONK_DAMAGE_VALUE;
    uint16_t getHeartPieces = gSaveContext.ship.stats.heartPieces / 4;
    uint16_t getHeartContainers = gSaveContext.ship.stats.heartContainers;

    if (hurtEnabled) {
        gSaveContext.healthCapacity = 320 - ((getHeartPieces + getHeartContainers) * 16);
    } else {
        gSaveContext.healthCapacity = 48 + ((getHeartPieces + getHeartContainers) * 16);
    }
}

void RegisterHurtContainerMode() {
    COND_HOOK(OnLoadGame, true, OnLoadGameHurtContainerMode);
    OnLoadGameHurtContainerMode(0);
}

static RegisterShipInitFunc initFunc(RegisterHurtContainerMode, { CVAR_BONK_DAMAGE_NAME });
