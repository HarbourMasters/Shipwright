#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "variables.h"
}

#define CVAR_NAME "gEnhancements.Fixes.CompletedHeartContainerAudio"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterCompletedHeartContainerAudio() {
    COND_VB_SHOULD(VB_PLAY_HEART_CONTAINER_GET_FANFARE, CVAR, {
        GetItemId getItemId = (GetItemId)va_arg(args, int);
        if (getItemId == GI_HEART_PIECE && GET_QUEST_HEART_PIECE_COUNT == 0) {
            *should = true;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterCompletedHeartContainerAudio, { CVAR_NAME });
