#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"

static void RegisterClimbEverything() {
    COND_VB_SHOULD(VB_SURFACE_IS_CLIMBABLE, CVarGetInteger(CVAR_CHEAT("ClimbEverything"), 0), { *should = true; });
    COND_VB_SHOULD(VB_SURFACE_ANGLE_IS_CLIMBABLE, CVarGetInteger(CVAR_CHEAT("ClimbEverything"), 0),
                   { *should = true; });
}

static RegisterShipInitFunc initFunc(RegisterClimbEverything, { CVAR_CHEAT("ClimbEverything") });
