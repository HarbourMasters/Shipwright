#include <soh/OTRGlobals.h>

extern "C" {
extern PlayState* gPlayState;
#include <macros.h>
#include <functions.h>
#include <variables.h>
}

#define RAND_GET_OPTION(rsk) OTRGlobals::Instance->gRandoContext->GetOption(rsk)
#define RAND_GET_ITEM(rc) OTRGlobals::Instance->gRandoContext->GetItemLocation(rc)
#define RAND_GET_OVERRIDE(rc) OTRGlobals::Instance->gRandoContext->overrides[rc]
#define NON_BEAN_MERCHANTS (RAND_GET_OPTION(RSK_SHUFFLE_MERCHANTS).Is(RO_SHUFFLE_MERCHANTS_ALL_BUT_BEANS) || \
                            RAND_GET_OPTION(RSK_SHUFFLE_MERCHANTS).Is(RO_SHUFFLE_MERCHANTS_ALL))

void BuildMerchantMessage(CustomMessage& msg, RandomizerCheck rc) {
    RandomizerGet rgid = RAND_GET_ITEM(rc)->GetPlacedRandomizerGet();
    uint16_t price = RAND_GET_ITEM(rc)->GetPrice();
    CustomMessage itemName;
    bool mysterious = RAND_GET_OPTION(RSK_MERCHANT_TEXT_HINT).Is(RO_GENERIC_OFF) ||
        CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("MysteriousShuffle"), 0);
    if (mysterious) {
        itemName = Rando::StaticData::hintTextTable[RHT_MYSTERIOUS_ITEM_CAPITAL].GetHintMessage();
    } else if (rgid == RG_ICE_TRAP) {
        rgid = RAND_GET_OVERRIDE(rc).LooksLike();
        itemName = CustomMessage(RAND_GET_OVERRIDE(rc).GetTrickName());
    } else {
        itemName = CustomMessage(Rando::StaticData::RetrieveItem(rgid).GetName());
    }
    msg.InsertNames({ itemName , CustomMessage(std::to_string(price))});
}

void BuildBeanGuyMessage(uint16_t* textId, bool* loadFromMessageTable) {
    CustomMessage msg;
    if (*textId == TEXT_BEAN_SALESMAN_BUY_FOR_100) {
        msg = CustomMessage(
            "I never thought I'd say this, but I'm selling the last %rMagic Bean%w.^%y99 Rupees%w, no less.\x1B%gYes&No%w",
            "Ich hätte nie gedacht, daß ich das sage, aber ich verkaufe die letzte^%rWundererbse%w für %y99 Rubine%w.\x1B&%gJa&Nein%w",
            "Je te vends mon dernier %rHaricot&magique%g pour %y99 Rubis%w.\x1B&%gAcheterNe pas acheter%w"
        );
        msg.Format();
    } else if (*textId == TEXT_BEAN_SALESMAN_BUY_FOR_10) {
        msg = CustomMessage(
            "Want to buy %g[[1]]%w for %y[[2]] Rupees%w?\x1B%gYes&No%w",
            "Möchten Sie %g[[1]]%w für %y[[2]] Rubin%w kaufen?\x1B%gJa&Nein%w",
            "Voulez-vous acheter %g[[1]]%w pour %y[[2]] Rubis%w?\x1B%gOui&Non%w"
        );
        BuildMerchantMessage(msg, RC_ZR_MAGIC_BEAN_SALESMAN);
        msg.AutoFormat();
    }
    msg.LoadIntoFont();
    *loadFromMessageTable = false;
}

void BuildMedigoronMessage(uint16_t* textId, bool* loadFromMessageTable) {
    CustomMessage msg = CustomMessage(
        "Want to buy %g[[1]]%w for %y[[2]] Rupees%w?\x1B%gYes&No%w",
        "Möchten Sie %g[[1]]%w für %y[[2]] Rubin%w kaufen?\x1B%gJa&Nein%w",
        "Voulez-vous acheter %g[[1]]%w pour %y[[2]] Rubis%w?\x1B%gOui&Non%w"
    );
    BuildMerchantMessage(msg, RC_GC_MEDIGORON);
    msg.AutoFormat();
    msg.LoadIntoFont();
    *loadFromMessageTable = false;
}

void BuildGrannyMessage(uint16_t* textId, bool* loadFromMessageTable) {
    if (!Flags_GetRandomizerInf(RAND_INF_MERCHANTS_GRANNYS_SHOP) &&
        (RAND_GET_OPTION(RSK_SHUFFLE_ADULT_TRADE).Is(RO_GENERIC_ON) || 
        INV_CONTENT(ITEM_CLAIM_CHECK) == ITEM_CLAIM_CHECK)) {
        CustomMessage msg = CustomMessage(
            "Want to buy %g[[1]]%w for %y[[2]] Rupees%w?\x1B%gYes&No%w",
            "Möchten Sie %g[[1]]%w für %y[[2]] Rubin%w kaufen?\x1B%gJa&Nein%w",
            "Voulez-vous acheter %g[[1]]%w pour %y[[2]] Rubis%w?\x1B%gOui&Non%w"
        );
        BuildMerchantMessage(msg, RC_KAK_GRANNYS_SHOP);
        msg.AutoFormat();
        msg.LoadIntoFont();
        *loadFromMessageTable = false;
    }
}

void BuildCarpetGuyMessage(uint16_t* textId, bool* loadFromMessageTable) {
    CustomMessage msg;
    if (*textId == TEXT_CARPET_SALESMAN_ARMS_DEALER) {
        msg = CustomMessage("Finally! Now I can go back to being an %rarms dealer%w!",
            /*german*/"Endlich! Schon bald kann ich wieder %rKrabbelminen-Händler%w sein!",
            /*french*/"Squalala! Je vais enfin pouvoir %rprendre des vacances%w!");
    } else if (!Flags_GetRandomizerInf(RAND_INF_MERCHANTS_CARPET_SALESMAN)) {
        msg = CustomMessage(
            "Want to buy %g[[1]]%w for %y[[2]] Rupees%w?\x1B%gYes&No%w",
            "Möchten Sie %g[[1]]%w für %y[[2]] Rubin%w kaufen?\x1B%gJa&Nein%w",
            "Voulez-vous acheter %g[[1]]%w pour %y[[2]] Rubis%w?\x1B%gOui&Non%w"
        );
        BuildMerchantMessage(msg, RC_WASTELAND_BOMBCHU_SALESMAN);
    }
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
}

static RegisterShipInitFunc initFunc(RegisterMerchantMessages, { "IS_RANDO" });