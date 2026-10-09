#include "ActorBehavior.h"
#include <libultraship/bridge/consolevariablebridge.h>

extern "C" {
#include "variables.h"
void Player_StartTalking(PlayState* play, Actor* actor);
}

/*
 * This is the same block found for non-scripted actors in OfferGetItem.cpp, with the addition of unsetting
 * ACTOR_FLAG_TALK. Without that, the actor will bring up an extra textbox after the applause cutscene ends.
 * Link can freely move when this extra textbox appears.
 */
void Rando::ActorBehavior::InitEnRzBehavior() {
    COND_VB_SHOULD(VB_GIVE_ITEM_FROM_OFFER, IS_RANDO, {
        GetItemId* item = va_arg(args, GetItemId*);
        Actor* actor = va_arg(args, Actor*);
        if (actor->id == ACTOR_EN_RZ) {
            Player* player = GET_PLAYER(gPlayState);
            actor->parent = &player->actor;
            player->talkActor = actor;
            player->talkActorDistance = actor->xzDistToPlayer;
            player->exchangeItemAction = PLAYER_IA_MINUS1;
            Player_StartTalking(gPlayState, actor);
            actor->flags &= ~ACTOR_FLAG_TALK;
            *should = false;
        }
    });
}
