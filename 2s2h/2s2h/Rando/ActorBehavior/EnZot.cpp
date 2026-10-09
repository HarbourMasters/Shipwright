#include "ActorBehavior.h"
#include <libultraship/bridge/consolevariablebridge.h>

extern "C" {
#include "variables.h"
void Player_StartTalking(PlayState* play, Actor* actor);
}

void Rando::ActorBehavior::InitEnZotBehavior() {
    COND_VB_SHOULD(VB_GIVE_ITEM_FROM_OFFER, IS_RANDO, {
        GetItemId* item = va_arg(args, GetItemId*);
        Actor* refActor = va_arg(args, Actor*);
        Player* player = GET_PLAYER(gPlayState);

        if (refActor->id != ACTOR_EN_ZOT || gPlayState->sceneId != SCENE_33ZORACITY) {
            return;
        }

        // There are multiple Zora with ACTOR_EN_ZOT in the same room, in the event we add checks to ones that don't
        // have any I am specifying the Params for the Zora Hall Scene Lights.
        if (refActor->params == -1015) {
            RANDO_SAVE_CHECKS[RC_ZORA_HALL_SCENE_LIGHTS].eligible = true;
            *should = false;
            refActor->parent = &player->actor;
            player->talkActor = refActor;
            player->talkActorDistance = refActor->xzDistToPlayer;
            player->exchangeItemAction = PLAYER_IA_MINUS1;
            Player_StartTalking(gPlayState, refActor);
        }
    });
}