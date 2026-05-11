#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"
#include "global.h"

extern "C" {
void Player_SetupRoll(Player* player, PlayState* play);
}

#define CVAR_ROLL_CHAIN CVAR_ENHANCEMENT("ImprovedRoll")
#define CVAR_ROLL_STEER CVAR_ENHANCEMENT("ImprovedRollSteering")

void ImprovedRoll_Register() {
    COND_VB_SHOULD(VB_PLAYER_ROLL_CHAIN, CVarGetInteger(CVAR_ROLL_CHAIN, 0), {
        Player* player = va_arg(args, Player*);
        PlayState* play = va_arg(args, PlayState*);
        Input* controlInput = va_arg(args, Input*);
        s32 floorType = va_arg(args, s32);
        if ((player->skelAnime.curFrame >= 15.0f) && CHECK_BTN_ALL(controlInput->press.button, BTN_A) &&
            (floorType != 7)) {
            Player_SetupRoll(player, play);
            *should = true;
        }
    });

    COND_VB_SHOULD(VB_PLAYER_ROLL_STEER, CVarGetInteger(CVAR_ROLL_CHAIN, 0) && CVarGetInteger(CVAR_ROLL_STEER, 0), {
        Player* player = va_arg(args, Player*);
        PlayState* play = va_arg(args, PlayState*);
        s16 yawTarget = (s16)va_arg(args, int);
        if (!CHECK_BTN_ALL(play->state.input[0].cur.button, BTN_Z)) {
            Math_ScaledStepToS(&player->actor.shape.rot.y, yawTarget, 0x200);
        }
        *should = false;
    });
}

static RegisterShipInitFunc initFunc(ImprovedRoll_Register, { CVAR_ROLL_CHAIN, CVAR_ROLL_STEER });
