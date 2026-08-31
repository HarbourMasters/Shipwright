#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"
#include "soh/cvar_prefixes.h"

extern "C" {
// Converts a drop id based on age, health and owned items. Returns -1 when the drop can't be given.
s16 func_8001F404(s16 dropId);
}

static void RegisterFixFlexDrops() {
    // The flex drop picks the first thing you're low on, but doesn't check that you own the item to hold it,
    // so the drop is thrown away instead of falling through to the next candidate.
    COND_VB_SHOULD(VB_FLEX_DROP_AMMO, CVarGetInteger(CVAR_ENHANCEMENT("FixFlexDrops"), 0), {
        s16 dropId = va_arg(args, s32);
        if (*should && func_8001F404(dropId) == -1) {
            *should = false;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterFixFlexDrops, { CVAR_ENHANCEMENT("FixFlexDrops") });
