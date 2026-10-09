#include "ActorBehavior.h"
#include <libultraship/bridge/consolevariablebridge.h>

extern "C" {
#include "variables.h"
#include "overlays/actors/ovl_En_Baba/z_en_baba.h"
void Player_StartTalking(PlayState* play, Actor* actor);
void EnBaba_GaveBlastMask(EnBaba* enBaba, PlayState* play);
}

#define BOMB_SHOP_LADY_STATE_GAVE_BLAST_MASK 64

// The Bomb Shop Lady's item give is non-scripted, but the catch-all for VB_GIVE_ITEM_FROM_OFFER does not work for
// this case, as Link can move freely once the next textbox appears. This code fixes that.
void Rando::ActorBehavior::InitEnBabaBehavior() {
    COND_VB_SHOULD(VB_GIVE_ITEM_FROM_OFFER, IS_RANDO, {
        GetItemId* item = va_arg(args, GetItemId*);
        Actor* actor = va_arg(args, Actor*);
        // This runs for all actors using Actor_OfferGetItem, so make sure we only do this with the Bomb Shop Lady.
        if (actor->id == ACTOR_EN_BABA) {
            *should = false;
            EnBaba* enBaba = (EnBaba*)actor;
            Player* player = GET_PLAYER(gPlayState);
            enBaba->stateFlags |= BOMB_SHOP_LADY_STATE_GAVE_BLAST_MASK;
            enBaba->actionFunc = EnBaba_GaveBlastMask;
            enBaba->actor.parent = &player->actor;
            player->talkActor = &enBaba->actor;
            player->talkActorDistance = enBaba->actor.xzDistToPlayer;
            player->exchangeItemAction = PLAYER_IA_MINUS1;
            Player_StartTalking(gPlayState, &enBaba->actor);
            RANDO_SAVE_CHECKS[RC_CLOCK_TOWN_NORTH_BOMB_LADY].eligible = true;
        }
    });

    COND_VB_SHOULD(VB_HAVE_BLAST_MASK, IS_RANDO,
                   { *should = RANDO_SAVE_CHECKS[RC_CLOCK_TOWN_NORTH_BOMB_LADY].cycleObtained; });
}
