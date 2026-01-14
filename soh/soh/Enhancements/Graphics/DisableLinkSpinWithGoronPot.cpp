#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"

static constexpr int32_t CVAR_LINK_SPIN_DISABLE_DEFAULT = 0;
#define CVAR_LINK_SPIN_DISABLE_NAME CVAR_ENHANCEMENT("DisableLinkSpinWithGoronPot")
#define CVAR_LINK_SPIN_DISABLE_VALUE CVarGetInteger(CVAR_LINK_SPIN_DISABLE_NAME, CVAR_LINK_SPIN_DISABLE_DEFAULT)

static void RegisterDisableLinkSpinWithGoronPot() {
    COND_VB_SHOULD(VB_LINK_SPIN_WITH_GORON_POT, CVAR_LINK_SPIN_DISABLE_VALUE, { *should = false; });
}

static RegisterShipInitFunc initFunc(RegisterDisableLinkSpinWithGoronPot, { CVAR_LINK_SPIN_DISABLE_NAME });
