#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

#define CVAR_NAME "gAudioEditor.ChildGoronCry"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterMuteCryingGoronChild() {
    COND_VB_SHOULD(VB_PLAY_GORON_CHILD_CRY, CVAR, { *should = false; });
}

static RegisterShipInitFunc initFunc(RegisterMuteCryingGoronChild, { CVAR_NAME });
