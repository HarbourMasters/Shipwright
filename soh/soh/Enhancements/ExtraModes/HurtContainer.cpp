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
    uint16_t heartPieceContainers = gSaveContext.ship.stats.heartPieces / 4;
    uint16_t heartContainers = gSaveContext.ship.stats.heartContainers;
    uint16_t healthCapacityMod = (heartPieceContainers + heartContainers) * 16;

    if (hurtEnabled != CVAR_HURT_CONTAINER_DEFAULT) {
        gSaveContext.healthCapacity = 320 - healthCapacityMod;
    } else {
        gSaveContext.healthCapacity = 48 + healthCapacityMod;
    }
}

static void RegisterHurtContainer() {
    COND_HOOK(OnLoadGame, hurtEnabled != CVAR_HURT_CONTAINER_VALUE, [](int32_t) { UpdateHurtContainerModeState(); });

    COND_VB_SHOULD(VB_HEARTS_INCREASE_WITH_CONTAINERS, CVAR_HURT_CONTAINER_VALUE, {
        *should = false;
        gSaveContext.healthCapacity -= 0x10;
        gSaveContext.health -= 0x10;
    });
}

static RegisterShipInitFunc initFunc(RegisterHurtContainer, { CVAR_HURT_CONTAINER_NAME });
