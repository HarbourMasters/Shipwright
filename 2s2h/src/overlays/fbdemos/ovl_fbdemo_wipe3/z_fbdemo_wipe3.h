#ifndef Z_FBDEMO_WIPE3_H
#define Z_FBDEMO_WIPE3_H

#include "ultra64.h"
#include "color.h"

typedef struct {
    /* 0x000 */ Color_RGBA8_u32 color;
    /* 0x008 */ Color_RGBA8_u32 envColor;
    /* 0x004 */ u32 scrollX;
    /* 0x00C */ s32 scrollY;
    /* 0x010 */ s32 wipeSpeed;
    /* 0x014 */ u8 dir;
    /* 0x015 */ u8 colorType;
    /* 0x016 */ u8 speedType;
    /* 0x017 */ u8 texIndex;
    /* 0x018 */ u8 isDone;
    /* 0x019 */ u8 frame;
    /* 0x01A */ u16 normal;
    /* 0x01C */ TexturePtr curTexture;
    /* 0x020 */ Mtx projection;
    /* 0x060 */ Mtx lookAt;
    /* 0x0A0 */ Mtx modelView[2];
} TransitionWipe3; // size = 0x120

#endif
