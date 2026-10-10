#include "ActorBehavior.h"
#include <libultraship/bridge/consolevariablebridge.h>

extern "C" {
#include "variables.h"
void Player_StartTalking(PlayState* play, Actor* actor);
}

void Rando::ActorBehavior::InitEnLiftNutsBehavior() {
    COND_VB_SHOULD(VB_GIVE_ITEM_FROM_OFFER, IS_RANDO, {
        GetItemId* item = va_arg(args, GetItemId*);
        Actor* refActor = va_arg(args, Actor*);
        Player* player = GET_PLAYER(gPlayState);

        if (refActor->id != ACTOR_EN_LIFT_NUTS) {
            return;
        }

        if ((CURRENT_DAY == 3 && (!CHECK_WEEKEVENTREG(WEEKEVENTREG_WON_DEKU_PLAYGROUND_DAY_1) ||
                                  !CHECK_WEEKEVENTREG(WEEKEVENTREG_WON_DEKU_PLAYGROUND_DAY_2))) ||
            (CURRENT_DAY == 2 || CURRENT_DAY == 1)) {
            RANDO_SAVE_CHECKS[RC_DEKU_PLAYGROUND_ANY_DAY].eligible = true;
        }

        *should = false;
        refActor->parent = &player->actor;
        player->talkActor = refActor;
        player->talkActorDistance = refActor->xzDistToPlayer;
        player->exchangeItemAction = PLAYER_IA_MINUS1;
        Player_StartTalking(gPlayState, refActor);
    });
}