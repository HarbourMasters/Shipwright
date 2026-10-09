#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

#define CVAR_NAME "gAudioEditor.LowHpAlarm"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterMuteLowHpAlarm() {
    COND_VB_SHOULD(VB_PLAY_LOW_HP_ALARM, CVAR, { *should = false; });
}

static RegisterShipInitFunc initFunc(RegisterMuteLowHpAlarm, { CVAR_NAME });
