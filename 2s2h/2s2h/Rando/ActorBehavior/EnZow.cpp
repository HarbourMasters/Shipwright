#include "ActorBehavior.h"
#include "2s2h/CustomMessage/CustomMessage.h"
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipUtils.h"

extern "C" {
#include "variables.h"
}

void ApplyHookshotHint(u16* textId, bool* loadFromMessageTable) {
    std::string msg = "I overheard those Pirates talk about treasure "
                      "%y{{location}}%w that can help you reach the unreachable!";

    RandoCheckId randoCheckId = Rando::FindItemPlacement(RI_HOOKSHOT);
    CustomMessage::Replace(&msg, "{{location}}", Rando::StaticData::GetLocationNameForHint(randoCheckId, false));

    CustomMessage::Entry entry = {
        .nextMessageID = (u16)0xFFFF,
        .msg = msg,
    };

    CustomMessage::LoadCustomMessageIntoFont(entry);
    *loadFromMessageTable = false;
}

void Rando::ActorBehavior::InitEnZowBehavior() {
    bool shouldRegister = IS_RANDO && RANDO_SAVE_OPTIONS[RO_HINTS_HOOKSHOT];

    COND_ID_HOOK(OnOpenText, 0x12FD, shouldRegister, ApplyHookshotHint);
    COND_ID_HOOK(OnOpenText, 0x12FA, shouldRegister, ApplyHookshotHint);
    COND_ID_HOOK(OnOpenText, 0x1301, shouldRegister, ApplyHookshotHint);
    COND_ID_HOOK(OnOpenText, 0x12FF, shouldRegister, ApplyHookshotHint);
    COND_ID_HOOK(OnOpenText, 0x12F8, shouldRegister, ApplyHookshotHint);
    COND_ID_HOOK(OnOpenText, 0x12F3, shouldRegister, ApplyHookshotHint);
}
