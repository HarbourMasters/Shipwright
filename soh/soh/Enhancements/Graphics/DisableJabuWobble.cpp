#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"
#include "soh/cvar_prefixes.h"

void RegisterDisableJabuWobble() {
    COND_VB_SHOULD(VB_JABU_WOBBLE, CVarGetInteger(CVAR_SETTING("A11yNoJabuWobble"), 0), { *should = false; });
}

static RegisterShipInitFunc initFunc(RegisterDisableJabuWobble, { CVAR_SETTING("A11yNoJabuWobble") });
