#include "ActorBehavior.h"
#include <libultraship/bridge/consolevariablebridge.h>

extern "C" {
#include "variables.h"
#include "src/overlays/actors/ovl_En_Trt/z_en_trt.h"
#include "src/overlays/actors/ovl_En_Trt2/z_en_trt2.h"
void Player_StartTalking(PlayState* play, Actor* actor);
void EnTrt_ItemGiven(EnTrt* enTrt, PlayState* play);
}

void EnTrt_CompleteDialogue(Actor* actor) {
    EnTrt* refActor = (EnTrt*)actor;
    Player_StartTalking(gPlayState, &refActor->actor);
    refActor->actionFunc = EnTrt_ItemGiven;
}

void EnTrt2_CompleteDialogue(Actor* actor) {
    EnTrt2* refActor = (EnTrt2*)actor;
    refActor->unk_3B2 = 13;
    Player_StartTalking(gPlayState, &refActor->actor);
}

void Rando::ActorBehavior::InitEnTrtBehavior() {
    COND_VB_SHOULD(VB_GIVE_ITEM_FROM_OFFER, IS_RANDO, {
        GetItemId* item = va_arg(args, GetItemId*);
        Actor* actor = va_arg(args, Actor*);
        Player* player = GET_PLAYER(gPlayState);

        // Let vanilla item give continue
        if ((actor->id != ACTOR_EN_TRT && actor->id != ACTOR_EN_TRT2) ||
            !RANDO_SAVE_CHECKS[RC_HAGS_POTION_SHOP_KOTAKE].shuffled ||
            (*item != GI_POTION_RED_BOTTLE && *item != GI_POTION_RED)) {
            return;
        }

        if (!RANDO_SAVE_CHECKS[RC_HAGS_POTION_SHOP_KOTAKE].cycleObtained) {
            SET_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_KOTAKE_BOTTLE);
            SET_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_RED_POTION_FOR_KOUME);
        }

        // In either case here, WEEKEVENTREG_RECEIVED_KOTAKE_BOTTLE will be set and trigger the rando
        // check to be queued.

        // If they have an empty bottle, force item to red potion and let vanilla item get continue
        if (Inventory_HasEmptyBottle()) {
            *item = GI_POTION_RED;
            return;
        }

        // Otherwise bypass item get entirely
        *should = false;

        actor->parent = &player->actor;
        player->talkActor = actor;
        player->talkActorDistance = actor->xzDistToPlayer;
        player->exchangeItemAction = PLAYER_IA_MINUS1;

        if (actor->id == ACTOR_EN_TRT) {
            EnTrt_CompleteDialogue(actor);
        } else {
            EnTrt2_CompleteDialogue(actor);
        }
    });
}
