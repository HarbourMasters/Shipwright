// RANDOTODO: Consider breaking this up into the different actors
// That speak these hints, like 2Ship does with ActorBehavior

/**
 * This file handles the custom messages for merchants,
 * including both shops and one-off merchants (i.e. Medigoron, Bean Guy,
 * and Carpet Salesman)
 */
#include <soh/OTRGlobals.h>
#include "soh/ObjectExtension/ObjectExtension.h"

extern "C" {
extern PlayState* gPlayState;
#include <macros.h>
#include <functions.h>
#include <variables.h>
#include <overlays/actors/ovl_En_Dns/z_en_dns.h>
}

#define RAND_GET_ITEM(rc) OTRGlobals::Instance->gRandoContext->GetItemLocation(rc)
#define RAND_GET_OVERRIDE(rc) OTRGlobals::Instance->gRandoContext->overrides[rc]
#define NON_BEAN_MERCHANTS                                                            \
    (RAND_GET_OPTION(RSK_SHUFFLE_MERCHANTS).Is(RO_SHUFFLE_MERCHANTS_ALL_BUT_BEANS) || \
     RAND_GET_OPTION(RSK_SHUFFLE_MERCHANTS).Is(RO_SHUFFLE_MERCHANTS_ALL))

void BuildMerchantMessage(CustomMessage& msg, RandomizerCheck rc, bool mysterious = true) {
    RandomizerGet rgid = RAND_GET_ITEM(rc)->GetPlacedRandomizerGet();
    uint16_t price = RAND_GET_ITEM(rc)->GetPrice();
    CustomMessage itemName;
    std::string color = Rando::StaticData::RetrieveItem(static_cast<RandomizerGet>(rgid)).GetColor();
    if (mysterious) {
        itemName = Rando::StaticData::hintTextTable[RHT_MYSTERIOUS_ITEM_CAPITAL].GetHintMessage();
        color = "%g";
    } else if (rgid == RG_ICE_TRAP) {
        rgid = RAND_GET_OVERRIDE(rc).LooksLike();
        itemName = CustomMessage(RAND_GET_OVERRIDE(rc).GetTrickName());
        color = "%g";
    } else {
        itemName = CustomMessage(Rando::StaticData::RetrieveItem(rgid).GetName());
    }
    msg.Replace("[[color]]", color);
    msg.InsertNames({ itemName, CustomMessage(std::to_string(price)) });
}

void BuildBeanGuyMessage(uint16_t* textId, bool* loadFromMessageTable) {
    CustomMessage msg;
    if (*textId == TEXT_BEAN_SALESMAN_BUY_FOR_100) {
        msg = CustomMessage(
            "I never thought I'd say this, but I'm selling the last %rMagic Bean%w.^%y99 Rupees%w, no "
            "less.\x1B%gYes&No%w",
            "Ich hätte nie gedacht, daß ich das sage, aber ich verkaufe die letzte^%rWundererbse%w für %y99 "
            "Rubine%w.\x1B&%gJa&Nein%w",
            "Je te vends mon dernier %rHaricot&magique%g pour %y99 Rubis%w.\x1B&%gAcheterNe pas acheter%w");
        msg.Format();
    } else if (*textId == TEXT_BEAN_SALESMAN_BUY_FOR_10) {
        msg = CustomMessage("Want to buy [[color]][[1]]%w for %y[[2]] Rupees%w?\x1B%gYes&No%w",
                            "Möchten Sie [[color]][[1]]%w für %y[[2]] Rubin%w kaufen?\x1B%gJa&Nein%w",
                            "Voulez-vous acheter [[color]][[1]]%w pour %y[[2]] Rubis%w?\x1B%gOui&Non%w");
        BuildMerchantMessage(msg, RC_ZR_MAGIC_BEAN_SALESMAN,
                             !RAND_GET_OPTION(RSK_MERCHANT_TEXT_HINT) ||
                                 CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("MysteriousShuffle"), 0));
        msg.AutoFormat();
    }
    msg.LoadIntoFont();
    *loadFromMessageTable = false;
}

