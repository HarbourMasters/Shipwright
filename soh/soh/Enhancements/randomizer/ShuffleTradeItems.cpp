#include "soh/Enhancements/randomizer/context.h"
#include "soh/Enhancements/randomizer/randomizerTypes.h"
#include "soh/OTRGlobals.h"
#include "functions.h"
extern "C" {
#include "variables.h"
#include "macros.h"
}

// RANDOTODO: Migrate other aspects of adult trade shuffle to hooks here.

void RegisterAdultTradeShuffle() {
    SHOULD_SHUFFLE_LOCATION({
        if (location->GetRCType() == RCTYPE_ADULT_TRADE) {
            if (RAND_GET_OPTION(RSK_SHUFFLE_ADULT_TRADE) == RO_GENERIC_OFF) {
                *should = false;
            }
        }
    })
}

static RegisterShipInitFunc registerAdultTradeShuffle(RegisterAdultTradeShuffle, { "IS_RANDO" });

extern "C" u8 Randomizer_GetNextChildTradeItem() {
    const u8 numTradeItems = ITEM_MASK_TRUTH - ITEM_WEIRD_EGG + 1;
    u8 currentTradeItemIndex = INV_CONTENT(ITEM_TRADE_CHILD) - ITEM_WEIRD_EGG;
    for (int i = 0; i < numTradeItems; i++) {
        u8 tradeIndex = (currentTradeItemIndex + i + 1) % numTradeItems;
        if (Flags_GetRandomizerInf(static_cast<RandomizerInf>(tradeIndex + RAND_INF_CHILD_TRADES_HAS_WEIRD_EGG))) {
            return ITEM_WEIRD_EGG + tradeIndex;
        }
    }
    return ITEM_NONE;
}

extern "C" u8 Randomizer_GetPrevChildTradeItem() {
    const u8 numTradeItems = ITEM_MASK_TRUTH - ITEM_WEIRD_EGG + 1;
    u8 currentTradeItemIndex = INV_CONTENT(ITEM_TRADE_CHILD) - ITEM_WEIRD_EGG;
    for (int i = 0; i < numTradeItems; i++) {
        u8 tradeIndex = (currentTradeItemIndex - i - 1 + numTradeItems) % numTradeItems;
        if (Flags_GetRandomizerInf(static_cast<RandomizerInf>(tradeIndex + RAND_INF_CHILD_TRADES_HAS_WEIRD_EGG))) {
            return ITEM_WEIRD_EGG + tradeIndex;
        }
    }
    return ITEM_NONE;
}

extern "C" u8 Randomizer_GetNextAdultTradeItem() {
    const u8 numTradeItems = ITEM_CLAIM_CHECK - ITEM_POCKET_EGG + 1;
    u8 currentTradeItemIndex = INV_CONTENT(ITEM_TRADE_ADULT) - ITEM_POCKET_EGG;
    for (int i = 0; i < numTradeItems; i++) {
        u8 tradeIndex = (currentTradeItemIndex + i + 1) % numTradeItems;
        if (Flags_GetRandomizerInf(static_cast<RandomizerInf>(tradeIndex + RAND_INF_CHILD_TRADES_HAS_WEIRD_EGG))) {
            return ITEM_POCKET_EGG + tradeIndex;
        }
    }
    return ITEM_NONE;
}

extern "C" u8 Randomizer_GetPrevAdultTradeItem() {
    const u8 numTradeItems = ITEM_CLAIM_CHECK - ITEM_POCKET_EGG + 1;
    u8 currentTradeItemIndex = INV_CONTENT(ITEM_TRADE_ADULT) - ITEM_POCKET_EGG;
    for (int i = 0; i < numTradeItems; i++) {
        u8 tradeIndex = (currentTradeItemIndex - i - 1 + numTradeItems) % numTradeItems;
        if (Flags_GetRandomizerInf(static_cast<RandomizerInf>(tradeIndex + RAND_INF_CHILD_TRADES_HAS_WEIRD_EGG))) {
            return ITEM_POCKET_EGG + tradeIndex;
        }
    }
    return ITEM_NONE;
}
