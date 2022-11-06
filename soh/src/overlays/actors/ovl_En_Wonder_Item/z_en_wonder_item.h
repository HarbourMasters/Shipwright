#ifndef Z_EN_WONDER_ITEM_H
#define Z_EN_WONDER_ITEM_H

#include "ultra64.h"
#include "global.h"

struct EnWonderItem;

typedef void (*EnWonderItemUpdateFunc)(struct EnWonderItem*, PlayState*);

typedef struct EnWonderItem {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ EnWonderItemUpdateFunc updateFunc;
    /* 0x0150 */ f32 unkHeight; // sets height of dummied out mode 4 
    /* 0x0154 */ s16 wonderMode;
    /* 0x0156 */ s16 itemDrop;
    /* 0x0158 */ s16 numTagPoints;
    /* 0x015A */ s16 dropCount;
    /* 0x015C */ s16 timer;
    /* 0x015E */ s16 tagFlags;
    /* 0x015A */ s16 tagCount;
    /* 0x0162 */ s16 switchFlag;
    /* 0x0164 */ char unk_164[4];
    /* 0x0168 */ s16 nextTag;
    /* 0x016A */ s16 timerMod;
    /* 0x016C */ Vec3f unkPos; // set to initial position by mode bomb soldier, then never used.
    /* 0x0178 */ char unk_178[8];
    /* 0x0180 */ ColliderCylinder collider;
    /* 0x01CC */ char unk_1CC[4];
} EnWonderItem; // size = 0x01D0

typedef enum {
    /* 0 */ WONDERITEM_MULTITAG_FREE,
    /* 1 */ WONDERITEM_TAG_POINT_FREE,
    /* 2 */ WONDERITEM_PROXIMITY_DROP,
    /* 3 */ WONDERITEM_INTERACT_SWITCH,
    /* 4 */ WONDERITEM_UNUSED,
    /* 5 */ WONDERITEM_MULTITAG_ORDERED,
    /* 6 */ WONDERITEM_TAG_POINT_ORDERED,
    /* 7 */ WONDERITEM_PROXIMITY_SWITCH,
    /* 8 */ WONDERITEM_BOMB_SOLDIER,
    /* 9 */ WONDERITEM_ROLL_DROP
} EnWonderItemMode;

typedef enum {
    /* 0 */ WONDERITEM_DROP_NUTS,
    /* 1 */ WONDERITEM_DROP_HEART_PIECE,
    /* 2 */ WONDERITEM_DROP_MAGIC_LARGE,
    /* 3 */ WONDERITEM_DROP_MAGIC_SMALL,
    /* 4 */ WONDERITEM_DROP_HEART,
    /* 5 */ WONDERITEM_DROP_ARROWS_SMALL,
    /* 6 */ WONDERITEM_DROP_ARROWS_MEDIUM,
    /* 7 */ WONDERITEM_DROP_ARROWS_LARGE,
    /* 8 */ WONDERITEM_DROP_GREEN_RUPEE,
    /* 9 */ WONDERITEM_DROP_BLUE_RUPEE,
    /* A */ WONDERITEM_DROP_RED_RUPEE,
    /* B */ WONDERITEM_DROP_FLEXIBLE,
    /* C */ WONDERITEM_DROP_RANDOM
} EnWonderItemDrop;

#endif
