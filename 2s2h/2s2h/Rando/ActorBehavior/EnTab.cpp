#include "ActorBehavior.h"
#include "Rando/MiscBehavior/MiscBehavior.h"
#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/CustomMessage/CustomMessage.h"

extern "C" {
#include "variables.h"
#include "overlays/actors/ovl_En_Tab/z_en_tab.h"
}

#define ENTAB_EMPTY_BOTTLE_CHECK_FAILED_SCRIPT_POS 0x000F
#define MSCRIPT_OFFER_ITEM_CMD_LEN 0x5

static bool isInitialGiveItemMscriptCommandExecution = false;

int32_t EnTab_OverrideBottleCheckCallback(Actor* thisx, PlayState* play) {
    // Should always return true - as if the player always has an empty bottle!
    // See func_80BE0D38 in z_en_tab.c
    return true;
}

void EnTab_OnOpenShopText(u16* textId, bool* loadFromMessageTable) {
    RandoSaveCheck milkPurchaseCheck = RANDO_SAVE_CHECKS[RC_MILK_BAR_PURCHASE_MILK];
    RandoSaveCheck chateauPurchaseCheck = RANDO_SAVE_CHECKS[RC_MILK_BAR_PURCHASE_CHATEAU];

    RandoItemId riMilkPurchase = Rando::ConvertItem(milkPurchaseCheck.randoItemId, RC_MILK_BAR_PURCHASE_MILK);
    RandoItemId riChateauPurchase = Rando::ConvertItem(chateauPurchaseCheck.randoItemId, RC_MILK_BAR_PURCHASE_CHATEAU);

    auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
    entry.autoFormat = false;

    entry.msg = "\x02\xC3{item1}\x01 {price1} Rupees\x11"
                "\x02{item2}\x01 {price2} Rupees\x11"
                "\x02Nothing";

    std::string itemName1 = "Milk";
    std::string itemPrice1 = "20";
    if (!milkPurchaseCheck.cycleObtained) {
        itemName1 = Rando::StaticData::GetItemName(riMilkPurchase, false, RC_MILK_BAR_PURCHASE_MILK);
        itemPrice1 = std::to_string(milkPurchaseCheck.price);
    }

    std::string itemName2 = "Chateau Romani";
    std::string itemPrice2 = "200";
    if (!chateauPurchaseCheck.cycleObtained) {
        itemName2 = Rando::StaticData::GetItemName(riChateauPurchase, false, RC_MILK_BAR_PURCHASE_CHATEAU);
        itemPrice2 = std::to_string(chateauPurchaseCheck.price);
    }

    CustomMessage::Replace(&entry.msg, "{item1}", itemName1);
    CustomMessage::Replace(&entry.msg, "{item2}", itemName2);
    CustomMessage::Replace(&entry.msg, "{price1}", itemPrice1);
    CustomMessage::Replace(&entry.msg, "{price2}", itemPrice2);
    CustomMessage::EnsureMessageEnd(&entry.msg);
    CustomMessage::LoadCustomMessageIntoFont(entry);
    *loadFromMessageTable = false;
};

