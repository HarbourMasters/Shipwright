#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "z64play.h"
extern PlayState* gPlayState;
}

#define CVAR_NAME "gEnhancements.A11y.NoScreenDistortion"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

static void RegisterDisableScreenDistortion() {
    COND_VB_SHOULD(VB_APPLY_SCREEN_DISTORTION, CVAR, {
        View_ClearDistortion(&gPlayState->view);
        *should = false;
    });
}

static RegisterShipInitFunc initFunc(RegisterDisableScreenDistortion, { CVAR_NAME });
