#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"

void RegisterSkipAmyPuzzle() {
    COND_VB_SHOULD(VB_AMY_SOLVE, CVarGetInteger(CVAR_ENHANCEMENT("SkipAmyPuzzle"), 0), { *should = true; });
}

static RegisterShipInitFunc initFunc(RegisterSkipAmyPuzzle, { CVAR_ENHANCEMENT("SkipAmyPuzzle") });