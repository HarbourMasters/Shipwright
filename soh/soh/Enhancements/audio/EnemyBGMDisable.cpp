#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"

#define CVAR_ENEMYBGMDISABLE_NAME CVAR_AUDIO("EnemyBGMDisable")
#define CVAR_ENEMYBGMDISABLE_VALUE CVarGetInteger(CVAR_ENEMYBGMDISABLE_NAME, 0)

static void RegisterEnemyBGMDisable() {
    COND_VB_SHOULD(VB_DETECT_BGM_ENEMY, CVAR_ENEMYBGMDISABLE_VALUE, { *should = false; });
}

static RegisterShipInitFunc initFunc_EnemyBGMDisable(RegisterEnemyBGMDisable, { CVAR_ENEMYBGMDISABLE_NAME });
