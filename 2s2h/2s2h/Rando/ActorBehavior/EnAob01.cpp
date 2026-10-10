#include "ActorBehavior.h"
#include <libultraship/bridge/consolevariablebridge.h>

extern "C" {
#include "variables.h"
#include "src/overlays/actors/ovl_En_Fu/z_en_fu.h"
void Player_StartTalking(PlayState* play, Actor* actor);
}

void Rando::ActorBehavior::InitEnAob01Behavior() {
    COND_VB_SHOULD(VB_GIVE_ITEM_FROM_OFFER, IS_RANDO, {
        GetItemId* item = va_arg(args, GetItemId*);
        Actor* refActor = va_arg(args, Actor*);
        Player* player = GET_PLAYER(gPlayState);

        if (refActor->id != ACTOR_EN_AOB_01) {
            return;
        }

        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_DOGGY_RACETRACK_HEART_PIECE)) {
            RANDO_SAVE_CHECKS[RC_DOGGY_RACETRACK_PIECE_OF_HEART].eligible = true;
        } else {
            Rando::GiveItem(RI_RUPEE_RED);
        }

        *should = false;
        refActor->parent = &player->actor;
        player->talkActor = refActor;
        player->talkActorDistance = refActor->xzDistToPlayer;
        player->exchangeItemAction = PLAYER_IA_MINUS1;
        Player_StartTalking(gPlayState, refActor);
    });
}