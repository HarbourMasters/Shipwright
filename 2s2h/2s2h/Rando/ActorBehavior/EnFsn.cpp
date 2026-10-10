#include "ActorBehavior.h"
#include <libultraship/bridge/consolevariablebridge.h>

extern "C" {
#include "variables.h"
#include "overlays/actors/ovl_En_Fsn/z_en_fsn.h"
#include "overlays/actors/ovl_En_GirlA/z_en_girla.h"
void Player_StartTalking(PlayState* play, Actor* actor);
void EnFsn_ResumeInteraction(EnFsn* enFsn, PlayState* play);
}

#define ENFSN_END_CONVERSATION (1 << 0)
#define ENFSN_GAVE_KEATONS_MASK (1 << 2)
#define ENFSN_GAVE_LETTER_TO_MAMA (1 << 3)

void EndEnFsnDialogue(EnFsn* actor) {
    Player* player = GET_PLAYER(gPlayState);

    player->talkActor = &actor->actor;
    player->talkActorDistance = actor->actor.xzDistToPlayer;
    player->exchangeItemAction = PLAYER_IA_MINUS1;
    Player_StartTalking(gPlayState, &actor->actor);
    actor->flags |= ACTOR_FLAG_TALK;
    actor->actionFunc = EnFsn_ResumeInteraction;
}

void Rando::ActorBehavior::InitEnFsnBehavior() {
    COND_VB_SHOULD(VB_GIVE_ITEM_FROM_OFFER, IS_RANDO, {
        GetItemId* item = va_arg(args, GetItemId*);
        Actor* actor = va_arg(args, Actor*);
        Player* player = GET_PLAYER(gPlayState);

        if (actor->id == ACTOR_EN_FSN) { // Curiosity Shop owner
            EnFsn* enFsn = (EnFsn*)actor;
            if (enFsn->getItemId == GI_MASK_KEATON || enFsn->getItemId == GI_LETTER_TO_MAMA) {
                *should = false;
                EndEnFsnDialogue(enFsn);
                return;
            }
            // Handling for when the Curiosity Shop owner sells something to the player
            if (enFsn->isSelling && enFsn->cursorIndex >= 0 && enFsn->cursorIndex <= 2) {
                EnGirlA* enGirlA = enFsn->items[enFsn->cursorIndex];
                RandoCheckId randoCheckId = (RandoCheckId)enGirlA->actor.world.rot.z;
                // Only handle the two special checks. Leave stolen items as-is.
                if (randoCheckId == RC_CURIOSITY_SHOP_SPECIAL_ITEM ||
                    randoCheckId == RC_BOMB_SHOP_ITEM_04_OR_CURIOSITY_SHOP_ITEM) {
                    *should = false;
                    EndEnFsnDialogue(enFsn);
                    enGirlA->buyFunc(gPlayState, enGirlA);
                    /*
                     * This notebook event must be faked because the randomized item probably won't be the All-Night
                     * Mask. There exists a minor bug where, if the player tries to sell something immediately after
                     * buying the special item, this notebook event will pop as Link pulls out the item to show. The
                     * Curiosity Shop owner's response will then erase that textbox. Not game breaking, but something to
                     * note.
                     */
                    if (randoCheckId == RC_CURIOSITY_SHOP_SPECIAL_ITEM) {
                        Message_BombersNotebookQueueEvent(gPlayState, BOMBERS_NOTEBOOK_EVENT_RECEIVED_ALL_NIGHT_MASK);
                    }
                }
            }
        }
    });

    COND_VB_SHOULD(VB_GIVE_KEATON_MASK, IS_RANDO, {
        EnFsn* enFsn = va_arg(args, EnFsn*);
        RANDO_SAVE_CHECKS[RC_KAFEIS_HIDEOUT_KEATON_MASK].eligible = true;
        enFsn->flags |= ENFSN_GAVE_KEATONS_MASK;
        enFsn->textId = 0x29E2;
        EndEnFsnDialogue(enFsn);
        *should = false;
    });

    COND_VB_SHOULD(VB_GIVE_LETTER_TO_MAMA, IS_RANDO, {
        EnFsn* enFsn = va_arg(args, EnFsn*);
        RANDO_SAVE_CHECKS[RC_KAFEIS_HIDEOUT_LETTER_TO_MAMA].eligible = true;
        enFsn->flags |= ENFSN_END_CONVERSATION;
        enFsn->flags |= ENFSN_GAVE_LETTER_TO_MAMA;
        enFsn->textId = 0x29E4;
        enFsn->actionFunc = EnFsn_ResumeInteraction;
        SET_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_PRIORITY_MAIL);
        *should = false;
    });
}
