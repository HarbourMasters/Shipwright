#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/Enhancements/mods.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "variables.h"
extern SaveContext gSaveContext;
}

static constexpr int32_t CVAR_HURT_CONTAINER_DEFAULT = 0;
#define CVAR_HURT_CONTAINER_NAME CVAR_ENHANCEMENT("HurtContainer")
#define CVAR_HURT_CONTAINER_VALUE CVarGetInteger(CVAR_HURT_CONTAINER_NAME, CVAR_HURT_CONTAINER_DEFAULT)

static bool hurtEnabled = false;

void UpdateHurtContainerModeState() {
    hurtEnabled = CVAR_HURT_CONTAINER_VALUE;
    uint16_t getHeartPieces = gSaveContext.ship.stats.heartPieces / 4;
    uint16_t getHeartContainers = gSaveContext.ship.stats.heartContainers;

    if (hurtEnabled) {
        gSaveContext.healthCapacity = 320 - ((getHeartPieces + getHeartContainers) * 16);
    } else {
        gSaveContext.healthCapacity = 48 + ((getHeartPieces + getHeartContainers) * 16);
    }
}

static void RegisterHurtContainer() {
    COND_HOOK(OnLoadGame, hurtEnabled != CVAR_HURT_CONTAINER_VALUE, [](int32_t) { UpdateHurtContainerModeState(); });
}

static RegisterShipInitFunc initFunc(RegisterHurtContainer, { CVAR_HURT_CONTAINER_NAME });
