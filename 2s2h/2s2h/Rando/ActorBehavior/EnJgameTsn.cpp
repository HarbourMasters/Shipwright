#include "ActorBehavior.h"
#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/CustomMessage/CustomMessage.h"

extern "C" {
#include "variables.h"
void Player_StartTalking(PlayState* play, Actor* actor);
}

void Rando::ActorBehavior::InitEnJgameTsnBehavior() {
    COND_VB_SHOULD(VB_GIVE_ITEM_FROM_OFFER, IS_RANDO, {
        GetItemId* getItemId = va_arg(args, GetItemId*);
        Actor* actor = va_arg(args, Actor*);
        if (actor->id == ACTOR_EN_JGAME_TSN &&
            !RANDO_SAVE_CHECKS[RC_GREAT_BAY_COAST_FISHERMAN_MINIGAME].cycleObtained) {
            *should = false;
            Player* player = GET_PLAYER(gPlayState);
            actor->parent = &player->actor;
            player->talkActor = actor;
            player->talkActorDistance = actor->xzDistToPlayer;
            player->exchangeItemAction = PLAYER_IA_MINUS1;
            Player_StartTalking(gPlayState, actor);
        }
    });

    // Fisherman "Want to play the jumping game for a prize?"
    COND_ID_HOOK(OnOpenText, 0x1096, IS_RANDO, [](u16* textId, bool* loadFromMessageTable) {
        auto randoSaveCheck = RANDO_SAVE_CHECKS[RC_GREAT_BAY_COAST_FISHERMAN_MINIGAME];

        auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
        entry.msg =
            "Want to try my %rjumping game%w for %p20 Rupees%w? Win, and I'll give you %r{{itemName}}%w!\x19\xA8";
        // The same-cycle repeat reward is a purple Rupee
        CustomMessage::Replace(&entry.msg, "{{itemName}}",
                               randoSaveCheck.cycleObtained
                                   ? "50 Rupees"
                                   : Rando::StaticData::GetItemName(randoSaveCheck.randoItemId, true,
                                                                    RC_GREAT_BAY_COAST_FISHERMAN_MINIGAME));

        CustomMessage::LoadCustomMessageIntoFont(entry);
        *loadFromMessageTable = false;
    });
}
