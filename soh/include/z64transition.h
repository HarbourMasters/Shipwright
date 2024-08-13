#ifndef Z64TRANSITION_H
#define Z64TRANSITION_H

#include <libultraship/libultra.h>
#include <libultraship/color.h>

typedef struct {
    f32 unk_0;
    f32 unk_4;
} TransitionUnkData;

typedef struct {
    /* 0x00 */ s32 row;
    /* 0x04 */ s32 col;
    /* 0x08 */ s32 frame;
    /* 0x0C */ TransitionUnkData* unk_0C;
    /* 0x10 */ Vtx* vtxFrame1;
    /* 0x14 */ Vtx* vtxFrame2;
    /* 0x18 */ Mtx projection;
    /* 0x58 */ Mtx modelView;
    /* 0x98 */ Mtx unk_98;
    /* 0xD8 */ Gfx* gfx; // "gfxtbl"
    /* 0xDC */ u16* zBuffer;
} TransitionUnk; // size = 0xE0

typedef struct {
    /* 0x000 */ Color_RGBA8_u32 color;
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
} TransitionWipe; // size = 0x218

typedef struct {
    /* 0x000 */ u8 fadeType;
    /* 0x001 */ u8 isDone;
    /* 0x002 */ u8 fadeDirection;
    /* 0x004 */ Color_RGBA8_u32 fadeColor;
    /* 0x008 */ u16 fadeTimer;
} TransitionFade; // size = 0xC

typedef struct {
    /* 0x000 */ Color_RGBA8_u32 color;
    /* 0x004 */ Color_RGBA8_u32 envColor;
    /* 0x008 */ s32 texX;
    /* 0x00C */ s32 texY;
    // /* 0x010 */ s32 step;
    // /* 0x014 */ u8 unk_14;
    // /* 0x015 */ u8 typeColor;
    // /* 0x016 */ u8 speed;
    // /* 0x017 */ u8 effect;
    /* 0x010 */ s32 speed;
    /* 0x014 */ u8 direction;
    /* 0x015 */ u8 colorType;
    /* 0x016 */ u8 speedType;
    /* 0x017 */ u8 appearanceType;
    /* 0x018 */ u8 isDone;
    /* 0x019 */ u8 frame;
    /* 0x01A */ u16 normal;
    /* 0x020 */ Mtx projection;
    /* 0x060 */ Mtx lookAt;
    /* 0x0A0 */ void* texture;
    /* 0x0A8 */ Mtx modelView[2][3];
} TransitionCircle; // size = 0x228;

typedef struct {
    /* 0x000 */ Color_RGBA8_u32 color;
    /* 0x004 */ f32 transPos;
    /* 0x008 */ f32 step;
    /* 0x00C */ s32 state;
    /* 0x010 */ s32 fadeDirection;
    /* 0x018 */ Mtx projection;
    /* 0x058 */ s32 frame;
    /* 0x060 */ Mtx modelView[2][3];
} TransitionTriforce; // size = 0x1E0;

#endif
