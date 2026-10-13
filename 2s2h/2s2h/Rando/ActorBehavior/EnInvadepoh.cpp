#include "ActorBehavior.h"

void Rando::ActorBehavior::InitEnInvadepohBehavior() {
    // This is the same block found for non-scripted actors in OfferGetItem.cpp, but without Player_StartTalking().
    COND_VB_SHOULD(VB_GIVE_ITEM_FROM_OFFER, IS_RANDO, {
        GetItemId* item = va_arg(args, GetItemId*);
        Actor* actor = va_arg(args, Actor*);
        if (actor->id == ACTOR_EN_INVADEPOH) { // Romani Ranch invasion actor
            *should = false;
            Player* player = GET_PLAYER(gPlayState);
            actor->parent = &player->actor;
            player->talkActor = actor;
            player->talkActorDistance = actor->xzDistToPlayer;
            player->exchangeItemAction = PLAYER_IA_MINUS1;
        }
    });
}