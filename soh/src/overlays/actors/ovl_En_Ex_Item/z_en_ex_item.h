#ifndef Z_EN_EX_ITEM_H
#define Z_EN_EX_ITEM_H

#include "ultra64.h"
#include "global.h"

struct EnExItem;

typedef void (*EnExItemActionFunc)(struct EnExItem* this, PlayState* play);
typedef void (*EnExItemLightFunc)(Actor*, PlayState*, s32);

typedef struct EnExItem {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ EnExItemActionFunc actionFunc;
    /* 0x0150 */ s16 getItemObjId;
    /* 0x0152 */ s16 type;
    /* 0x0152 */ s16 unusedParam;
    /* 0x0156 */ s16 giDrawId;
    /* 0x0158 */ s16 stopRotate;
    /* 0x015A */ s16 timer;
    /* 0x015A */ s16 chestKillTimer;
    /* 0x015A */ s16 prizeRotateTimer;
    /* 0x0160 */ s16 killItem;
    /* 0x0164 */ f32 scale;
    /* 0x0168 */ f32 unkFloat; // set to the same value as scale, but unused
    /* 0x016C */ s8 objectIdx;
    /* 0x0170 */ Vec3f initPos; // unused
    /* 0x017C */ EnExItemLightFunc unk_17C;
    /* 0x0180 */ EnExItemLightFunc unk_180;
} EnExItem; // size = 0x0184

typedef enum {
    /*  0 */ EXITEM_BOMB_BAG_BOWLING,
    /*  1 */ EXITEM_HEART_PIECE_BOWLING,
    /*  2 */ EXITEM_BOMBCHUS_BOWLING,
    /*  3 */ EXITEM_BOMBS_BOWLING,
    /*  4 */ EXITEM_PURPLE_RUPEE_BOWLING,
    /*  5 */ EXITEM_BOMB_BAG_COUNTER,
    /*  6 */ EXITEM_HEART_PIECE_COUNTER,
    /*  7 */ EXITEM_BOMBCHUS_COUNTER,
    /*  8 */ EXITEM_BOMBS_COUNTER,
    /*  9 */ EXITEM_PURPLE_RUPEE_COUNTER,
    /* 10 */ EXITEM_GREEN_RUPEE_CHEST,
    /* 11 */ EXITEM_BLUE_RUPEE_CHEST,
    /* 12 */ EXITEM_RED_RUPEE_CHEST,
    /* 13 */ EXITEM_13,
    /* 14 */ EXITEM_14,
    /* 15 */ EXITEM_SMALL_KEY_CHEST,
    /* 16 */ EXITEM_MAGIC_FIRE,
    /* 17 */ EXITEM_MAGIC_WIND,
    /* 18 */ EXITEM_MAGIC_DARK,
    /* 19 */ EXITEM_BULLET_BAG
} EnExItemType;

#define EXITEM_COUNTER 5
#define EXITEM_CHEST 10
#define EXITEM_MAGIC 16

#endif
