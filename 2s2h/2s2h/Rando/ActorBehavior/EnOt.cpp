#include "ActorBehavior.h"

extern "C" {
#include "variables.h"
}

/*
 * This is the same block found for non-scripted actors in OfferGetItem.cpp, with the removal
 * of Player_StartTalking()
 */
void Rando::ActorBehavior::InitEnOtBehavior() {
    COND_VB_SHOULD(VB_GIVE_ITEM_FROM_OFFER, IS_RANDO, {
        GetItemId* item = va_arg(args, GetItemId*);
        Actor* actor = va_arg(args, Actor*);
        if (actor->id == ACTOR_EN_OT) {
            Player* player = GET_PLAYER(gPlayState);
            *should = false;
            actor->parent = &player->actor;
            player->talkActor = actor;
            player->talkActorDistance = actor->xzDistToPlayer;
            player->exchangeItemAction = PLAYER_IA_MINUS1;
        }
    });
}
