#ifndef Z_EN_JSJUTAN_H
#define Z_EN_JSJUTAN_H

#include "ultra64.h"
#include "global.h"

struct EnJsjutan;

typedef struct EnJsjutan {
    /* 0x0000 */ DynaPolyActor dyna;
    /* 0x0164 */ u8 unk_164; // flag to set carpet shadow textures.
    /* 0x0168 */ f32 unk_168; // actorYPos
    /* 0x016C */ f32 shadowAlpha;
    /* 0x0170 */ f32 unk_170;
    /* 0x0174 */ u8 unk_174; // flag?
    /* 0x0175 */ u8 unk_175; // setup flag?
} EnJsjutan; // size = 0x0178

typedef enum {
    /* 0 */ ENJSJUTAN_TYPE_00,
    /* 1 */ ENJSJUTAN_TYPE_01
} EnJsjutanType;

#endif
