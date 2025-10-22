#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "functions.h"
#include "macros.h"
}

static constexpr int32_t CVAR_PAUSE_WARP_DEFAULT = 0;
#define CVAR_PAUSE_WARP_NAME CVAR_ENHANCEMENT("PauseWarp")
#define CVAR_PAUSE_WARP_VALUE CVarGetInteger(CVAR_PAUSE_WARP_NAME, CVAR_PAUSE_WARP_DEFAULT)

void RegisterPauseMenuHooks() {
    COND_HOOK(OnKaleidoUpdate, CVAR_PAUSE_WARP_VALUE, [] {
        if (GameInteractor::IsSaveLoaded())
            PauseWarp_HandleSelection();
    });
    COND_HOOK(OnGameFrameUpdate, CVAR_PAUSE_WARP_VALUE, [] {
        if (GameInteractor::IsSaveLoaded())
            PauseWarp_Execute();
    });
}

static RegisterShipInitFunc initFunc(RegisterPauseMenuHooks, { CVAR_PAUSE_WARP_NAME });
