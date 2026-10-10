#ifndef Z_FBDEMO_WIPE1_H
#define Z_FBDEMO_WIPE1_H

#include "ultra64.h"
#include "color.h"

typedef struct {
    /* 0x000 */ Color_RGBA8_u32 primColor;
    /* 0x004 */ Color_RGBA8_u32 envColor;
    /* 0x008 */ u8 direction;
    /* 0x009 */ u8 frame;
    /* 0x00A */ u8 isDone;
    /* 0x00C */ u16 texX;
    /* 0x00E */ u16 texY;
    /* 0x010 */ u16 normal;
    /* 0x018 */ Mtx projection;
    /* 0x058 */ Mtx lookAt;
    /* 0x098 */ Mtx modelView[2][3];
} TransitionWipe1; // size = 0x218

#endif
