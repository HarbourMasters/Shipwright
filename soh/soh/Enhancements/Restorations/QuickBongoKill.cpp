#include <libultraship/bridge/consolevariablebridge.h>

#include "soh/cvar_prefixes.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"

static constexpr const char* CVAR_QUICK_BONGO_KILL = CVAR_ENHANCEMENT("QuickBongoKill");

static void RegisterQuickBongoKill() {
    COND_VB_SHOULD(VB_ALLOW_QUICK_BONGO_KILL, CVarGetInteger(CVAR_QUICK_BONGO_KILL, 0), { *should = true; });
}

static RegisterShipInitFunc initFunc(RegisterQuickBongoKill, { CVAR_QUICK_BONGO_KILL });
