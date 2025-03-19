#include "functions.h"
#include "variables.h"
#include "macros.h"

u8 Randomizer_GetNextChildTradeItem() {
    const u8 numTradeItems = ITEM_MASK_TRUTH - ITEM_WEIRD_EGG + 1;
    u8 currentTradeItemIndex = INV_CONTENT(ITEM_TRADE_CHILD) - ITEM_WEIRD_EGG;
    for (int i = 0; i < numTradeItems; i++) {
        u8 tradeIndex = (currentTradeItemIndex + i + 1) % numTradeItems;
        if (Flags_GetRandomizerInf(tradeIndex + RAND_INF_CHILD_TRADES_HAS_WEIRD_EGG)) {
			return ITEM_WEIRD_EGG + tradeIndex;
        }
    }
    return ITEM_NONE;
}

u8 Randomizer_GetPrevChildTradeItem() {
    const u8 numTradeItems = ITEM_MASK_TRUTH - ITEM_WEIRD_EGG + 1;
    u8 currentTradeItemIndex = INV_CONTENT(ITEM_TRADE_CHILD) - ITEM_WEIRD_EGG;
    for (int i = 0; i < numTradeItems; i++) {
        u8 tradeIndex = (currentTradeItemIndex - i - 1 + numTradeItems) % numTradeItems;
        if (Flags_GetRandomizerInf(tradeIndex + RAND_INF_CHILD_TRADES_HAS_WEIRD_EGG)) {
			return ITEM_WEIRD_EGG + tradeIndex;
        }
    }
    return ITEM_NONE;
}

u8 Randomizer_GetNextAdultTradeItem() {
    const u8 numTradeItems = ITEM_CLAIM_CHECK - ITEM_POCKET_EGG + 1;
    u8 currentTradeItemIndex = INV_CONTENT(ITEM_TRADE_ADULT) - ITEM_POCKET_EGG;
    for (int i = 0; i < numTradeItems; i++) {
        u8 tradeIndex = (currentTradeItemIndex + i + 1) % numTradeItems;
        if (Flags_GetRandomizerInf(tradeIndex + RAND_INF_ADULT_TRADES_HAS_POCKET_EGG)) {
			return ITEM_POCKET_EGG + tradeIndex;
        }
    }
    return ITEM_NONE;
}

u8 Randomizer_GetPrevAdultTradeItem() {
    const u8 numTradeItems = ITEM_CLAIM_CHECK - ITEM_POCKET_EGG + 1;
    u8 currentTradeItemIndex = INV_CONTENT(ITEM_TRADE_ADULT) - ITEM_POCKET_EGG;
    for (int i = 0; i < numTradeItems; i++) {
        u8 tradeIndex = (currentTradeItemIndex - i - 1 + numTradeItems) % numTradeItems;
        if (Flags_GetRandomizerInf(tradeIndex + RAND_INF_ADULT_TRADES_HAS_POCKET_EGG)) {
			return ITEM_POCKET_EGG + tradeIndex;
        }
    }
    return ITEM_NONE;
}
