#ifndef Z_ADULT_TRADE_SHUFFLE_H
#define Z_ADULT_TRADE_SHUFFLE_H

#include <z64.h>

#define ADULT_TRADE_FLAG(itemId) (1 << (itemId - ITEM_POCKET_EGG))
#define PLAYER_HAS_SHUFFLED_ADULT_TRADE_ITEM(itemID) (gSaveContext.adultTradeItems & ADULT_TRADE_FLAG(itemID))

#ifdef __cplusplus
extern "C" {
#endif

void Randomizer_ConsumeAdultTradeItem(PlayState* play, u8 itemId);
u8 Randomizer_GetNextAdultTradeItem();
u8 Randomizer_GetPrevAdultTradeItem();

#ifdef __cplusplus
}
#endif

#endif
