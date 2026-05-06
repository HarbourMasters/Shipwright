#include <libultraship/bridge.h>
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/OTRGlobals.h"
#include "soh/ShipInit.hpp"
#include "z64save.h"

extern "C" {
extern SaveContext gSaveContext;
#include "variables.h"
#include "macros.h"
}

#define CVAR_INFINITE_AMMO_NAME CVAR_CHEAT("InfiniteAmmo")
#define CVAR_INFINITE_AMMO_DEFAULT 0
#define CVAR_INFINITE_AMMO_VALUE CVarGetInteger(CVAR_INFINITE_AMMO_NAME, CVAR_INFINITE_AMMO_DEFAULT)

void OnGameFrameUpdateInfiniteAmmo() {
    if (!GameInteractor::IsSaveLoaded(true)) {
        return;
    }

    AMMO(ITEM_STICK) = CUR_CAPACITY(UPG_STICKS);
    AMMO(ITEM_NUT) = CUR_CAPACITY(UPG_NUTS);
    AMMO(ITEM_BOMB) = CUR_CAPACITY(UPG_BOMB_BAG);
    AMMO(ITEM_BOW) = CUR_CAPACITY(UPG_QUIVER);
    AMMO(ITEM_SLINGSHOT) = CUR_CAPACITY(UPG_BULLET_BAG);
    if (INV_CONTENT(ITEM_BOMBCHU) != ITEM_NONE) {
        int chuCapacity = 50;
        if (IS_RANDO && RAND_GET_OPTION(RSK_BOMBCHU_BAG).Is(RO_BOMBCHU_BAG_PROGRESSIVE)) {
            chuCapacity = OTRGlobals::Instance->gRandoContext->GetBombchuCapacity();
        }
        AMMO(ITEM_BOMBCHU) = chuCapacity;
    }
}

void RegisterInfiniteAmmo() {
    COND_HOOK(OnGameFrameUpdate, CVAR_INFINITE_AMMO_VALUE, OnGameFrameUpdateInfiniteAmmo);
}

static RegisterShipInitFunc initFunc(RegisterInfiniteAmmo, { CVAR_INFINITE_AMMO_NAME });
