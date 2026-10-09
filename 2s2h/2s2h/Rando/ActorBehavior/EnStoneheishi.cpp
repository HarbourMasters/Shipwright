#include "ActorBehavior.h"
#include <libultraship/bridge/consolevariablebridge.h>

extern "C" {
#include "variables.h"
#include "overlays/actors/ovl_En_Stone_heishi/z_en_stone_heishi.h"
void Player_StartTalking(PlayState* play, Actor* actor);
void func_80BC9E50(EnStoneheishi* enStoneheishi, PlayState* play);
}

/*
 * This is the same block found for non-scripted actors in OfferGetItem.cpp, with the addition of setting
 * ACTOR_FLAG_TALK, other actor data, and finishes actor's dialogue.
 */
void Rando::ActorBehavior::InitEnStoneheishiBehavior() {
    COND_VB_SHOULD(VB_GIVE_ITEM_FROM_OFFER, IS_RANDO, {
        GetItemId* item = va_arg(args, GetItemId*);
        Actor* actor = va_arg(args, Actor*);
        if (actor->id == ACTOR_EN_STONE_HEISHI) {
            Player* player = GET_PLAYER(gPlayState);
            actor->parent = &player->actor;
            player->talkActor = actor;
            player->talkActorDistance = actor->xzDistToPlayer;
            player->exchangeItemAction = PLAYER_IA_MINUS1;
            Player_StartTalking(gPlayState, actor);
            actor->flags |= ACTOR_FLAG_TALK;
            SET_WEEKEVENTREG(WEEKEVENTREG_41_40);
            EnStoneheishi* enStoneheishi = (EnStoneheishi*)actor;
            enStoneheishi->textIdIndex++;
            enStoneheishi->actor.textId = 0x147A;
            enStoneheishi->actionFunc = func_80BC9E50;
            Message_StartTextbox(gPlayState, 0x147A, actor);
            *should = false;
        }
    });

    COND_VB_SHOULD(VB_STONE_HEISHI_SET_ACTION, IS_RANDO, { *should = false; });
}