#include "ActorBehavior.h"
#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/CustomMessage/CustomMessage.h"

extern "C" {
#include "variables.h"
#include "overlays/actors/ovl_En_Scopenuts/z_en_scopenuts.h"

void Player_StartTalking(PlayState* play, Actor* actor);
void func_80BCB980(EnScopenuts* enScopenuts, PlayState* play);
}

void Rando::ActorBehavior::InitEnScopenutsBehavior() {
    COND_VB_SHOULD(VB_GIVE_ITEM_FROM_OFFER, IS_RANDO, {
        GetItemId* item = va_arg(args, GetItemId*);
        Actor* actor = va_arg(args, Actor*);
        Player* player = GET_PLAYER(gPlayState);
        if (actor->id == ACTOR_EN_SCOPENUTS) {
            EnScopenuts* enScopenuts = (EnScopenuts*)actor;
            *should = false;
            SET_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_BUSINESS_SCRUB_HEART_PIECE);
            actor->parent = &player->actor;
            player->talkActor = actor;
            player->talkActorDistance = actor->xzDistToPlayer;
            player->exchangeItemAction = PLAYER_IA_MINUS1;
            Player_StartTalking(gPlayState, actor);
            enScopenuts->actionFunc = func_80BCB980;
        }
    });

    COND_ID_HOOK(OnOpenText, 0x1631, IS_RANDO, [](u16* textId, bool* loadFromMessageTable) {
        auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
        RandoItemId randoItemId = RANDO_SAVE_CHECKS[RC_TERMINA_FIELD_GROTTO_SCRUB].randoItemId;
        entry.msg = "Please! I'll sell you %y{{itemName}}%w if you just keep this place a secret...\xE0";

        CustomMessage::Replace(&entry.msg, "{{itemName}}",
                               Rando::StaticData::GetItemName(randoItemId, true, RC_TERMINA_FIELD_GROTTO_SCRUB));
        CustomMessage::LoadCustomMessageIntoFont(entry);
        *loadFromMessageTable = false;
    });

    COND_VB_SHOULD(VB_SCOPENUTS_CONSIDER_FIRST_CYCLE, IS_RANDO, { *should = false; });
}
