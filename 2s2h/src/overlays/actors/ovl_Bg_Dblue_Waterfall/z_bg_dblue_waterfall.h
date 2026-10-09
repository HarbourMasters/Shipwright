#ifndef Z_BG_DBLUE_WATERFALL_H
#define Z_BG_DBLUE_WATERFALL_H

#include "global.h"

struct BgDblueWaterfall;

typedef void (*BgDblueWaterfallActionFunc)(struct BgDblueWaterfall*, PlayState*);

#define BGDBLUEWATERFALL_GET_SWITCH_FLAG(thisx) ((thisx)->params & 0x7F)
#define BGDBLUEWATERFALL_GET_100(thisx) (((thisx)->params >> 8) & 1)

typedef struct BgDblueWaterfall {
    /* 0x000 */ Actor actor;
    /* 0x144 */ ColliderCylinder collider;
    /* 0x190 */ AnimatedMaterial* unk_190;
    /* 0x194 */ BgDblueWaterfallActionFunc actionFunc;
    /* 0x198 */ f32 unk_198;
    /* 0x19C */ s16 unk_19C;
    /* 0x19E */ u8 unk_19E;
    /* 0x19F */ u8 unk_19F;
    /* 0x1A0 */ u8 unk_1A0;
    /* 0x1A1 */ UNK_TYPE1 unk1A1[2];
    /* 0x1A3 */ s8 unk_1A3;
    /* 0x1A4 */ s16 csId;
    /* 0x1A6 */ UNK_TYPE1 unk1A6[1];
    /* 0x1A7 */ s8 unk_1A7;
    /* 0x1A8 */ f32 unk_1A8;
} BgDblueWaterfall; // size = 0x1AC

#endif // Z_BG_DBLUE_WATERFALL_H
