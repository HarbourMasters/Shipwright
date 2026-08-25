#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "functions.h"
#include "macros.h"
}

#define CVAR_DAMPE_BACKWARDS_NAME CVAR_ENHANCEMENT("FixDampeGoingBackwards")
#define CVAR_DAMPE_BACKWARDS_VALUE CVarGetInteger(CVAR_DAMPE_BACKWARDS_NAME, 0)

static void RegisterDampeGoingBackwardsFix() {
    COND_VB_SHOULD(VB_DAMPE_GO_BACKWARDS, CVAR_DAMPE_BACKWARDS_VALUE, {
        f32* speed = va_arg(args, f32*);
        *speed = ABS(*speed);
    });
}

static RegisterShipInitFunc initFunc(RegisterDampeGoingBackwardsFix, { CVAR_DAMPE_BACKWARDS_NAME });
