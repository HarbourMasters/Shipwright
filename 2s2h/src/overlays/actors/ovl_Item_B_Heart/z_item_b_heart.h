#ifndef Z_ITEM_B_HEART_H
#define Z_ITEM_B_HEART_H

#include "global.h"

typedef enum {
    /*  0 */ BHEART_PARAM_NORMAL,    // Spawn Parameter for Normal Scale.
    /* 35 */ BHEART_PARAM_SMALL = 35 // Spawn Parameter for Small Scale.
} ItemBHeartParam;

#define BHEART_SCALE_SMALL (0.1f)           // Scale value for Small Mode
#define BHEART_SCALE_NORMAL (1.0f)          // Scale value for Normal Mode
#define BHEART_SCALE_MIN_COLLECTIBLE (0.5f) // baseScale <50% will disable collection

typedef struct ItemBHeart {
    /* 0x000 */ Actor actor;
    /* 0x144 */ UNK_TYPE1 unk_144[0x20];
    /* 0x164 */ f32 variableScale;
    /* 0x168 */ f32 baseScale;
} ItemBHeart; // size = 0x16C

#endif // Z_ITEM_B_HEART_H
