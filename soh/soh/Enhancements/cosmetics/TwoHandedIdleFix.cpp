#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "variables.h"
#include "soh/cvar_prefixes.h"
}

static void RegisterFidgetIdleOverrides() {
    REGISTER_VB_SHOULD(VB_TWO_HANDED_FIDGET_IDLE, {
        // Only allow the two-handed fidget when the toggle is enabled; otherwise keep vanilla behavior.
        if (*should) {
            *should = CVarGetInteger(CVAR_ENHANCEMENT("TwoHandedIdle"), 0) != 0;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterFidgetIdleOverrides, { CVAR_ENHANCEMENT("TwoHandedIdle") });
