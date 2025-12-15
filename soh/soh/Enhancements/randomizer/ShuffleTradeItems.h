#ifndef Z_ADULT_TRADE_SHUFFLE_H
#define Z_ADULT_TRADE_SHUFFLE_H

#include <libultraship/libultra/types.h>

#ifdef __cplusplus
extern "C" {
#endif
u8 Randomizer_GetNextChildTradeItem();
u8 Randomizer_GetPrevChildTradeItem();
u8 Randomizer_GetNextAdultTradeItem();
u8 Randomizer_GetPrevAdultTradeItem();
#ifdef __cplusplus
}
#endif

#endif
