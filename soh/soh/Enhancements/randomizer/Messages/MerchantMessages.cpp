#include "soh/Enhancements/custom-message/MessageCatalog.h"
// RANDOTODO: Consider breaking this up into the different actors
// That speak these hints, like 2Ship does with ActorBehavior

/**
 * This file handles the custom messages for merchants,
 * including both shops and one-off merchants (i.e. Medigoron, Bean Guy,
 * and Carpet Salesman)
 */

#include <libultraship/bridge/consolevariablebridge.h>

#include <soh/OTRGlobals.h>
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ObjectExtension/ObjectExtension.h"
#include "soh/Enhancements/randomizer/randomizer.h"
#include "soh/Enhancements/custom-message/CustomMessageTypes.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include <macros.h>
#include <functions.h>
#include <overlays/actors/ovl_En_Dns/z_en_dns.h>
extern PlayState* gPlayState;
}

#define RAND_GET_ITEM(rc) OTRGlobals::Instance->gRandoContext->GetItemLocation(rc)
#define RAND_GET_OVERRIDE(rc) OTRGlobals::Instance->gRandoContext->overrides[rc]
#define NON_BEAN_MERCHANTS                                                            \
    (RAND_GET_OPTION(RSK_SHUFFLE_MERCHANTS).Is(RO_SHUFFLE_MERCHANTS_ALL_BUT_BEANS) || \
     RAND_GET_OPTION(RSK_SHUFFLE_MERCHANTS).Is(RO_SHUFFLE_MERCHANTS_ALL))

void BuildMerchantMessage(CustomMessage& msg, RandomizerCheck rc, bool mysterious = true) {
    auto location = RAND_GET_ITEM(rc);
    RandomizerGet rgid = location->GetPlacedRandomizerGet();
    CustomMessage itemName;
    std::string color = Rando::StaticData::RetrieveItem(static_cast<RandomizerGet>(rgid)).GetColor();
    // Shop lists want a bare name, everyone else speaks in sentences and wants an article
    bool inShop = Rando::StaticData::GetLocation(rc)->IsShop();
    if (mysterious) {
        itemName = Rando::StaticData::hintTextTable[RHT_MYSTERIOUS_ITEM_CAPITAL].GetHintMessage();
        if (!inShop) {
            itemName = CustomMessages::CreateMessage("randomizer.messages.merchant_messages.build_merchant_message.0") +
                       itemName;
        }
        color = "%g";
    } else if (rgid == RG_ICE_TRAP) {
        rgid = RAND_GET_OVERRIDE(rc).LooksLike();
        Text trickName = RAND_GET_OVERRIDE(rc).GetTrickName();
        if (!inShop) {
            trickName = RAND_GET_OVERRIDE(rc).GetTrickArticle() + trickName;
        }
        itemName = CustomMessage(trickName);
        color = "%g";
    } else if (inShop) {
        itemName = CustomMessage(Rando::StaticData::RetrieveItem(rgid).GetName());
    } else {
        // Hint text brings its own article
        itemName = Rando::StaticData::RetrieveItem(rgid).GetHint().GetHintMessage();
    }
    msg.Replace("[[color]]", color);
    msg.InsertNames({ itemName, CustomMessage(std::to_string(location->GetPrice())) });
}

void BuildBeanGuyMessage(uint16_t* textId, bool* loadFromMessageTable) {
    CustomMessage msg;
    if (*textId == TEXT_BEAN_SALESMAN_BUY_FOR_100) {
        msg = CustomMessages::CreateMessage("randomizer.messages.merchant_messages.build_bean_guy_message.0");
        msg.AutoFormat();
    } else if (*textId == TEXT_BEAN_SALESMAN_BUY_FOR_10) {
        msg = CustomMessages::CreateMessage("randomizer.messages.merchant_messages.build_bean_guy_message.1");
        BuildMerchantMessage(msg, RC_ZR_MAGIC_BEAN_SALESMAN,
                             !RAND_GET_OPTION(RSK_MERCHANT_TEXT_HINT) ||
                                 CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("MysteriousShuffle"), 0));
        msg.AutoFormat();
    }
    msg.LoadIntoFont();
    *loadFromMessageTable = false;
}

