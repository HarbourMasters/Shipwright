#include "ActorBehavior.h"
#include <libultraship/bridge/consolevariablebridge.h>

extern "C" {
#include "variables.h"
#include "overlays/actors/ovl_En_Yb/z_en_yb.h"
void Player_StartTalking(PlayState* play, Actor* actor);
}

void Rando::ActorBehavior::InitEnYbBehavior() {

    /*
     * Kamaro's check upon completing his quest is weird. After he gives you Kamaro's Mask and says his piece, the game
     * checks if you have Kamaro's Mask in your inventory before marking his quest as complete. Normally, this is always
     * true, making the check redundant. For randomizer, we wrap that check.
     */
    COND_VB_SHOULD(VB_HAVE_KAMAROS_MASK, IS_RANDO,
                   { *should = RANDO_SAVE_CHECKS[RC_TERMINA_FIELD_KAMARO_MASK].eligible; });

    COND_VB_SHOULD(VB_GIVE_ITEM_FROM_OFFER, IS_RANDO, {
        GetItemId* item = va_arg(args, GetItemId*);
        Actor* actor = va_arg(args, Actor*);
        if (actor->id == ACTOR_EN_YB) {
            *should = false;
            Player* player = GET_PLAYER(gPlayState);
            actor->parent = &player->actor;
            player->talkActor = actor;
            player->talkActorDistance = actor->xzDistToPlayer;
            player->exchangeItemAction = PLAYER_IA_MINUS1;
            Player_StartTalking(gPlayState, actor);
            RANDO_SAVE_CHECKS[RC_TERMINA_FIELD_KAMARO_MASK].eligible = true;
        }
    });
}
