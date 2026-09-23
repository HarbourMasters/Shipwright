#include <libultraship/bridge/consolevariablebridge.h>

#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/game-interactor/vanilla-behavior/GIVanillaBehavior.h"
#include "soh/ShipInit.hpp"
#include "variables.h"

#define CVAR_FULL_HEALTH_SPAWN_NAME CVAR_ENHANCEMENT("FullHealthSpawn")
#define CVAR_FULL_HEALTH_SPAWN_VALUE CVarGetInteger(CVAR_FULL_HEALTH_SPAWN_NAME, 0)

static void RegisterFullhealthSpawn() {
    COND_VB_SHOULD(VB_FULL_HEALTH_SPAWN, CVAR_FULL_HEALTH_SPAWN_VALUE, {
        if (gSaveContext.health < gSaveContext.healthCapacity) {
            gSaveContext.health = gSaveContext.healthCapacity;
        }
        *should = false;
    });
}

static RegisterShipInitFunc initFunc(RegisterFullhealthSpawn, { CVAR_FULL_HEALTH_SPAWN_NAME });