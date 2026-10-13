#include "ActorBehavior.h"
#include <libultraship/bridge/consolevariablebridge.h>

extern "C" {
#include "variables.h"
void Player_StartTalking(PlayState* play, Actor* actor);
}

// This is the same block found for non-scripted actors in OfferGetItem.cpp
void talkToShootingGalleryNpc(Player* player, Actor* actor) {
    player->talkActor = actor;
    player->talkActorDistance = actor->xzDistToPlayer;
    player->exchangeItemAction = PLAYER_IA_MINUS1;
    Player_StartTalking(gPlayState, actor);
}

void Rando::ActorBehavior::InitEnSyatekiManBehavior() {
    COND_VB_SHOULD(VB_GIVE_ITEM_FROM_OFFER, IS_RANDO, {
        GetItemId* item = va_arg(args, GetItemId*);
        Actor* actor = va_arg(args, Actor*);
        // Only hijack the quivers and Heart Pieces; let the rupee rewards remain as normal
        if (actor->id == ACTOR_EN_SYATEKI_MAN && *item > GI_RUPEE_HUGE) {
            Player* player = GET_PLAYER(gPlayState);
            if (gPlayState->sceneId == SCENE_SYATEKI_MIZU) { // Town Shooting Gallery
                if (CURRENT_DAY == 3) {
                    // On the third day, there is additional dialog after the reward
                    talkToShootingGalleryNpc(player, actor);
                    actor->flags |= ACTOR_FLAG_TALK;
                } else {
                    /*
                     * On the other two days, there is no dialog after the reward. The simplest thing to make the NPC
                     * behave is to add an extra textbox. 0x3F8 is "Usually this place is packed..."
                     */
                    Message_StartTextbox(gPlayState, 0x3F8, actor);
                }
            } else if (gPlayState->sceneId == SCENE_SYATEKI_MORI) { // Swamp Shooting Gallery
                talkToShootingGalleryNpc(player, actor);
            }
            actor->parent = &player->actor;
            *should = false;
        }
    });
}