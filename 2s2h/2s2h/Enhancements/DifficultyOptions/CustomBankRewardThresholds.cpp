#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"
#include "2s2h/CustomMessage/CustomMessage.h"

extern "C" {
#include "variables.h"
#include "overlays/actors/ovl_En_Ginko_Man/z_en_ginko_man.h"
}

#define CVAR_NAME "gEnhancements.DifficultyOptions.LowerBankRewardThresholds"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

const int FIRST_BANK_THRESHOLD = 100;
const int INTEREST_BANK_THRESHOLD = 500;
const int SECOND_BANK_THRESHOLD = 1000;

void RegisterCustomBankRewardThresholds() {
    COND_VB_SHOULD(VB_PASS_FIRST_BANK_THRESHOLD, CVAR, {
        EnGinkoMan* enGinkoMan = va_arg(args, EnGinkoMan*);

        *should = (HS_GET_BANK_RUPEES() >= FIRST_BANK_THRESHOLD) &&
                  (enGinkoMan->previousBankValue < FIRST_BANK_THRESHOLD) &&
                  !CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_BANK_WALLET_UPGRADE);
    });

    COND_VB_SHOULD(VB_PASS_INTEREST_BANK_THRESHOLD, CVAR, {
        EnGinkoMan* enGinkoMan = va_arg(args, EnGinkoMan*);

        *should = (HS_GET_BANK_RUPEES() >= INTEREST_BANK_THRESHOLD &&
                   enGinkoMan->previousBankValue < INTEREST_BANK_THRESHOLD);
    });

    COND_VB_SHOULD(VB_PASS_SECOND_BANK_THRESHOLD, CVAR, {
        EnGinkoMan* enGinkoMan = va_arg(args, EnGinkoMan*);

        if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_BANK_HEART_PIECE) &&
            HS_GET_BANK_RUPEES() >= SECOND_BANK_THRESHOLD) {
            *should = true;
        }
    });

    COND_VB_SHOULD(VB_PASS_SECOND_BANK_THRESHOLD_ALT, CVAR, {
        EnGinkoMan* enGinkoMan = va_arg(args, EnGinkoMan*);

        if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_BANK_HEART_PIECE) &&
            HS_GET_BANK_RUPEES() >= SECOND_BANK_THRESHOLD && enGinkoMan->previousBankValue < SECOND_BANK_THRESHOLD) {
            *should = true;
        }
    });

    COND_ID_HOOK(OnOpenText, 0x045B, CVAR, [](u16* textId, bool* loadFromMessageTable) {
        auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
        entry.autoFormat = false;
        CustomMessage::Replace(&entry.msg, "200", "100");

        CustomMessage::LoadCustomMessageIntoFont(entry);
        *loadFromMessageTable = false;
    });

    COND_ID_HOOK(OnOpenText, 0x045C, CVAR, [](u16* textId, bool* loadFromMessageTable) {
        auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
        entry.autoFormat = false;
        CustomMessage::Replace(&entry.msg, "1000", "500");

        CustomMessage::LoadCustomMessageIntoFont(entry);
        *loadFromMessageTable = false;
    });

    COND_ID_HOOK(OnOpenText, 0x045D, CVAR, [](u16* textId, bool* loadFromMessageTable) {
        auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
        entry.autoFormat = false;
        CustomMessage::Replace(&entry.msg, "5000", "1000");

        CustomMessage::LoadCustomMessageIntoFont(entry);
        *loadFromMessageTable = false;
    });
}

static RegisterShipInitFunc initFunc(RegisterCustomBankRewardThresholds, { CVAR_NAME });
