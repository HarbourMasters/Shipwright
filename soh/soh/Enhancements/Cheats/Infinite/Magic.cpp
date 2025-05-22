#include <libultraship/bridge.h>
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"
#include "z64save.h"

extern "C" SaveContext gSaveContext;

#define CVAR_INFINITE_MAGIC_NAME CVAR_CHEAT("InfiniteMagic")
#define CVAR_INFINITE_MAGIC_DEFAULT 0
#define CVAR_INFINITE_MAGIC_VALUE CVarGetInteger(CVAR_INFINITE_MAGIC_NAME, CVAR_INFINITE_MAGIC_DEFAULT)

void OnGameFrameUpdateInfiniteMagic() {
    if (!GameInteractor::IsSaveLoaded(true)) {
        return;
    }

    gSaveContext.magic = gSaveContext.magicLevel * MAGIC_NORMAL_METER;
}

void RegisterInfiniteMagic() {
    COND_HOOK(OnGameFrameUpdate, CVAR_INFINITE_MAGIC_VALUE, OnGameFrameUpdateInfiniteMagic);
}

static RegisterShipInitFunc initFunc(RegisterInfiniteMagic, { CVAR_INFINITE_MAGIC_NAME });
