#ifndef Z_EFF_SS_KAKERA_H
#define Z_EFF_SS_KAKERA_H

#include "ultra64.h"
#include "global.h"

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
    /* 0x3A */ s16 objId;
    /* 0x3C */ Gfx* dList;
} EffectSsKakeraInitParams; // size = 0x40

#define KAKERA_OBJECT_DEFAULT -1

typedef enum {
    /* -1 */ KAKERA_COLOR_NONE = -1,
    /*  0 */ KAKERA_COLOR_WHITE,
    /*  1 */ KAKERA_COLOR_BROWN
} KakeraColorIndex;

#endif
