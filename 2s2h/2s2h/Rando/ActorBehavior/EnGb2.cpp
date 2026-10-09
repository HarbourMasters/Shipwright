#include "ActorBehavior.h"
#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/CustomMessage/CustomMessage.h"

extern "C" {
#include "variables.h"
#include "src/overlays/actors/ovl_En_Gb2/z_en_gb2.h"

void Player_StartTalking(PlayState* play, Actor* actor);
}

void Rando::ActorBehavior::InitEnGb2Behavior() {
    COND_VB_SHOULD(VB_GIVE_ITEM_FROM_OFFER, IS_RANDO, {
        GetItemId* item = va_arg(args, GetItemId*);
        Actor* refActor = va_arg(args, Actor*);
        Player* player = GET_PLAYER(gPlayState);

        // Do not override the vanilla Purple rupee grant
        if (refActor->id != ACTOR_EN_GB2 || *item != GI_HEART_PIECE) {
            return;
        }

        if (!RANDO_SAVE_CHECKS[RC_IKANA_CANYON_GHOST_HUT_PIECE_OF_HEART].cycleObtained) {
            RANDO_SAVE_CHECKS[RC_IKANA_CANYON_GHOST_HUT_PIECE_OF_HEART].eligible = true;
        }

        *should = false;

        refActor->parent = &player->actor;
        player->talkActor = refActor;
        player->talkActorDistance = refActor->xzDistToPlayer;
        player->exchangeItemAction = PLAYER_IA_MINUS1;
        Player_StartTalking(gPlayState, refActor);
        /*
         * This actor sets MSGMODE_TEXT_CLOSING state and expects GI to set it back to MSGMODE_TEXT_START. Because the
         * GI is skipped, we manually start the textbox to prevent the player from being able to move during dialog.
         */
        Message_StartTextbox(gPlayState, 0x14DE, refActor);
    });

    COND_ID_HOOK(OnOpenText, 0x14D1, IS_RANDO, [](u16* textId, bool* loadFromMessageTable) {
        if (RANDO_SAVE_CHECKS[RC_IKANA_CANYON_GHOST_HUT_PIECE_OF_HEART].obtained) {
            return;
        }

        std::string checkItemName =
            Rando::StaticData::GetItemName(RANDO_SAVE_CHECKS[RC_IKANA_CANYON_GHOST_HUT_PIECE_OF_HEART].randoItemId,
                                           true, RC_IKANA_CANYON_GHOST_HUT_PIECE_OF_HEART);

        auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
        entry.msg = "If you are seeking the one who is\n";
        entry.msg += "%rstronger%w than you are, you may find\n";
        entry.msg += "%g{{itemName}}%w here...\n";
        entry.msg += "\x10";
        entry.msg += "from a group of spirits plagued by\n";
        entry.msg += "lingering regrets.\xE0";

        CustomMessage::Replace(&entry.msg, "{{itemName}}", checkItemName);
        CustomMessage::LoadCustomMessageIntoFont(entry);
        *loadFromMessageTable = false;
    });
}