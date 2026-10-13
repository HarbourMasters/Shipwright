#include "ActorBehavior.h"
#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/CustomMessage/CustomMessage.h"

extern "C" {
#include "variables.h"
#include "src/overlays/actors/ovl_En_Kujiya/z_en_kujiya.h"
void EnKujiya_Wait(EnKujiya* enKujiya, PlayState* play);
}

void Rando::ActorBehavior::InitEnKujiyaBehavior() {
    COND_VB_SHOULD(VB_GIVE_LOTTERY_WINNINGS, IS_RANDO, {
        EnKujiya* refActor = va_arg(args, EnKujiya*);

        RANDO_SAVE_CHECKS[RC_CLOCK_TOWN_WEST_LOTTERY].eligible = true;
        refActor->actionFunc = EnKujiya_Wait;
        *should = false;
    });

    COND_ID_HOOK(OnOpenText, 0x2b5c, IS_RANDO, [](u16* textId, bool* loadFromMessageTable) {
        auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
        entry.msg = "Step right up! For a measly %p10 Rupees%w, your dreams could come true!\x11\x13\x12";
        entry.msg += "Guess all three numbers to win %p{{itemName}}%w!\x19";
        RandoItemId randoItemId = RANDO_SAVE_CHECKS[RC_CLOCK_TOWN_WEST_LOTTERY].randoItemId;
        CustomMessage::Replace(&entry.msg, "{{itemName}}",
                               Rando::StaticData::GetItemName(randoItemId, true, RC_CLOCK_TOWN_WEST_LOTTERY));

        CustomMessage::LoadCustomMessageIntoFont(entry);
        *loadFromMessageTable = false;
    });

    COND_ID_HOOK(OnOpenText, 0x2b66, IS_RANDO, [](u16* textId, bool* loadFromMessageTable) {
        auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
        entry.msg = "Congratulations! You win the jackpot: %p{{itemName}}%w!\x19";
        RandoItemId randoItemId = RANDO_SAVE_CHECKS[RC_CLOCK_TOWN_WEST_LOTTERY].randoItemId;
        CustomMessage::Replace(&entry.msg, "{{itemName}}",
                               Rando::StaticData::GetItemName(randoItemId, true, RC_CLOCK_TOWN_WEST_LOTTERY));

        CustomMessage::LoadCustomMessageIntoFont(entry);
        *loadFromMessageTable = false;
    });
}