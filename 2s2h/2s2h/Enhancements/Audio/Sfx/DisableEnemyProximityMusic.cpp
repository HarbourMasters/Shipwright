#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

#define CVAR_NAME "gAudioEditor.EnemyBGMDisable"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterDisableEnemyProximityMusic() {
    COND_VB_SHOULD(VB_PLAY_ENEMY_PROXIMITY_MUSIC, CVAR, { *should = false; });
}

static RegisterShipInitFunc initFunc(RegisterDisableEnemyProximityMusic, { CVAR_NAME });
