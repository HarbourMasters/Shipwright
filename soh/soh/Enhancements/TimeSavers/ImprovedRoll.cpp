#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"
#include "global.h"

extern "C" {
extern PlayState* gPlayState;
}

#define CVAR_ROLL_CHAIN_NAME CVAR_ENHANCEMENT("ImprovedRoll")
#define CVAR_ROLL_CHAIN_VALUE CVarGetInteger(CVAR_ROLL_CHAIN_NAME, 0)

#define CVAR_ROLL_STEER_NAME CVAR_ENHANCEMENT("ImprovedRollSteering")
#define CVAR_ROLL_STEER_VALUE CVarGetInteger(CVAR_ROLL_STEER_NAME, 0)

void ImprovedRoll_Register() {
    COND_VB_SHOULD(VB_PLAYER_ROLL_CHAIN, CVAR_ROLL_CHAIN_VALUE, { *should = true; });

    COND_VB_SHOULD(VB_PLAYER_ROLL_STEER, CVAR_ROLL_CHAIN_VALUE && CVAR_ROLL_STEER_VALUE, {
        Player* player = va_arg(args, Player*);
        va_arg(args, PlayState*);
        s16 yawTarget = (s16)va_arg(args, int);
        Math_ScaledStepToS(&player->actor.shape.rot.y, yawTarget, 0x200);
        *should = false;
    });
}

static RegisterShipInitFunc initFunc(ImprovedRoll_Register, { CVAR_ROLL_CHAIN_NAME, CVAR_ROLL_STEER_NAME });
