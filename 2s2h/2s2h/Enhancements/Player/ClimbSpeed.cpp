#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

#define CVAR_NAME "gEnhancements.Player.ClimbSpeed"
#define CVAR CVarGetInteger(CVAR_NAME, 1)

void RegisterClimbSpeed() {
    COND_VB_SHOULD(VB_SET_CLIMB_SPEED, CVAR > 1, {
        f32* speed = va_arg(args, f32*);
        *speed *= CVAR;
    });
}

static RegisterShipInitFunc initFunc(RegisterClimbSpeed, { CVAR_NAME });
