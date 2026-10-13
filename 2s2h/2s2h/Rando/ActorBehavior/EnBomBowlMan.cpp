#include "ActorBehavior.h"
#include <libultraship/bridge/consolevariablebridge.h>

extern "C" {
#include "variables.h"

#include "overlays/actors/ovl_En_Bom_Bowl_Man/z_en_bom_bowl_man.h"
void Player_StartTalking(PlayState* play, Actor* actor);
void func_809C4DA4(EnBomBowlMan* thisx, PlayState* play);
void func_809C5598(EnBomBowlMan* thisx, PlayState* play);
}

void Rando::ActorBehavior::InitEnBomBowlManBehavior() {
    // This handles getting the check in East clock town
    COND_VB_SHOULD(VB_GIVE_ITEM_FROM_OFFER, IS_RANDO, {
        GetItemId* item = va_arg(args, GetItemId*);
        Actor* actor = va_arg(args, Actor*);
        Player* player = GET_PLAYER(gPlayState);

        if (actor->id != ACTOR_EN_BOM_BOWL_MAN) {
            return;
        }

        EnBomBowlMan* enBomBowlMan = (EnBomBowlMan*)actor;

        *should = false;
        actor->parent = &player->actor;
        player->talkActor = actor;
        player->talkActorDistance = actor->xzDistToPlayer;
        player->exchangeItemAction = PLAYER_IA_MINUS1;
        Player_StartTalking(gPlayState, actor);
        actor->textId = 0x735;
        SET_WEEKEVENTREG(WEEKEVENTREG_84_80);
    });

    // This handles the check in north clock town, after bombers have been found
    COND_VB_SHOULD(VB_BOM_BOWL_MAN_GIVE_ITEM, IS_RANDO, {
        Actor* actor = va_arg(args, Actor*);
        EnBomBowlMan* enBomBowlMan = (EnBomBowlMan*)actor;

        *should = false;

        enBomBowlMan->unk_2C0 = 3;
        enBomBowlMan->actor.textId = 0x716;
        if (enBomBowlMan->unk_2F6 == ENBOMBOWLMAN_F0_0) {
            enBomBowlMan->actionFunc = func_809C4DA4;
        } else {
            enBomBowlMan->actionFunc = func_809C5598;
        }
    });

    // Override the original requirement, which is the absence of the Bombers' Notebook
    COND_VB_SHOULD(VB_BE_ELIGIBLE_FOR_BOMBERS_NOTEBOOK, IS_RANDO, {
        *should =
            CHECK_WEEKEVENTREG(WEEKEVENTREG_73_80) && !RANDO_SAVE_CHECKS[RC_CLOCK_TOWN_BOMBERS_NOTEBOOK].cycleObtained;
    });
};
