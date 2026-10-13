#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"
#include "2s2h/BenGui/Notification.h"
#include "2s2h/Rando/Rando.h"
#include "2s2h/CustomMessage/CustomMessage.h"
#include "2s2h/CustomItem/CustomItem.h"

#define CVAR_NAME "gEnhancements.Timesavers.AutoBankDeposit"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

#define BANK_MAX_CAPACITY 5000

std::vector<std::string> bankDepositMessages = {
    "STONKS!", "You should invest!", "Cha-ching!", "Money in the bank!", "Rupees secured!",
};

static void EmitDepositNotification(s16 newBalance) {
    Notification::Options notif = {};
    notif.prefix = bankDepositMessages[rand() % bankDepositMessages.size()];
    notif.prefixColor = ImVec4(0.4f, 0.7f, 1.0f, 1.0f);
    notif.message = "Bank Balance:";
    notif.messageColor = ImVec4(0.9f, 0.9f, 0.9f, 1.0f);
    notif.suffix = std::to_string(newBalance);
    notif.suffixColor = ImVec4(0.3f, 1.0f, 0.3f, 1.0f);
    notif.remainingTime = 6.0f;
    Notification::Emit(notif);
}

static void GrantBankFirstReward() {
    SET_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_BANK_WALLET_UPGRADE);

    if (IS_RANDO) {
        RANDO_SAVE_CHECKS[RC_CLOCK_TOWN_WEST_BANK_ADULTS_WALLET].eligible = true;
    } else {
        u32 walletLevel = CUR_UPG_VALUE(UPG_WALLET);
        s16 itemDrawId = (walletLevel == 0) ? (s16)GID_WALLET_ADULT : (s16)GID_WALLET_GIANT;

        GameInteractor::Instance->events.emplace_back(GIEventGiveItem{
            .showGetItemCutscene = true, .param = itemDrawId, .giveItem = [](Actor* actor, PlayState* play) {
                u32 walletLevel = CUR_UPG_VALUE(UPG_WALLET);
                ItemId wallet = (walletLevel == 0) ? ITEM_WALLET_ADULT : ITEM_WALLET_GIANT;
                const char* walletName = (walletLevel == 0) ? "Adult's Wallet" : "Giant's Wallet";

                if (CUSTOM_ITEM_FLAGS & CustomItem::GIVE_ITEM_CUTSCENE) {
                    CustomMessage::SetActiveCustomMessage(std::string("You got ") + walletName + "!",
                                                          { .textboxType = 2 });
                } else {
                    CustomMessage::StartTextbox(std::string("You got ") + walletName + "!\x1C\x02\x10",
                                                { .textboxType = 2 });
                }

                Item_Give(play, wallet);
            } });
    }
}

static void GrantBankInterestReward() {
    SET_WEEKEVENTREG(WEEKEVENTREG_59_80);

    if (IS_RANDO) {
        RANDO_SAVE_CHECKS[RC_CLOCK_TOWN_WEST_BANK_INTEREST].eligible = true;
    } else {
        GameInteractor::Instance->events.emplace_back(GIEventGiveItem{
            .showGetItemCutscene = true, .param = GID_RUPEE_BLUE, .giveItem = [](Actor* actor, PlayState* play) {
                if (CUSTOM_ITEM_FLAGS & CustomItem::GIVE_ITEM_CUTSCENE) {
                    CustomMessage::SetActiveCustomMessage("You got a Blue Rupee!", { .textboxType = 2 });
                } else {
                    CustomMessage::StartTextbox("You got a Blue Rupee!\x1C\x02\x10", { .textboxType = 2 });
                }

                Item_Give(play, ITEM_RUPEE_BLUE);
            } });
    }
}

static void GrantBankFinalReward() {
    SET_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_BANK_HEART_PIECE);

    if (IS_RANDO) {
        RANDO_SAVE_CHECKS[RC_CLOCK_TOWN_WEST_BANK_PIECE_OF_HEART].eligible = true;
    } else {
        GameInteractor::Instance->events.emplace_back(GIEventGiveItem{
            .showGetItemCutscene = true, .param = GID_HEART_PIECE, .giveItem = [](Actor* actor, PlayState* play) {
                if (CUSTOM_ITEM_FLAGS & CustomItem::GIVE_ITEM_CUTSCENE) {
                    CustomMessage::SetActiveCustomMessage("You got a Piece of Heart!", { .textboxType = 2 });
                } else {
                    CustomMessage::StartTextbox("You got a Piece of Heart!\x1C\x02\x10", { .textboxType = 2 });
                }

                Item_Give(play, ITEM_HEART_PIECE);
            } });
    }
}

static void GrantBankerReward(s16 balanceBeforeDeposit, s16 balanceAfterDeposit) {
    bool useCustomThresholds = CVarGetInteger("gEnhancements.DifficultyOptions.LowerBankRewardThresholds", 0);

    s16 walletThreshold = useCustomThresholds ? 100 : 200;
    s16 interestThreshold = useCustomThresholds ? 500 : 1000;
    s16 heartPieceThreshold = useCustomThresholds ? 1000 : 5000;

    if (balanceBeforeDeposit < walletThreshold && balanceAfterDeposit >= walletThreshold &&
        !CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_BANK_WALLET_UPGRADE)) {
        GrantBankFirstReward();
    }

    if (balanceBeforeDeposit < interestThreshold && balanceAfterDeposit >= interestThreshold &&
        !CHECK_WEEKEVENTREG(WEEKEVENTREG_59_80)) {
        GrantBankInterestReward();
    }

    if (balanceBeforeDeposit < heartPieceThreshold && balanceAfterDeposit >= heartPieceThreshold &&
        !CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_BANK_HEART_PIECE)) {
        GrantBankFinalReward();
    }
}

static bool CrossedNotificationThreshold(s16 balanceBeforeDeposit, s16 balanceAfterDeposit) {
    const s16 milestones[] = { 100, 200, 500, 1000, 5000 };

    for (s16 milestone : milestones) {
        if (balanceBeforeDeposit < milestone && balanceAfterDeposit >= milestone) {
            return true;
        }
    }

    return false;
}

static void HandleWalletOverflow() {
    s16 currentBankBalance = HS_GET_BANK_RUPEES();

    if (currentBankBalance >= BANK_MAX_CAPACITY) {
        return;
    }

    s16 spaceInBank = BANK_MAX_CAPACITY - currentBankBalance;
    s16 depositAmount = MIN(gSaveContext.rupeeAccumulator, spaceInBank);

    if (depositAmount > 0) {
        s16 balanceBeforeDeposit = currentBankBalance;
        s16 balanceAfterDeposit = currentBankBalance + depositAmount;

        HS_SET_BANK_RUPEES(balanceAfterDeposit);
        gSaveContext.rupeeAccumulator -= depositAmount;

        if (CrossedNotificationThreshold(balanceBeforeDeposit, balanceAfterDeposit)) {
            EmitDepositNotification(balanceAfterDeposit);
        }
        GrantBankerReward(balanceBeforeDeposit, balanceAfterDeposit);
    }
}

static void RegisterAutoBankDeposit() {
    COND_VB_SHOULD(VB_DISCARD_EXCESS_RUPEES, CVAR, {
        HandleWalletOverflow();

        if (gSaveContext.rupeeAccumulator == 0) {
            *should = true;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterAutoBankDeposit, { CVAR_NAME });
