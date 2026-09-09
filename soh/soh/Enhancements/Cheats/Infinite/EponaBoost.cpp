#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"
#include "soh/cvar_prefixes.h"

static void RegisterInfiniteEponaBoost() {
    COND_VB_SHOULD(VB_CONSUME_EPONA_BOOST, CVarGetInteger(CVAR_CHEAT("InfiniteEponaBoost"), 0), { *should = false; });
    COND_VB_SHOULD(VB_DRAW_EPONA_BOOST_CARROTS, CVarGetInteger(CVAR_CHEAT("InfiniteEponaBoost"), 0),
                   { *should = false; });
}

static RegisterShipInitFunc initFunc(RegisterInfiniteEponaBoost, { CVAR_CHEAT("InfiniteEponaBoost") });
