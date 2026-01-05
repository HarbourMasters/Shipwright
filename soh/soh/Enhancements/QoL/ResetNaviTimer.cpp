#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "variables.h"
extern SaveContext gSaveContext;
}

static constexpr int32_t CVAR_RESET_NAVI_DEFAULT = 0;
#define CVAR_RESET_NAVI_NAME CVAR_ENHANCEMENT("ResetNaviTimer")
#define CVAR_RESET_NAVI_VALUE CVarGetInteger(CVAR_RESET_NAVI_NAME, CVAR_RESET_NAVI_DEFAULT)

static void RegisterResetNaviTimer() {
    COND_HOOK(OnSceneInit, CVAR_RESET_NAVI_VALUE, [](int32_t) { gSaveContext.naviTimer = 0; });
}

static RegisterShipInitFunc initFunc(RegisterResetNaviTimer, { CVAR_RESET_NAVI_NAME });
