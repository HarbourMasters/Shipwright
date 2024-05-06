#ifndef Z_ITEM_B_HEART_H
#define Z_ITEM_B_HEART_H

#include <libultraship/libultra.h>
#include "global.h"

struct ItemBHeart;

typedef struct ItemBHeart {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ char unk_14C[0xC];
    /* 0x0158 */ f32 unk_158;
    /* 0x015C */ char unk_15C[0x8];
    /* 0x0164 */ s16 unk_164;
    /* 0x0166 */ char unk_166[0x6];
    // #region SOH [Randomizer] Cached for drawing for performance
    /*        */ GetItemEntry sohItemEntry;
    // #endregion
} ItemBHeart; // size = 0x016C

void func_80B85264(ItemBHeart* itemBHeart, PlayState* play);

#endif
