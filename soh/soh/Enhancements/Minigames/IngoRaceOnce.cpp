#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "functions.h"
}

#define CVAR_INGO_RACE_ONCE_NAME CVAR_ENHANCEMENT("IngoRaceOnce")
#define CVAR_INGO_RACE_ONCE_VALUE CVarGetInteger(CVAR_INGO_RACE_ONCE_NAME, 0)

static void RegisterIngoRaceOnce() {
    COND_VB_SHOULD(VB_LINK_WIN_EPONA, CVAR_INGO_RACE_ONCE_NAME, { *should = true; });
}

static RegisterShipInitFunc initFunc(RegisterIngoRaceOnce, { CVAR_INGO_RACE_ONCE_NAME });
