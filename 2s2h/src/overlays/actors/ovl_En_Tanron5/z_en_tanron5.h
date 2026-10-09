#ifndef Z_EN_TANRON5_H
#define Z_EN_TANRON5_H

#include "global.h"

struct EnTanron5;

#define TWINMOLD_PROP_GET_TYPE(thisx) ((thisx)->params)
#define TWINMOLD_PROP_PARAMS(type) (type)

typedef enum {
    /*   0 */ TWINMOLD_PROP_TYPE_SPAWN_HANDLER,
    /*   1 */ TWINMOLD_PROP_TYPE_RUIN_PILLAR_1,
    /*   2 */ TWINMOLD_PROP_TYPE_RUIN_PILLAR_2,
    /*   3 */ TWINMOLD_PROP_TYPE_RUIN_PILLAR_3,
    /*   4 */ TWINMOLD_PROP_TYPE_RUIN_PILLAR_4,
    /*   5 */ TWINMOLD_PROP_TYPE_RUIN_PILLAR_5,
    /*   6 */ TWINMOLD_PROP_TYPE_RUIN_PILLAR_6,
    /*   7 */ TWINMOLD_PROP_TYPE_RUIN_PILLAR_7,
    /*   8 */ TWINMOLD_PROP_TYPE_RUIN_PILLAR_8,
    /*   9 */ TWINMOLD_PROP_TYPE_RUIN_PILLAR_9,
    /*  10 */ TWINMOLD_PROP_TYPE_RUIN_PILLAR_10,
    /*  11 */ TWINMOLD_PROP_TYPE_RUIN_PILLAR_11,
    /*  12 */ TWINMOLD_PROP_TYPE_RUIN_PILLAR_12,
    /*  13 */ TWINMOLD_PROP_TYPE_RUIN_PYRAMID_1,
    /*  14 */ TWINMOLD_PROP_TYPE_RUIN_PYRAMID_2,
    /*  15 */ TWINMOLD_PROP_TYPE_RUIN_PYRAMID_3,
    /*  16 */ TWINMOLD_PROP_TYPE_RUIN_PYRAMID_4,
    /*  17 */ TWINMOLD_PROP_TYPE_RUIN_PYRAMID_5,
    /*  18 */ TWINMOLD_PROP_TYPE_RUIN_PYRAMID_6,
    /*  19 */ TWINMOLD_PROP_TYPE_RUIN_PYRAMID_7,
    /*  20 */ TWINMOLD_PROP_TYPE_RUIN_PYRAMID_8,
    /* 100 */ TWINMOLD_PROP_TYPE_FRAGMENT_LARGE_1 = 100,
    /* 101 */ TWINMOLD_PROP_TYPE_FRAGMENT_LARGE_2,
    /* 102 */ TWINMOLD_PROP_TYPE_FRAGMENT_LARGE_3,
    /* 103 */ TWINMOLD_PROP_TYPE_FRAGMENT_LARGE_4,
    /* 104 */ TWINMOLD_PROP_TYPE_FRAGMENT_LARGE_5,
    /* 105 */ TWINMOLD_PROP_TYPE_FRAGMENT_LARGE_6,
    /* 106 */ TWINMOLD_PROP_TYPE_FRAGMENT_LARGE_7,
    /* 107 */ TWINMOLD_PROP_TYPE_FRAGMENT_SMALL_1,
    /* 108 */ TWINMOLD_PROP_TYPE_FRAGMENT_SMALL_2,
    /* 109 */ TWINMOLD_PROP_TYPE_FRAGMENT_SMALL_3,
    /* 110 */ TWINMOLD_PROP_TYPE_ITEM_DROP_1,
    /* 111 */ TWINMOLD_PROP_TYPE_ITEM_DROP_2
} TwinmoldPropType;

typedef struct EnTanron5 {
    /* 0x000 */ Actor actor;
    /* 0x144 */ s16 timer;
    /* 0x148 */ Gfx* dList;
    /* 0x14C */ ColliderCylinder collider;
    /* 0x198 */ union {
                    s16 fragmentAngularVelocityX;
                    s16 itemDropRotZ;
                };
    /* 0x19A */ s16 fragmentAngularVelocityY;
    /* 0x19C */ f32 baseScale;
    /* 0x1A0 */ union {
                    u8 hitCount;     // used by destructible ruins
                    u8 sinkTimer;    // used by ruin fragments
                    u8 itemDropType; // used by item drops
                };
    /* 0x1A1 */ u8 hitTimer;
} EnTanron5; // size = 0x1A4

#endif // Z_EN_TANRON5_H
