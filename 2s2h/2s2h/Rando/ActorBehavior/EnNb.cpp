#include "ActorBehavior.h"

extern "C" {
#include "variables.h"
void Player_SetupTalk(PlayState* play, Player* player);
s32 Player_SetupWaitForPutAway(PlayState* play, Player* player, AfterPutAwayFunc afterPutAwayFunc);
}

void Rando::ActorBehavior::InitEnNbBehavior() {
    COND_VB_SHOULD(VB_EXEC_MSG_EVENT, IS_RANDO, {
        u32 cmdId = va_arg(args, u32);
        Actor* actor = va_arg(args, Actor*);
        if (actor->id == ACTOR_EN_NB) { // Anju's Grandmother
            MsgScript* script = va_arg(args, MsgScript*);
            Player* player = GET_PLAYER(gPlayState);
            switch (cmdId) {
                case MSCRIPT_CMD_ID_OFFER_ITEM:
                    // Lock the player into conversation because a notebook message might appear
                    Player_SetupWaitForPutAway(gPlayState, player, Player_SetupTalk);
                    *should = false;
                    break;
                case MSCRIPT_CMD_ID_DONE:
                    // Prevent softlocks in case a notebook message did not appear
                    Message_CloseTextbox(gPlayState);
                    break;
                default:
                    break;
            }
        }
    });
}
