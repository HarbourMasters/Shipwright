#include <libultraship/bridge.h>
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"

#define CVAR_TARGET_BEHIND_NAME CVAR_ENHANCEMENT("TargetBehind")
#define CVAR_TARGET_BEHIND_VALUE CVarGetInteger(CVAR_TARGET_BEHIND_NAME, 0)

void RegisterTargetBehind() {
    // Disable the FOV cone limit if the enhancement is on
    COND_VB_SHOULD(VB_LIMIT_TARGET_FOV, CVAR_TARGET_BEHIND_VALUE, {
        *should = false;
    });

    // Apply the angle-based scoring to targets behind Link if the enhancement is on
    COND_VB_SHOULD(VB_APPLY_TARGET_SCORING, CVAR_TARGET_BEHIND_VALUE, {
        *should = true;
    });
}

static RegisterShipInitFunc initFunc(RegisterTargetBehind, { CVAR_TARGET_BEHIND_NAME });