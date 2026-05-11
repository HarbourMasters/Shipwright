#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"

void RegisterDisableKokiriDrawDistance() {
    COND_VB_SHOULD(VB_FADE_KOKIRI, CVarGetInteger(CVAR_ENHANCEMENT("DisableKokiriDrawDistance"), 0),
                   { *should = false; });
}

static RegisterShipInitFunc initFunc(RegisterDisableKokiriDrawDistance,
                                     { CVAR_ENHANCEMENT("DisableKokiriDrawDistance") });
