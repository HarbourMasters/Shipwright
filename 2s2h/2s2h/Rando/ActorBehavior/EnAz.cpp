#include "ActorBehavior.h"
#include <libultraship/bridge/consolevariablebridge.h>

extern "C" {
#include "variables.h"
void Player_StartTalking(PlayState* play, Actor* actor);
}

void Rando::ActorBehavior::InitEnAzBehavior() {
    COND_VB_SHOULD(VB_GIVE_ITEM_FROM_OFFER, IS_RANDO, {
        GetItemId* getItemId = va_arg(args, GetItemId*);
        Actor* actor = va_arg(args, Actor*);
        if (actor->id == ACTOR_EN_AZ && *getItemId > GI_RUPEE_PURPLE) { // Beaver Bro non-repeat reward
            Player* player = GET_PLAYER(gPlayState);
            *should = false;
            // Force advance textbox
            Message_StartTextbox(gPlayState, ++actor->textId, actor);
            actor->parent = &player->actor;
            player->talkActor = actor;
            player->talkActorDistance = actor->xzDistToPlayer;
            player->exchangeItemAction = PLAYER_IA_MINUS1;
            Player_StartTalking(gPlayState, actor);
        }
    });
}
