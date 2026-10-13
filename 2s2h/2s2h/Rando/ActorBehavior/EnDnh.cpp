#include "ActorBehavior.h"
#include <libultraship/bridge/consolevariablebridge.h>

extern "C" {
#include "variables.h"
#include "overlays/actors/ovl_En_Dnh/z_en_dnh.h"

void Player_StartTalking(PlayState* play, Actor* actor);
void Player_SetupTalk(PlayState* play, Player* player);
s32 Player_SetupWaitForPutAway(PlayState* play, Player* player, AfterPutAwayFunc afterPutAwayFunc);
}

void Rando::ActorBehavior::InitEnDnhBehavior() {
    // Scripted Actors
    COND_VB_SHOULD(VB_EXEC_MSG_EVENT, IS_RANDO, {
        u32 cmdId = va_arg(args, u32);
        Actor* actor = va_arg(args, Actor*);
        MsgScript* script = va_arg(args, MsgScript*);
        Player* player = GET_PLAYER(gPlayState);
        static std::vector<u8> skipCmds = {};

        if (actor->id != ACTOR_EN_DNH) {
            return;
        }

        if (cmdId == MSCRIPT_CMD_ID_CHECK_ITEM) {
            *should = false;
            if (!RANDO_SAVE_CHECKS[RC_TOURIST_INFORMATION_PICTOBOX].cycleObtained) {
                return;
            } else {
                skipCmds.clear();
                skipCmds.push_back(MSCRIPT_CMD_ID_OFFER_ITEM);
                skipCmds.push_back(MSCRIPT_CMD_ID_AUTOTALK);
                skipCmds.push_back(MSCRIPT_CMD_ID_AWAIT_TEXT);
            }
        }

        if (cmdId == MSCRIPT_CMD_ID_OFFER_ITEM) {
            Player_SetupWaitForPutAway(gPlayState, player, Player_SetupTalk);
            *should = false;
            skipCmds.clear();
            skipCmds.push_back(MSCRIPT_CMD_ID_AUTOTALK);
            skipCmds.push_back(MSCRIPT_CMD_ID_AWAIT_TEXT);
            return;
        }

        if (skipCmds.empty()) {
            return;
        }

        if (cmdId == skipCmds[0]) {
            skipCmds.erase(skipCmds.begin());
            *should = false;
        }
    });
}
