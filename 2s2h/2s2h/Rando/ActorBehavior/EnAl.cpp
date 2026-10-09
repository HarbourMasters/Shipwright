#include "ActorBehavior.h"
#include <libultraship/bridge/consolevariablebridge.h>

extern "C" {
#include "variables.h"
void Player_SetupTalk(PlayState* play, Player* player);
s32 Player_SetupWaitForPutAway(PlayState* play, Player* player, AfterPutAwayFunc afterPutAwayFunc);
void func_80848250(PlayState* play, Player* player);
void Player_StartTalking(PlayState* play, Actor* actor);
}

static std::vector<u8> skipCmds = {};

void Rando::ActorBehavior::InitEnAlBehavior() {

    COND_ID_HOOK(OnActorInit, ACTOR_EN_AL, IS_RANDO, [](Actor* actor) { skipCmds.clear(); });

    COND_VB_SHOULD(VB_MADAME_AROMA_ASK_FOR_HELP, IS_RANDO,
                   { *should = !RANDO_SAVE_CHECKS[RC_MAYORS_OFFICE_KAFEIS_MASK].cycleObtained; });
    // "I'm counting on you"
    COND_ID_HOOK(OnOpenText, 0x2AA2, IS_RANDO, [](u16* textId, bool* loadFromMessageTable) {
        Message_BombersNotebookQueueEvent(gPlayState, BOMBERS_NOTEBOOK_EVENT_MET_MADAME_AROMA);
        Message_BombersNotebookQueueEvent(gPlayState, BOMBERS_NOTEBOOK_EVENT_RECEIVED_KAFEIS_MASK);
    });

    COND_VB_SHOULD(VB_EXEC_MSG_EVENT, IS_RANDO, {
        u32 cmdId = va_arg(args, u32);
        Actor* actor = va_arg(args, Actor*);

        if (actor->id == ACTOR_EN_AL) { // Madame Aroma
            Player* player = GET_PLAYER(gPlayState);

            if (cmdId == MSCRIPT_CMD_ID_OFFER_ITEM) {
                *should = false;
                MsgScriptCmdOfferItem* cmd = va_arg(args, MsgScriptCmdOfferItem*);
                GetItemId getItemId = (GetItemId)SCRIPT_PACK_16(cmd->itemIdH, cmd->itemIdL);
                skipCmds.clear();
                if (getItemId == GI_MASK_KAFEIS_MASK) { // Mayor's Residence
                    // There is no usable flag for this check, so grant it manually
                    RANDO_SAVE_CHECKS[RC_MAYORS_OFFICE_KAFEIS_MASK].eligible = true;
                } else { // Express Mail reward
                    /*
                     * We do something a little tricky here. We manually open a textbox with the message that normally
                     * plays after the player receives the reward (0x2B20), then also skip the MsgScript commands to
                     * open that textbox and wait on it. More naive attempts at handling this actor case resulted in
                     * softlocks, not appropriately locking textboxes, duplicate textboxes, or Bombers' Notebook
                     * messages being eaten. The method below handles the intended behavior, both with or without
                     * notebook messages, even if it is a little counterintuitive.
                     */
                    Message_StartTextbox(gPlayState, 0x2B20, actor);
                    Player_StartTalking(gPlayState, actor);
                    func_80848250(gPlayState, player); // End the giveItem animation, or the Express Mail will persist
                    skipCmds.push_back(MSCRIPT_CMD_ID_BEGIN_TEXT); // The scripted text at textId 0x2B20
                    skipCmds.push_back(MSCRIPT_CMD_ID_AWAIT_TEXT);
                }
                return;
            }

            if (skipCmds.empty()) {
                return;
            }

            if (cmdId == skipCmds[0]) {
                skipCmds.erase(skipCmds.begin());
                *should = false;
            }
        }
    });
}
