#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"
#include "2s2h/Rando/Rando.h"

#define CVAR_NAME "gCheats.UnbreakableRazorSword"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterUnbreakableRazorSword() {
    COND_VB_SHOULD(VB_LOWER_RAZOR_SWORD_DURABILITY, CVAR || IS_RANDO, { *should = false; });
}

static RegisterShipInitFunc initFunc(RegisterUnbreakableRazorSword, { CVAR_NAME, "IS_RANDO" });
