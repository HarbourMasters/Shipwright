#include "ActorBehavior.h"
#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/CustomMessage/CustomMessage.h"

extern "C" {
#include "variables.h"
#include "overlays/actors/ovl_En_Geg/z_en_geg.h"
}

void Rando::ActorBehavior::InitEnGegBehavior() {
    COND_ID_HOOK(OnActorInit, ACTOR_EN_GEG, IS_RANDO, [](Actor* actor) { SET_WEEKEVENTREG(WEEKEVENTREG_35_40); });

    COND_VB_SHOULD(VB_GIVE_DON_GERO_MASK, IS_RANDO, {
        EnGeg* refActor = va_arg(args, EnGeg*);

        if (refActor == nullptr || refActor->actor.id != ACTOR_EN_GEG) {
            return;
        }
        SET_WEEKEVENTREG(WEEKEVENTREG_61_01);
        refActor->unk_496 = 0xD75;
        Message_StartTextbox(gPlayState, 0xD75, &refActor->actor);

        *should = false;
    });

    COND_ID_HOOK(OnOpenText, 0xd75, IS_RANDO, [](u16* textId, bool* loadFromMessageTable) {
        auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
        RandoItemId randoItemId = RANDO_SAVE_CHECKS[RC_MOUNTAIN_VILLAGE_DON_GERO_MASK].randoItemId;
        entry.msg = "I could tell you really wanted %y{{itemName}}%w! I'm going back to Goron Village.\xE0";

        CustomMessage::Replace(&entry.msg, "{{itemName}}",
                               Rando::StaticData::GetItemName(randoItemId, true, RC_MOUNTAIN_VILLAGE_DON_GERO_MASK));
        CustomMessage::LoadCustomMessageIntoFont(entry);
        *loadFromMessageTable = false;
    });
}