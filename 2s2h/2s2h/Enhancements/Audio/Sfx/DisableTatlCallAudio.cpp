#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

#define CVAR_NAME "gAudioEditor.DisableTatlCallAudio"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterDisableTatlCallAudio() {
    COND_VB_SHOULD(VB_PLAY_TATL_CALL_AUDIO, CVAR, { *should = false; });
}

static RegisterShipInitFunc initFunc(RegisterDisableTatlCallAudio, { CVAR_NAME });
