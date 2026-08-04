#include <libultraship/bridge.h>
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "macros.h"
}

static constexpr int32_t CVAR_NOKEESEGUAYTARGET_DEFAULT = 0;
#define CVAR_NOKEESEGUAYTARGET_NAME CVAR_CHEAT("NoKeeseGuayTarget")
#define CVAR_NOKEESEGUAYTARGET_VALUE CVarGetInteger(CVAR_NOKEESEGUAYTARGET_NAME, CVAR_NOKEESEGUAYTARGET_DEFAULT)

void RegisterNoKeeseGuayTarget() {
    // Dive Attack
    COND_VB_SHOULD(VB_KEESE_DO_DIVE_ATTACK, CVAR_NOKEESEGUAYTARGET_VALUE, { *should = false; });
    COND_VB_SHOULD(VB_GUAY_DO_DIVE_ATTACK, CVAR_NOKEESEGUAYTARGET_VALUE, { *should = false; });

    // Force Fly Away
    COND_VB_SHOULD(VB_KEESE_FORCE_FLY_AWAY, CVAR_NOKEESEGUAYTARGET_VALUE, { *should = true; });
    COND_VB_SHOULD(VB_GUAY_FORCE_FLY_AWAY, CVAR_NOKEESEGUAYTARGET_VALUE, { *should = true; });
}

static RegisterShipInitFunc initFunc(RegisterNoKeeseGuayTarget, { CVAR_NOKEESEGUAYTARGET_NAME });