void BuildMedigoronMessage(uint16_t* textId, bool* loadFromMessageTable) {
    CustomMessage msg = CustomMessage("Want to buy [[color]][[1]]%w for %y[[2]] Rupees%w?\x1B%gYes&No%w",
                                      "Möchten Sie [[color]][[1]]%w für %y[[2]] Rubin%w kaufen?\x1B%gJa&Nein%w",
                                      "Voulez-vous acheter [[color]][[1]]%w pour %y[[2]] Rubis%w?\x1B%gOui&Non%w");
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
        CustomMessage msg = CustomMessage("Want to buy [[color]][[1]]%w for %y[[2]] Rupees%w?\x1B%gYes&No%w",
                                          "Möchten Sie [[color]][[1]]%w für %y[[2]] Rubin%w kaufen?\x1B%gJa&Nein%w",
                                          "Voulez-vous acheter [[color]][[1]]%w pour %y[[2]] Rubis%w?\x1B%gOui&Non%w");
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
        msg = CustomMessage("Finally! Now I can go back to being an %rarms dealer%w!",
                            /*german*/ "Endlich! Schon bald kann ich wieder %rKrabbelminen-Händler%w sein!",
                            /*french*/ "Squalala! Je vais enfin pouvoir %rprendre des vacances%w!");
    } else if (!Flags_GetRandomizerInf(RAND_INF_MERCHANTS_CARPET_SALESMAN)) {
        msg = CustomMessage("Want to buy [[color]][[1]]%w for %y[[2]] Rupees%w?\x1B%gYes&No%w",
                            "Möchten Sie [[color]][[1]]%w für %y[[2]] Rubin%w kaufen?\x1B%gJa&Nein%w",
                            "Voulez-vous acheter [[color]][[1]]%w pour %y[[2]] Rubis%w?\x1B%gOui&Non%w");
        BuildMerchantMessage(msg, RC_WASTELAND_BOMBCHU_SALESMAN,
                             !RAND_GET_OPTION(RSK_MERCHANT_TEXT_HINT) ||
                                 CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("MysteriousShuffle"), 0));
    }
    msg.AutoFormat();
    msg.LoadIntoFont();
    *loadFromMessageTable = false;
}

void BuildCarpetGuyFailToBuyMessage(uint16_t* textId, bool* loadFromMessageTable) {
    CustomMessage msg =
        CustomMessage("I'm sorry I can't sell you these fine specimens, they need an %rexperienced owner%w.^"
                      "Come back when you have had %gBombchus%w of your own.");
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
        msg = CustomMessage("\x12\x38\x82"
                            "All right! You win! In return for sparing me, I will give you a [[color]][[1]]%w!&Please, "
                            "take it!\x07\x10\xA3",
                            "\x12\x38\x82"
                            "In Ordnung! Du gewinnst! Im Austausch dafür, dass Du mich verschont hast, werde ich Dir "
                            "einen [[color]][[1]]%w geben!\x07\x10\xA3",
                            "\x12\x38\x82"
                            "J'me rends! Laisse-moi partir et en échange, je te donne un [[color]][[1]]%w! Vas-y "
                            "prends le!\x07\x10\xA3");
    } else {
        msg = CustomMessage(
            "\x12\x38\x82"
            "All right! You win! In return for sparing me, I will sell you a [[color]][[1]]%w! %y[[2]] Rupees%w it "
            "is!\x07\x10\xA3",
            "\x12\x38\x82"
            "Ich gebe auf! Ich verkaufe Dir einen [[color]][[1]]%w für %y[[2]] Rubine%w!\x07\x10\xA3",
            "\x12\x38\x82"
            "J'abandonne! Tu veux bien m'acheter un [[color]][[1]]%w? Ça fera %y[[2]] Rubis%w!\x07\x10\xA3");
    }
    BuildMerchantMessage(msg, rc,
                         !RAND_GET_OPTION(RSK_MERCHANT_TEXT_HINT) ||
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
        msg =
            CustomMessage("\x08[[color]][[1]]%w  %y[[2]]_Rupees%w&Special deal! %rONE LEFT%w!\x0A\x02",
                          "\x08[[color]][[1]]%w  %y[[2]]_Rubine%w&Sonderangebot! %rNUR NOCH EINES VERFÜGBAR%w!\x0A\x02",
                          "\x08[[color]][[1]]%w  %y[[2]]_Rubis%w&Offre spéciale! %rDERNIER EN STOCK%w!\x0A\x02");
    } else if (*textId >= TEXT_SHOP_ITEM_RANDOM_CONFIRM && *textId <= TEXT_SHOP_ITEM_RANDOM_CONFIRM_END) {
        rc = OTRGlobals::Instance->gRandomizer->GetCheckFromRandomizerInf(
            static_cast<RandomizerInf>((*textId - TEXT_SHOP_ITEM_RANDOM_CONFIRM) + RAND_INF_SHOP_ITEMS_KF_SHOP_ITEM_1));
        msg = CustomMessage("\x08[[color]][[1]]%w  %y[[2]]_Rupees%w\x09\x1B%gBuy&Don't buy%w\x09\x02",
                            "\x08[[color]][[1]]%w  %y[[2]]_Rubine%w\x09\x1B%gKaufen&Nicht kaufen%w\x09\x02",
                            "\x08[[color]][[1]]%w  %y[[2]]_Rubis%w\x09\x1B%gAcheter&Ne pas acheter%w\x09\x02");
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