#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"

#define CVAR_NO_HIDDEN_GROTTOS_NAME CVAR_CHEAT("NoHiddenGrottos")
#define CVAR_NO_HIDDEN_GROTTOS_VALUE CVarGetInteger(CVAR_NO_HIDDEN_GROTTOS_NAME, 0)

void RegisterNoHiddenGrottos() {
    COND_VB_SHOULD(VB_GROTTO_BE_HIDDEN, CVAR_NO_HIDDEN_GROTTOS_VALUE, { *should = false; });
}

static RegisterShipInitFunc initFunc(RegisterNoHiddenGrottos, { CVAR_NO_HIDDEN_GROTTOS_NAME });