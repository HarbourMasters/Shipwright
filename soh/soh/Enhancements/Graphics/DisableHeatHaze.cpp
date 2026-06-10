#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"

void RegisterDisableHeatHaze() {
    COND_VB_SHOULD(VB_HOT_ROOM_DISTORTION, CVarGetInteger(CVAR_SETTING("A11yNoHeatHaze"), 0), { *should = false; });
}

static RegisterShipInitFunc initFunc(RegisterDisableHeatHaze, { CVAR_SETTING("A11yNoHeatHaze") });