void BuildMedigoronMessage(uint16_t* textId, bool* loadFromMessageTable) {
    CustomMessage msg =
        CustomMessages::CreateMessage("randomizer.messages.merchant_messages.build_medigoron_message.0");
    BuildMerchantMessage(msg, RC_GC_MEDIGORON,
                         !RAND_GET_OPTION(RSK_MERCHANT_TEXT_HINT) ||
                             CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("MysteriousShuffle"), 0));
    msg.AutoFormat();
    msg.LoadIntoFont();
    *loadFromMessageTable = false;
}

void BuildGrannyMessage(uint16_t* textId, bool* loadFromMessageTable) {
    if (!Flags_GetRandomizerInf(RAND_INF_MERCHANTS_GRANNYS_SHOP) &&
        (RAND_GET_OPTION(RSK_SHUFFLE_ADULT_TRADE) || INV_CONTENT(ITEM_CLAIM_CHECK) == ITEM_CLAIM_CHECK)) {
        CustomMessage msg =
            CustomMessages::CreateMessage("randomizer.messages.merchant_messages.build_granny_message.0");
        BuildMerchantMessage(msg, RC_KAK_GRANNYS_SHOP,
                             !RAND_GET_OPTION(RSK_MERCHANT_TEXT_HINT) ||
                                 CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("MysteriousShuffle"), 0));
        msg.AutoFormat();
        msg.LoadIntoFont();
        *loadFromMessageTable = false;
    }
}

void BuildCarpetGuyMessage(uint16_t* textId, bool* loadFromMessageTable) {
    CustomMessage msg;
    if (*textId == TEXT_CARPET_SALESMAN_ARMS_DEALER) {
        msg = CustomMessages::CreateMessage("randomizer.messages.merchant_messages.build_carpet_guy_message.0");
    } else if (!Flags_GetRandomizerInf(RAND_INF_MERCHANTS_CARPET_SALESMAN)) {
        msg = CustomMessages::CreateMessage("randomizer.messages.merchant_messages.build_carpet_guy_message.1");
        BuildMerchantMessage(msg, RC_WASTELAND_BOMBCHU_SALESMAN,
                             !RAND_GET_OPTION(RSK_MERCHANT_TEXT_HINT) ||
                                 CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("MysteriousShuffle"), 0));
    } else {
        return;
    }
    msg.AutoFormat();
    msg.LoadIntoFont();
    *loadFromMessageTable = false;
}

void BuildCarpetGuyFailToBuyMessage(uint16_t* textId, bool* loadFromMessageTable) {
    CustomMessage msg =
        CustomMessages::CreateMessage("randomizer.messages.merchant_messages.build_carpet_guy_fail_to_buy_message.0");
    msg.AutoFormat();
    msg.LoadIntoFont();
    *loadFromMessageTable = false;
}

void BuildScrubMessage(uint16_t* textId, bool* loadFromMessageTable) {
    EnDns* enDns = reinterpret_cast<EnDns*>(GET_PLAYER(gPlayState)->talkActor);
    RandomizerCheck rc = ObjectExtension::GetInstance().Get<ScrubIdentity>(enDns)->identity.randomizerCheck;
    uint16_t price = RAND_GET_ITEM(rc)->GetPrice();
    CustomMessage msg;
    if (price == 0) {
        msg = CustomMessages::CreateMessage("randomizer.messages.merchant_messages.build_scrub_message.0");
    } else {
        msg = CustomMessages::CreateMessage("randomizer.messages.merchant_messages.build_scrub_message.1");
    }
    BuildMerchantMessage(msg, rc,
                         !RAND_GET_OPTION(RSK_SCRUB_TEXT_HINT) ||
                             CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("MysteriousShuffle"), 0));
    msg.AutoFormat();
    msg.LoadIntoFont();
    *loadFromMessageTable = false;
}

