#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"

void RegisterFasterBeanSkulltula() {
    COND_VB_SHOULD(VB_SPAWN_BEAN_SKULLTULA, CVarGetInteger(CVAR_ENHANCEMENT("FasterBeanSkull"), 0),
                   { *should = true; });
}

static RegisterShipInitFunc initFunc(RegisterFasterBeanSkulltula, { CVAR_ENHANCEMENT("FasterBeanSkull") });
