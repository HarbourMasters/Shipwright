#include <libultraship/bridge/consolevariablebridge.h>

#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"
#include "soh/cvar_prefixes.h"

#include "z64.h"

extern "C" {
    #include "macros.h"
    extern PlayState* gPlayState;
    extern void Player_StartMode_Idle(PlayState*, Player*);
}

#define CVAR_NAME CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.EntranceTransitions")
#define CVAR_VALUE CVarGetInteger(CVAR_NAME, false)

void RegisterSkipEntranceTransitions() {
    COND_VB_SHOULD(VB_SET_TRANSITION_TYPE, CVAR_VALUE, {
        *should = false;
        PlayState* play = va_arg(args, PlayState*);
        play->transitionType = TRANS_TYPE_INSTANT;
    });

    COND_VB_SHOULD(VB_EXECUTE_PLAYER_STARTMODE_FUNC, CVAR_VALUE, {
        s32 startMode = va_arg(args, s32);
        if (startMode == PLAYER_START_MODE_IDLE || startMode == PLAYER_START_MODE_MOVE_FORWARD || startMode == PLAYER_START_MODE_MOVE_FORWARD_SLOW) {
            *should = false;
            Player* player = GET_PLAYER(gPlayState);
            Player_StartMode_Idle(gPlayState, player);
            player->av2.actionVar2 = -1;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterSkipEntranceTransitions, { CVAR_NAME });