void BuildShopMessage(uint16_t* textId, bool* loadFromMessageTable) {
    CustomMessage msg;
    RandomizerCheck rc;
    // Shop items each have two message entries, second one offset by NUM_SHOP_ITEMS
    // textId: TEXT_SHOP_ITEM_RANDOM + (randomizerInf - RAND_INF_SHOP_ITEMS_KF_SHOP_ITEM_1)
    // textId: TEXT_SHOP_ITEM_RANDOM + ((randomizerInf - RAND_INF_SHOP_ITEMS_KF_SHOP_ITEM_1) + NUM_SHOP_ITEMS)
    if (*textId >= TEXT_SHOP_ITEM_RANDOM && *textId < TEXT_SHOP_ITEM_RANDOM_CONFIRM) {
        rc = OTRGlobals::Instance->gRandomizer->GetCheckFromRandomizerInf(
            static_cast<RandomizerInf>((*textId - TEXT_SHOP_ITEM_RANDOM) + RAND_INF_SHOP_ITEMS_KF_SHOP_ITEM_1));
        msg = CustomMessages::CreateMessage("randomizer.messages.merchant_messages.build_shop_message.0");
    } else if (*textId >= TEXT_SHOP_ITEM_RANDOM_CONFIRM && *textId <= TEXT_SHOP_ITEM_RANDOM_CONFIRM_END) {
        rc = OTRGlobals::Instance->gRandomizer->GetCheckFromRandomizerInf(
            static_cast<RandomizerInf>((*textId - TEXT_SHOP_ITEM_RANDOM_CONFIRM) + RAND_INF_SHOP_ITEMS_KF_SHOP_ITEM_1));
        msg = CustomMessages::CreateMessage("randomizer.messages.merchant_messages.build_shop_message.1");
    } else {
        return;
    }
    BuildMerchantMessage(msg, rc, CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("MysteriousShuffle"), 0));
    msg.AutoFormat();
    msg.LoadIntoFont();
    *loadFromMessageTable = false;
}

void RegisterMerchantMessages() {
    COND_ID_HOOK(OnOpenText, TEXT_BEAN_SALESMAN_BUY_FOR_10,
                 (RAND_GET_OPTION(RSK_SHUFFLE_MERCHANTS).Is(RO_SHUFFLE_MERCHANTS_BEANS_ONLY) ||
                  RAND_GET_OPTION(RSK_SHUFFLE_MERCHANTS).Is(RO_SHUFFLE_MERCHANTS_ALL)),
                 BuildBeanGuyMessage);
    COND_ID_HOOK(OnOpenText, TEXT_BEAN_SALESMAN_BUY_FOR_100, IS_RANDO, BuildBeanGuyMessage);
    COND_ID_HOOK(OnOpenText, TEXT_MEDIGORON, NON_BEAN_MERCHANTS, BuildMedigoronMessage);
    COND_ID_HOOK(OnOpenText, TEXT_GRANNYS_SHOP, NON_BEAN_MERCHANTS, BuildGrannyMessage);
    COND_ID_HOOK(OnOpenText, TEXT_CARPET_SALESMAN_1, NON_BEAN_MERCHANTS, BuildCarpetGuyMessage);
    COND_ID_HOOK(OnOpenText, TEXT_CARPET_SALESMAN_ARMS_DEALER, NON_BEAN_MERCHANTS, BuildCarpetGuyMessage);
    COND_ID_HOOK(OnOpenText, TEXT_SCRUB_RANDOM, IS_RANDO, BuildScrubMessage);
    COND_HOOK(OnOpenText, RAND_GET_OPTION(RSK_SHOPSANITY), BuildShopMessage);
    COND_ID_HOOK(OnOpenText, TEXT_CARPET_SALESMAN_CUSTOM_FAIL_TO_BUY, IS_RANDO, BuildCarpetGuyFailToBuyMessage);
}

static RegisterShipInitFunc initFunc(RegisterMerchantMessages, { "IS_RANDO" });