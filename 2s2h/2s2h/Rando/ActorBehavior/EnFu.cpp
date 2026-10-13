#include "ActorBehavior.h"
#include <libultraship/bridge/consolevariablebridge.h>

extern "C" {
#include "variables.h"
#include "src/overlays/actors/ovl_En_Fu/z_en_fu.h"
void Player_StartTalking(PlayState* play, Actor* actor);
}

void Rando::ActorBehavior::InitEnFuBehavior() {
    COND_VB_SHOULD(VB_GIVE_ITEM_FROM_OFFER, IS_RANDO, {
        GetItemId* item = va_arg(args, GetItemId*);
        Actor* refActor = va_arg(args, Actor*);
        Player* player = GET_PLAYER(gPlayState);

        if (refActor->id != ACTOR_EN_FU) {
            return;
        }

        if (!RANDO_SAVE_CHECKS[RC_CLOCK_TOWN_EAST_HONEY_DARLING_ANY_DAY].cycleObtained) {
            RANDO_SAVE_CHECKS[RC_CLOCK_TOWN_EAST_HONEY_DARLING_ANY_DAY].eligible = true;
        }

        *should = false;
        refActor->parent = &player->actor;
        player->talkActor = refActor;
        player->talkActorDistance = refActor->xzDistToPlayer;
        player->exchangeItemAction = PLAYER_IA_MINUS1;
        Player_StartTalking(gPlayState, refActor);
    });
}