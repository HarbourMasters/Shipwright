#include "soh/Enhancements/AdultMasks.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "z64.h"
#include "macros.h"
#include "variables.h"
}

#define CVAR_PERSISTENT_MASKS_NAME CVAR_ENHANCEMENT("PersistentMasks")

static void RegisterPersistentMasks() {
    bool persistentMasks = CVarGetInteger(CVAR_PERSISTENT_MASKS_NAME, 0);

    // A mask normally comes off the moment it leaves the buttons, keep it on instead
    COND_VB_SHOULD(VB_PLAYER_UNEQUIP_MASK_WITHOUT_BUTTON, persistentMasks, { *should = false; });

    // Put the remembered mask back on whenever the player actor is built, so it
    // survives loading zones, deaths and save loads
    COND_ID_HOOK(OnActorInit, ACTOR_PLAYER, persistentMasks, [](void* actorPtr) {
        // Forget the mask if it's gone from the inventory, or if adult can't wear it
        if (INV_CONTENT(ITEM_TRADE_CHILD) == ITEM_SOLD_OUT || (LINK_IS_ADULT && !Ship_MasksEquippableAsAdult())) {
            gSaveContext.ship.maskMemory = PLAYER_MASK_NONE;
        }
        ((Player*)actorPtr)->currentMask = gSaveContext.ship.maskMemory;
    });
}

static RegisterShipInitFunc initFunc(RegisterPersistentMasks, { CVAR_PERSISTENT_MASKS_NAME });
