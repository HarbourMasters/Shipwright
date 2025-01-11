#include <libultraship/bridge.h>
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"

extern "C" PlayState* gPlayState;

#define CVAR_UNRESTRICTED_ITEMS_NAME CVAR_CHEAT("NoRestrictItems")
#define CVAR_UNRESTRICTED_ITEMS_DEFAULT 0
#define CVAR_UNRESTRICTED_ITEMS_VALUE CVarGetInteger(CVAR_UNRESTRICTED_ITEMS_NAME, CVAR_UNRESTRICTED_ITEMS_DEFAULT)

void OnGameFrameUpdateUnrestrictedItems() {
    if (!GameInteractor::IsSaveLoaded(true)) {
        return;
    }

    // do not allow the use of sun's song even with the cheat
    u8 sunsBackup = gPlayState->interfaceCtx.restrictions.sunsSong;
    memset(&gPlayState->interfaceCtx.restrictions, 0, sizeof(gPlayState->interfaceCtx.restrictions));
    gPlayState->interfaceCtx.restrictions.sunsSong = sunsBackup;
}

void RegisterUnrestrictedItems() {
    COND_HOOK(OnGameFrameUpdate, CVAR_UNRESTRICTED_ITEMS_VALUE, OnGameFrameUpdateUnrestrictedItems);
}

static RegisterShipInitFunc initFunc(RegisterUnrestrictedItems, { CVAR_UNRESTRICTED_ITEMS_NAME });
