#ifndef _Z64_EFFECT_SS_KAKERA_H_
#define _Z64_EFFECT_SS_KAKERA_H_

#include "global.h"

#define KAKERA_OBJECT_DEFAULT -1

typedef enum {
    /* -1 */ KAKERA_COLOR_NONE = -1,
    /*  0 */ KAKERA_COLOR_WHITE,
    /*  1 */ KAKERA_COLOR_BROWN
} KakeraColorIndex;

typedef struct {
    /* 0x0 */ u8 lod;
    /* 0x1 */ Color_RGB8 color;
} KakeraColorStruct; // size = 0x4

typedef struct {
    /* 0x00 */ Vec3f pos;
    /* 0x0C */ Vec3f velocity;
    /* 0x18 */ Vec3f unk_18;
    /* 0x24 */ s16 gravity;
    /* 0x26 */ s16 unk_26;
    /* 0x28 */ s16 unk_28;
    /* 0x2A */ s16 unk_2A;
    /* 0x2C */ s16 unk_2C;
    /* 0x2E */ s16 scale;
    /* 0x30 */ s16 unk_30;
    /* 0x32 */ s16 unk_32;
    /* 0x34 */ s32 life;
    /* 0x38 */ s16 colorIdx;
    /* 0x3A */ s16 objectId;
    /* 0x3C */ Gfx* dList;
} EffectSsKakeraInitParams; // size = 0x40


#endif