void Rando::ActorBehavior::InitEnTabBehavior() {
    // Give the randomized items instead if they haven't already been purchased
    // Otherwise, give vanilla refills but check for bottles first
    COND_VB_SHOULD(VB_GIVE_ITEM_FROM_OFFER, IS_RANDO && RANDO_SAVE_OPTIONS[RO_SHUFFLE_SHOPS], {
        GetItemId* item = va_arg(args, GetItemId*);
        Actor* actor = va_arg(args, Actor*);

        if (actor->id == ACTOR_EN_TAB) {
            RandoCheckId checkId =
                gPlayState->msgCtx.choiceIndex == 0 ? RC_MILK_BAR_PURCHASE_MILK : RC_MILK_BAR_PURCHASE_CHATEAU;

            if (!RANDO_SAVE_CHECKS[checkId].cycleObtained) {
                RANDO_SAVE_CHECKS[checkId].eligible = true;
                *should = false;
            } else if (!Inventory_HasEmptyBottle()) {
                *should = false;
            }

            // Update actor parent to avoid looping over MSCRIPT_OFFER_ITEM infinitely
            Player* player = GET_PLAYER(gPlayState);
            EnTab* tabActor = (EnTab*)actor;
            tabActor->actor.parent = &player->actor;
        }
    });

    // Use the randomized prices for message script branching/game state updates
    COND_VB_SHOULD(VB_EXEC_MSG_EVENT, IS_RANDO && RANDO_SAVE_OPTIONS[RO_SHUFFLE_SHOPS], {
        u32 cmdId = va_arg(args, u32);
        Actor* actor = va_arg(args, Actor*);

        if (actor->id != ACTOR_EN_TAB) { // Mr. Barten
            return;
        }

        MsgScript* script = va_arg(args, MsgScript*);
        MsgScriptCallback* callback = va_arg(args, MsgScriptCallback*);

        // Override actor parent to skip item grant if the player is trying to buy
        // vanilla milk items and does not have a bottle
        if (cmdId == MSCRIPT_CMD_ID_OFFER_ITEM) {
            RandoCheckId checkId =
                gPlayState->msgCtx.choiceIndex == 0 ? RC_MILK_BAR_PURCHASE_MILK : RC_MILK_BAR_PURCHASE_CHATEAU;

            if (RANDO_SAVE_CHECKS[checkId].cycleObtained && isInitialGiveItemMscriptCommandExecution &&
                !Inventory_HasEmptyBottle()) {
                // Make sure that skip branch is taken in Mscript handler by setting actor parent to player
                Player* player = GET_PLAYER(gPlayState);
                EnTab* tabActor = (EnTab*)actor;
                tabActor->actor.parent = &player->actor;

                // Update skip offset to point to Open Bottle Failure MsgScript data
                s16 skipOffset = 0;
                if (gPlayState->msgCtx.choiceIndex == 0) {
                    skipOffset = ENTAB_EMPTY_BOTTLE_CHECK_FAILED_SCRIPT_POS - 0x004F - MSCRIPT_OFFER_ITEM_CMD_LEN;
                } else {
                    skipOffset = ENTAB_EMPTY_BOTTLE_CHECK_FAILED_SCRIPT_POS - 0x0040 - MSCRIPT_OFFER_ITEM_CMD_LEN;
                }

                script[3] = skipOffset >> 8;   // upper byte of skipOffset
                script[4] = skipOffset & 0xFF; // lower byte of skipOffset

                // Add helpful error sound to alert player of custom bottle check behavior
                Audio_PlaySfx(NA_SE_SY_ERROR);
            } else {
                // Write vanilla values to skip offset for script command
                s16 skipOffset = 0;
                script[3] = skipOffset >> 8;
                script[4] = skipOffset & 0xFF;
            }

            if (isInitialGiveItemMscriptCommandExecution) {
                isInitialGiveItemMscriptCommandExecution = false;
            }
        }

        // Use check prices instead of vanilla for MSCRIPT_BRANCH_ON_RUPEES
        if (cmdId == MSCRIPT_CMD_ID_CHECK_RUPEES) {
            s16 checkPrice = 0;
            if (gPlayState->msgCtx.choiceIndex == 0) {
                checkPrice = 20;

                if (!RANDO_SAVE_CHECKS[RC_MILK_BAR_PURCHASE_MILK].cycleObtained) {
                    checkPrice = RANDO_SAVE_CHECKS[RC_MILK_BAR_PURCHASE_MILK].price;
                }
            } else {
                checkPrice = 200;

                if (!RANDO_SAVE_CHECKS[RC_MILK_BAR_PURCHASE_CHATEAU].cycleObtained) {
                    checkPrice = RANDO_SAVE_CHECKS[RC_MILK_BAR_PURCHASE_CHATEAU].price;
                }
            }

            script[1] = checkPrice >> 8;   // upper byte of price
            script[2] = checkPrice & 0xFF; // lower byte of price
        }

        // MSCRIPT_BEGIN_TEXT
        if (cmdId == MSCRIPT_CMD_ID_BEGIN_TEXT) {
            isInitialGiveItemMscriptCommandExecution = true;
        }

        // Need to reset actor parent which otherwise would've been reset by vanilla Give Item cutscene in MSCRIPT_DONE
        if (cmdId == MSCRIPT_CMD_ID_DONE) {
            Player* player = GET_PLAYER(gPlayState);
            EnTab* tabActor = (EnTab*)actor;
            tabActor->actor.parent = NULL;
        }

        // Charge Link the randomized price when calling MSCRIPT_CHANGE_RUPEES
        // Will not charge Link when purchasing vanilla refills without an empty bottle
        if (cmdId == MSCRIPT_CMD_ID_CHANGE_RUPEES) {
            RandoCheckId checkId =
                gPlayState->msgCtx.choiceIndex == 0 ? RC_MILK_BAR_PURCHASE_MILK : RC_MILK_BAR_PURCHASE_CHATEAU;
            s16 rupeeChangeAmt = gPlayState->msgCtx.choiceIndex == 0 ? -20 : -200;

            if (!RANDO_SAVE_CHECKS[checkId].cycleObtained) {
                rupeeChangeAmt = -RANDO_SAVE_CHECKS[checkId].price;
            } else if (!Inventory_HasEmptyBottle()) {
                rupeeChangeAmt = 0;
            }

            script[1] = rupeeChangeAmt >> 8;   // upper byte of price
            script[2] = rupeeChangeAmt & 0xFF; // lower byte of price
        }

        // Override callback function depending on actor state for MSCRIPT_BRANCH_ON_CALLBACK_2
        if (cmdId == MSCRIPT_CMD_ID_CHECK_CALLBACK_CONTINUE) {
            *callback = EnTab_OverrideBottleCheckCallback;
        }
    });

    COND_ID_HOOK(OnOpenText, 0x2B0B, IS_RANDO && RANDO_SAVE_OPTIONS[RO_SHUFFLE_SHOPS], EnTab_OnOpenShopText);
}