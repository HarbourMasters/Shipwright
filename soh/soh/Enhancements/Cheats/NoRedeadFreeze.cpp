#include <libultraship/bridge.h>
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "macros.h"
}

static constexpr int32_t CVAR_NOREDEADFREEZE_DEFAULT = 0;
#define CVAR_NOREDEADFREEZE_NAME CVAR_CHEAT("NoRedeadFreeze")
#define CVAR_NOREDEADFREEZE_VALUE CVarGetInteger(CVAR_NOREDEADFREEZE_NAME, CVAR_NOREDEADFREEZE_DEFAULT)

void RegisterNoRedeadFreeze() {
    COND_VB_SHOULD(VB_REDEAD_GIBDO_FREEZE_LINK, CVAR_NOREDEADFREEZE_VALUE, { *should = false; });
}

static RegisterShipInitFunc initFunc_NoRedeadFreeze(RegisterNoRedeadFreeze, { CVAR_NOREDEADFREEZE_NAME });
