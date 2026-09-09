#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"
#include "soh/cvar_prefixes.h"

static constexpr int32_t CVAR_ENEMYBGMDISABLE_DEFAULT = 0;
#define CVAR_ENEMYBGMDISABLE_NAME CVAR_AUDIO("EnemyBGMDisable")
#define CVAR_ENEMYBGMDISABLE_VALUE CVarGetInteger(CVAR_ENEMYBGMDISABLE_NAME, CVAR_ENEMYBGMDISABLE_DEFAULT)

static void RegisterEnemyBGMDisable() {
    COND_VB_SHOULD(VB_DETECT_BGM_ENEMY, CVAR_ENEMYBGMDISABLE_VALUE, { *should = false; });
}

static RegisterShipInitFunc initFunc(RegisterEnemyBGMDisable, { CVAR_ENEMYBGMDISABLE_NAME });
