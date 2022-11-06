#ifndef Z_ITEM_ETC_H
#define Z_ITEM_ETC_H

#include "ultra64.h"
#include "global.h"

struct ItemEtcetera;

typedef void (*ItemEtceteraActionFunc)(struct ItemEtcetera*, PlayState*);

typedef struct ItemEtcetera {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ ItemEtceteraActionFunc futureActionFunc;
    /* 0x0150 */ s16 giDrawId;
    /* 0x0152 */ s16 getItemId;
    /* 0x0154 */ u8 objBankIndex;
    /* 0x0158 */ ActorFunc drawFunc;
    /* 0x015C */ ItemEtceteraActionFunc actionFunc;
} ItemEtcetera; // size = 0x0160

typedef enum {
    /* 0x00 */ ITEM_ETC_BOTTLE,
    /* 0x01 */ ITEM_ETC_LETTER,
    /* 0x02 */ ITEM_ETC_SHIELD_HYLIAN,
    /* 0x03 */ ITEM_ETC_QUIVER,
    /* 0x04 */ ITEM_ETC_SCALE_SILVER,
    /* 0x05 */ ITEM_ETC_SCALE_GOLD,
    /* 0x06 */ ITEM_ETC_KEY_SMALL,
    /* 0x07 */ ITEM_ETC_ARROW_FIRE,
    /* 0x08 */ ITEM_ETC_RUPEE_GREEN_CHEST_GAME,
    /* 0x09 */ ITEM_ETC_RUPEE_BLUE_CHEST_GAME,
    /* 0x0A */ ITEM_ETC_RUPEE_RED_CHEST_GAME,
    /* 0x0B */ ITEM_ETC_RUPEE_PURPLE_CHEST_GAME,
    /* 0x0C */ ITEM_ETC_HEART_PIECE_CHEST_GAME,
    /* 0x0D */ ITEM_ETC_KEY_SMALL_CHEST_GAME
} ItemEtceteraType;

#endif
