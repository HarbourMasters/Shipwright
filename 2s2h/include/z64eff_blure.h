#ifndef Z64EFF_BLURE_H
#define Z64EFF_BLURE_H

#include "ultra64.h"

#include "color.h"
#include "z64math.h"

struct GraphicsContext;

#define EFFECT_BLURE_COLOR_COUNT 4

#define EFFECT_BLURE_FLAG_2         (1 << 1)
#define EFFECT_BLURE_FLAG_4         (1 << 2)
#define EFFECT_BLURE_FLAG_10        (1 << 4)

#define EFFECT_BLURE_ELEMENT_FLAG_1         (1 << 0)
#define EFFECT_BLURE_ELEMENT_FLAG_2         (1 << 1)
#define EFFECT_BLURE_ELEMENT_FLAG_4         (1 << 2)
#define EFFECT_BLURE_ELEMENT_FLAG_8         (1 << 3)
#define EFFECT_BLURE_ELEMENT_FLAG_10        (1 << 4)
#define EFFECT_BLURE_ELEMENT_FLAG_20        (1 << 5)

typedef struct EffectBlureElement {
    /* 0x00 */ s32 state;
    /* 0x04 */ s32 timer;
    /* 0x08 */ Vec3s p1;
    /* 0x0E */ Vec3s p2;
    /* 0x14 */ u16 flags;
} EffectBlureElement; // size = 0x18

typedef struct EffectBlureInit1 {
    /* 0x000 */ char unk_00[0x184];
    /* 0x184 */ u8 p1StartColor[EFFECT_BLURE_COLOR_COUNT];
    /* 0x188 */ u8 p2StartColor[EFFECT_BLURE_COLOR_COUNT];
    /* 0x18C */ u8 p1EndColor[EFFECT_BLURE_COLOR_COUNT];
    /* 0x190 */ u8 p2EndColor[EFFECT_BLURE_COLOR_COUNT];
    /* 0x194 */ s32 elemDuration;
    /* 0x198 */ s32 unkFlag;
    /* 0x19C */ s32 calcMode;
} EffectBlureInit1; // size = 0x1A0

typedef enum EffectBlureDrawMode {
    /* 0 */ EFF_BLURE_DRAW_MODE_SIMPLE,
    /* 1 */ EFF_BLURE_DRAW_MODE_SIMPLE_ALT_COLORS,
    /* 2 */ EFF_BLURE_DRAW_MODE_SMOOTH,
    /* 3 */ EFF_BLURE_DRAW_MODE_MAX
} EffectBlureDrawMode;

typedef struct EffectBlureInit2 {
    /* 0x00 */ s32 calcMode;
    /* 0x04 */ u16 flags;
    /* 0x06 */ s16 addAngleChange;
    /* 0x08 */ u8 p1StartColor[EFFECT_BLURE_COLOR_COUNT];
    /* 0x0C */ u8 p2StartColor[EFFECT_BLURE_COLOR_COUNT];
    /* 0x10 */ u8 p1EndColor[EFFECT_BLURE_COLOR_COUNT];
    /* 0x14 */ u8 p2EndColor[EFFECT_BLURE_COLOR_COUNT];
    /* 0x18 */ u8 elemDuration;
    /* 0x19 */ u8 unkFlag;
    /* 0x1A */ u8 drawMode; // EffectBlureDrawMode enum
    /* 0x1B */ u8 mode4Param;
    /* 0x1C */ Color_RGBA8 altPrimColor; // used with EFF_BLURE_DRAW_MODE_SIMPLE_ALT_COLORS
    /* 0x20 */ Color_RGBA8 altEnvColor; // used with EFF_BLURE_DRAW_MODE_SIMPLE_ALT_COLORS
} EffectBlureInit2; // size = 0x24

typedef struct EffectBlure {
    /* 0x000 */ EffectBlureElement elements[16];
    /* 0x180 */ s32 calcMode;
    /* 0x184 */ f32 mode4Param;
    /* 0x188 */ u16 flags;
    /* 0x18A */ s16 addAngleChange;
    /* 0x18C */ s16 addAngle;
    /* 0x18E */ u8 p1StartColor[EFFECT_BLURE_COLOR_COUNT];
    /* 0x192 */ u8 p2StartColor[EFFECT_BLURE_COLOR_COUNT];
    /* 0x196 */ u8 p1EndColor[EFFECT_BLURE_COLOR_COUNT];
    /* 0x19A */ u8 p2EndColor[EFFECT_BLURE_COLOR_COUNT];
    /* 0x19E */ u8 numElements; // "now_edge_num"
    /* 0x19F */ u8 elemDuration;
    /* 0x1A0 */ u8 unkFlag;
    /* 0x1A1 */ u8 drawMode; // EffectBlureDrawMode enum
    /* 0x1A2 */ Color_RGBA8 altPrimColor; // used with EFF_BLURE_DRAW_MODE_SIMPLE_ALT_COLORS
    /* 0x1A6 */ Color_RGBA8 altEnvColor; // used with EFF_BLURE_DRAW_MODE_SIMPLE_ALT_COLORS
} EffectBlure; // size = 0x1AC

void EffectBlure_AddVertex(EffectBlure* thisx, Vec3f* p1, Vec3f* p2);
void EffectBlure_AddSpace(EffectBlure* thisx);
void EffectBlure_Init1(void* thisx, void* initParamsx);
void EffectBlure_Init2(void* thisx, void* initParamsx);
void EffectBlure_Destroy(void* thisx);
s32 EffectBlure_Update(void* thisx);
void EffectBlure_Draw(void* thisx, struct GraphicsContext* gfxCtx);

#endif
