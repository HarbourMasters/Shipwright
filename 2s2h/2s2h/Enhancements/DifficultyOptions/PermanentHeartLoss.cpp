#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "variables.h"
}

#define CVAR_NAME "gEnhancements.DifficultyOptions.PermanentHeartLoss"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterPermanentHeartLoss() {
    COND_ID_HOOK(OnActorUpdate, ACTOR_PLAYER, CVAR, [](Actor* actor) {
        if (gSaveContext.save.saveInfo.playerData.healthCapacity > 16 &&
            gSaveContext.save.saveInfo.playerData.healthCapacity - gSaveContext.save.saveInfo.playerData.health >= 16) {
            gSaveContext.save.saveInfo.playerData.healthCapacity -= 16;
            gSaveContext.save.saveInfo.playerData.health =
                MIN(gSaveContext.save.saveInfo.playerData.health, gSaveContext.save.saveInfo.playerData.healthCapacity);
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterPermanentHeartLoss, { CVAR_NAME });
