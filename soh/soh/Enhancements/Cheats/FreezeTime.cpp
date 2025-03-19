#include <libultraship/bridge.h>
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"
#include "z64save.h"

extern "C" SaveContext gSaveContext;

#define CVAR_FREEZE_TIME_NAME CVAR_CHEAT("FreezeTime")
#define CVAR_FREEZE_TIME_DEFAULT 0
#define CVAR_FREEZE_TIME_VALUE CVarGetInteger(CVAR_FREEZE_TIME_NAME, CVAR_FREEZE_TIME_DEFAULT)

#define CVAR_PREV_TIME_NAME CVAR_GENERAL("PrevTime")
#define CVAR_PREV_TIME_DEFAULT -1
#define CVAR_PREV_TIME_VALUE CVarGetInteger(CVAR_PREV_TIME_NAME, CVAR_PREV_TIME_DEFAULT)

void OnGameFrameUpdateFreezeTime() {
    if (!GameInteractor::IsSaveLoaded(true)) {
        return;
    }

    if (CVAR_PREV_TIME_VALUE == CVAR_PREV_TIME_DEFAULT) {
        CVarSetInteger(CVAR_PREV_TIME_NAME, gSaveContext.dayTime);
    }

    gSaveContext.dayTime = CVAR_PREV_TIME_VALUE;
}

void RegisterFreezeTime() {
    static HOOK_ID hookId = 0;
    GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnGameFrameUpdate>(hookId);
    hookId = 0;
    if (CVAR_FREEZE_TIME_VALUE) {
        hookId = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnGameFrameUpdate>(OnGameFrameUpdateFreezeTime);
    } else {
        CVarClear(CVAR_PREV_TIME_NAME);
    }
}

static RegisterShipInitFunc initFunc(RegisterFreezeTime, { CVAR_FREEZE_TIME_NAME });
