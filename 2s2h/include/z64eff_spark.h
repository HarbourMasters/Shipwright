#ifndef Z64EFF_SPARK_H
#define Z64EFF_SPARK_H

#include "ultra64.h"

#include "color.h"
#include "z64math.h"

struct GraphicsContext;

typedef struct EffectSparkElement {
    /* 0x00 */ Vec3f velocity;
    /* 0x0C */ Vec3f position;
    /* 0x18 */ Vec3s unkVelocity;
    /* 0x1E */ Vec3s unkPosition;
} EffectSparkElement; // size = 0x24

typedef struct EffectSparkInit {
    /* 0x000 */ Vec3s position;
    /* 0x008 */ s32 numElements; // "table_size"; calculated as uDiv * vDiv + 2
    /* 0x00C */ EffectSparkElement elements[32];
    /* 0x48C */ f32 speed;
    /* 0x490 */ f32 gravity;
    /* 0x494 */ u32 uDiv; // "u_div"
    /* 0x498 */ u32 vDiv; // "v_div"
    /* 0x49C */ Color_RGBA8 colorStart[4];
    /* 0x4AC */ Color_RGBA8 colorEnd[4];
    /* 0x4BC */ s32 timer;
    /* 0x4C0 */ s32 duration;
} EffectSparkInit; // size = 0x4C4

typedef struct EffectSpark {
    /* 0x000 */ Vec3s position;
    /* 0x008 */ s32 numElements; // "table_size"; calculated as uDiv * vDiv + 2
    /* 0x00C */ EffectSparkElement elements[32];
    /* 0x48C */ f32 speed;
    /* 0x490 */ f32 gravity;
    /* 0x494 */ u32 uDiv; // "u_div"
    /* 0x498 */ u32 vDiv; // "v_div"
    /* 0x49C */ Color_RGBA8 colorStart[4];
    /* 0x4AC */ Color_RGBA8 colorEnd[4];
    /* 0x4BC */ s32 timer;
    /* 0x4C0 */ s32 duration;
} EffectSpark; // size = 0x4C4

void EffectSpark_Init(void* thisx, void* initParamsx);
void EffectSpark_Destroy(void* thisx);
s32 EffectSpark_Update(void* thisx);
void EffectSpark_Draw(void* thisx, struct GraphicsContext* gfxCtx);

#endif
