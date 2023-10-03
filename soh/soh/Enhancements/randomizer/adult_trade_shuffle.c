#include "soh/Enhancements/randomizer/adult_trade_shuffle.h"
#include "functions.h"
#include "variables.h"
#include "macros.h"
#ifdef ENABLE_REMOTE_CONTROL
#include "soh/Enhancements/game-interactor/GameInteractor_Anchor.h"
#endif

void Randomizer_ConsumeAdultTradeItem(PlayState* play, u8 itemId) {
    gSaveContext.adultTradeItems &= ~ADULT_TRADE_FLAG(itemId);
	Inventory_ReplaceItem(play, itemId, Randomizer_GetNextAdultTradeItem());
#ifdef ENABLE_REMOTE_CONTROL
    Anchor_ConsumeAdultTradeItem(itemId);
#endif
}

u8 Randomizer_GetNextAdultTradeItem() {
    const u8 numTradeItems = ITEM_CLAIM_CHECK - ITEM_POCKET_EGG + 1;
    u8 currentTradeItemIndex = INV_CONTENT(ITEM_TRADE_ADULT) - ITEM_POCKET_EGG;
    for (int i = 0; i < numTradeItems; i++) {
        u8 tradeIndex = (currentTradeItemIndex + i + 1) % numTradeItems;
        if (gSaveContext.adultTradeItems & (1 << tradeIndex)) {
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
        if (gSaveContext.adultTradeItems & (1 << tradeIndex)) {
			return ITEM_POCKET_EGG + tradeIndex;
        }
    }
    return ITEM_NONE;
}
