#include "MiscBehavior.h"
#include "2s2h/CustomMessage/CustomMessage.h"

#define BANK_SIGN_MESSAGE_ID 0x1C14

void Rando::MiscBehavior::BankSignHint() {
    bool shouldRegister = IS_RANDO && RANDO_SAVE_OPTIONS[RO_HINTS_BANK_SIGN];

    COND_ID_HOOK(OnOpenText, BANK_SIGN_MESSAGE_ID, shouldRegister, [](u16* textId, bool* loadFromMessageTable) {
        auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
        RandoCheckId randoCheckId = RC_CLOCK_TOWN_WEST_BANK_PIECE_OF_HEART;
        auto& saveCheck = RANDO_SAVE_CHECKS[randoCheckId];

        if (!saveCheck.shuffled || saveCheck.obtained) {
            return; // Allow vanilla behavior to handle the text natively
        }

        bool useCustomThresholds = CVarGetInteger("gEnhancements.DifficultyOptions.LowerBankRewardThresholds", 0);
        std::string threshold = useCustomThresholds ? "1000" : "5000";

        entry.msg = "%gSpecial Promotion!%w\n"
                    "Deposit %r" +
                    threshold +
                    " Rupees%w in your account and "
                    "receive %g{{item}}%w!\n"
                    "-Clock Town Bank";

        CustomMessage::Replace(&entry.msg, "{{item}}",
                               Rando::StaticData::GetItemName(saveCheck.randoItemId, true, randoCheckId));

        CustomMessage::LoadCustomMessageIntoFont(entry);
        *loadFromMessageTable = false;
    });
}
