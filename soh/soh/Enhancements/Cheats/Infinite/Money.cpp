#include <libultraship/bridge.h>
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"
#include "z64save.h"
#include "variables.h"

extern "C" {
extern SaveContext gSaveContext;
extern s32 Flags_GetRandomizerInf(RandomizerInf flag);
#include "macros.h"
}

#define CVAR_INFINITE_MONEY_NAME CVAR_CHEAT("InfiniteMoney")
#define CVAR_INFINITE_MONEY_DEFAULT 0
#define CVAR_INFINITE_MONEY_VALUE CVarGetInteger(CVAR_INFINITE_MONEY_NAME, CVAR_INFINITE_MONEY_DEFAULT)

void OnGameFrameUpdateInfiniteMoney() {
    if (
        !GameInteractor::IsSaveLoaded(true) ||
        (IS_RANDO && !Flags_GetRandomizerInf(RAND_INF_HAS_WALLET))
    ) {
        return;
    }

    gSaveContext.rupees = CUR_CAPACITY(UPG_WALLET);
}

void RegisterInfiniteMoney() {
    COND_HOOK(OnGameFrameUpdate, CVAR_INFINITE_MONEY_VALUE, OnGameFrameUpdateInfiniteMoney);
}

static RegisterShipInitFunc initFunc(RegisterInfiniteMoney, { CVAR_INFINITE_MONEY_NAME });
