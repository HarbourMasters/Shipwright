#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "functions.h"
extern SaveContext gSaveContext;
}

void RegisterSkipAmyPuzzle() {
    COND_VB_SHOULD(VB_AMY_SOLVE, CVarGetInteger(CVAR_ENHANCEMENT("SkipAmyPuzzle"), 0),
                   { *should = !IS_RANDO || Flags_GetRandomizerInf(RAND_INF_CAN_GRAB); });
}

static RegisterShipInitFunc initFunc(RegisterSkipAmyPuzzle, { CVAR_ENHANCEMENT("SkipAmyPuzzle") });