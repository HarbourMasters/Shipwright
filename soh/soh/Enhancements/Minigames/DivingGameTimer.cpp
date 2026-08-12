#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "functions.h"
extern GameInfo* gGameInfo;
}

static constexpr int32_t CVAR_DIVING_GAME_TIME_DEFAULT = 50;
#define CVAR_DIVING_GAME_TIME_NAME CVAR_ENHANCEMENT("DivingGame.TimeLimit")
#define CVAR_DIVING_GAME_TIME_VALUE CVarGetInteger(CVAR_DIVING_GAME_TIME_NAME, CVAR_DIVING_GAME_TIME_DEFAULT)
#define CVAR_DIVING_GAME_TIME_SET (CVAR_DIVING_GAME_TIME_VALUE != CVAR_DIVING_GAME_TIME_DEFAULT)

static void RegisterDIvingGameTimeLimit() {
    COND_VB_SHOULD(VB_SET_DIVING_GAME_TIME_LIMIT, CVAR_DIVING_GAME_TIME_SET, {
        Interface_SetTimer(BREG(2) + CVAR_DIVING_GAME_TIME_VALUE);
        *should = false;
    });
}

static RegisterShipInitFunc initFunc(RegisterDIvingGameTimeLimit, { CVAR_DIVING_GAME_TIME_NAME });
