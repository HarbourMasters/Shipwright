#include <libultraship/bridge.h>
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"

#define CVAR_KNOCKDOWN_RECOVERY_NAME CVAR_ENHANCEMENT("KnockdownRecovery")
#define CVAR_KNOCKDOWN_RECOVERY_VALUE CVarGetInteger(CVAR_KNOCKDOWN_RECOVERY_NAME, 0)

// Tell C++ that these old N64 functions exist in the codebase!
extern "C" {
    void func_8083BCD0(Player* thisx, PlayState* play, s32 controlStickDirection);
    void Player_SetupRoll(Player* thisx, PlayState* play);
}

void RegisterKnockdownRecovery() {
    COND_VB_SHOULD(VB_PERFORM_KNOCKDOWN_RECOVERY, CVAR_KNOCKDOWN_RECOVERY_VALUE, {
        Player* player = va_arg(args, Player*);
        PlayState* play = va_arg(args, PlayState*);

        if (CHECK_BTN_ALL(play->state.input[0].press.button, BTN_A)) {
            if (player->yaw != player->actor.shape.rot.y) {
                func_8083BCD0(player, play, PLAYER_STICK_DIR_BACKWARD);
            } else {
                Player_SetupRoll(player, play);
            }
            
            *should = true; 
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterKnockdownRecovery, { CVAR_KNOCKDOWN_RECOVERY_NAME });