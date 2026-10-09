#ifndef Z64EFF_TIRE_MARK_H
#define Z64EFF_TIRE_MARK_H

#include "ultra64.h"

#include "color.h"
#include "z64math.h"
#include "unk.h"

struct CollisionContext;
struct CollisionPoly;
struct GraphicsContext;

#define EFFECT_TIRE_MARK_ELEMENT_FLAG_1      (1 << 0)
#define EFFECT_TIRE_MARK_ELEMENT_FLAG_2      (1 << 1)

typedef struct EffectTireMarkElement {
    /* 0x00 */ u16 flags;
    /* 0x02 */ Vec3s p1;
    /* 0x08 */ Vec3s p2;
    /* 0x0E */ s16 life;
    /* 0x10 */ UNK_TYPE1 pad10[0x4];
    /* 0x14 */ struct CollisionPoly* colPoly;
} EffectTireMarkElement; // size = 0x18

typedef struct EffectTireMarkInit {
    /* 0x0 */ s16 unk0;
    /* 0x2 */ s16 elemDuration;
    /* 0x4 */ Color_RGBA8 color;
} EffectTireMarkInit; // size = 0x8

typedef struct EffectTireMark {
    /* 0x000 */ EffectTireMarkElement elements[64];
    /* 0x600 */ s16 unk600;
    /* 0x602 */ s16 numElements;
    /* 0x604 */ s16 elemDuration;
    /* 0x606 */ Color_RGBA8 color;
} EffectTireMark; // size = 0x60C

void func_800AE930(struct CollisionContext* colCtx, EffectTireMark* thisx, Vec3f* pos, f32 arg3, s16 angle, struct CollisionPoly* colPoly, s32 bgId);
void func_800AEF44(EffectTireMark* thisx);
void EffectTireMark_Init(void* thisx, void* initParamsx);
void EffectTireMark_Destroy(void* thisx);
s32 EffectTireMark_Update(void* thisx);
void EffectTireMark_Draw(void* thisx, struct GraphicsContext* gfxCtx);

#endif
