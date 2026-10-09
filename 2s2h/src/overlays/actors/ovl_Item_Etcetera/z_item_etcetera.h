#ifndef Z_ITEM_ETCETERA_H
#define Z_ITEM_ETCETERA_H

#include "global.h"

struct ItemEtcetera;

#define ITEMETCETERA_GET_FF(thisx) ((thisx)->params & 0xFF)
#define ITEMETCETERA_GET_TREASUREFLAG(thisx) (((thisx)->params >> 8) & 0x1F)

typedef void (*ItemEtceteraActionFunc)(struct ItemEtcetera*, PlayState*);

typedef enum {
    /* 0x0 */ ITEM_ETC_BOTTLE_1,
    /* 0x1 */ ITEM_ETC_BOTTLE_2,
    /* 0x2 */ ITEM_ETC_BOTTLE_3,
    /* 0x3 */ ITEM_ETC_BOTTLE_4,
    /* 0x4 */ ITEM_ETC_BOTTLE_5,
    /* 0x5 */ ITEM_ETC_BOTTLE_6,
    /* 0x6 */ ITEM_ETC_KEY_SMALL,
    /* 0x7 */ ITEM_ETC_ARROW_FIRE,
    /* 0x8 */ ITEM_ETC_RUPEE_GREEN_CHEST_GAME,
    /* 0x9 */ ITEM_ETC_RUPEE_BLUE_CHEST_GAME,
    /* 0xA */ ITEM_ETC_RUPEE_RED_CHEST_GAME,
    /* 0xB */ ITEM_ETC_RUPEE_PURPLE_CHEST_GAME,
    /* 0xC */ ITEM_ETC_HEART_PIECE_CHEST_GAME,
    /* 0xD */ ITEM_ETC_KEY_SMALL_CHEST_GAME
} ItemEtceteraType;

typedef struct ItemEtcetera {
    /* 0x000 */ Actor actor;
    /* 0x144 */ ItemEtceteraActionFunc futureActionFunc;
    /* 0x148 */ s16 getItemDrawId;
    /* 0x14A */ s16 getItemId;
    /* 0x14C */ u8 objectSlot;
    /* 0x150 */ ActorFunc drawFunc;
    /* 0x154 */ ItemEtceteraActionFunc actionFunc;
} ItemEtcetera; // size = 0x158

#endif // Z_ITEM_ETCETERA_H
