#include <libultraship/bridge/consolevariablebridge.h>

#include "soh/cvar_prefixes.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"

static constexpr const char* CVAR_QUICK_PUTAWAY = CVAR_ENHANCEMENT("QuickPutaway");

static void RegisterQuickPutaway() {
    COND_VB_SHOULD(VB_ALLOW_QUICK_PUTAWAY, CVarGetInteger(CVAR_QUICK_PUTAWAY, 0), { *should = true; });
}

static RegisterShipInitFunc initFunc(RegisterQuickPutaway, { CVAR_QUICK_PUTAWAY });
