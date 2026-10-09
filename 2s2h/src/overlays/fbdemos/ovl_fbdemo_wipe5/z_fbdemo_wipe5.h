#ifndef Z_FBDEMO_WIPE5_H
#define Z_FBDEMO_WIPE5_H

#include "ultra64.h"
#include "color.h"

typedef struct {
    /* 0x00 */ Color_RGBA8_u32 primColor;
    /* 0x04 */ Color_RGBA8_u32 envColor;
    /* 0x08 */ f32 unk_08;
    /* 0x0C */ f32 unk_0C;
    /* 0x10 */ u8 unk_10;
    /* 0x11 */ u8 unk_11;
    /* 0x12 */ u8 unk_12;
    /* 0x13 */ u8 isDone;
} TransitionWipe5; // size = 0x14

#endif
