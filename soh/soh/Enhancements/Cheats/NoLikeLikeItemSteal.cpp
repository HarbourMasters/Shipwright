#include <libultraship/bridge.h>
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "macros.h"
}

static constexpr int32_t CVAR_NOLIKELIKEITEMSTEAL_DEFAULT = 0;
#define CVAR_NOLIKELIKEITEMSTEAL_NAME CVAR_CHEAT("NoLikeLikeItemSteal")
#define CVAR_NOLIKELIKEITEMSTEAL_VALUE CVarGetInteger(CVAR_NOLIKELIKEITEMSTEAL_NAME, CVAR_NOLIKELIKEITEMSTEAL_DEFAULT)

void RegisterNoLikeLikeItemSteal() {
    COND_VB_SHOULD(VB_LIKE_LIKE_STEAL_EQUIPMENT, CVAR_NOLIKELIKEITEMSTEAL_VALUE, { *should = false; });
}

static RegisterShipInitFunc initFunc(RegisterNoLikeLikeItemSteal, { CVAR_NOLIKELIKEITEMSTEAL_NAME });
