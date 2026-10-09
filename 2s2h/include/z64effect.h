#ifndef Z64EFFECT_H
#define Z64EFFECT_H

#include "PR/ultratypes.h"
#include "libc/stdint.h"
#include "color.h"
#include "z64light.h"
#include "z64math.h"
#include "unk.h"

#include "z64eff_blure.h"
#include "z64eff_tire_mark.h"
#include "z64eff_spark.h"
#include "z64eff_shield_particle.h"
#include "z64effect_ss.h"

struct GraphicsContext;
struct PlayState;

typedef enum EffectType {
    /* 0 */ EFFECT_SPARK,
    /* 1 */ EFFECT_BLURE1,
    /* 2 */ EFFECT_BLURE2,
    /* 3 */ EFFECT_SHIELD_PARTICLE,
    /* 4 */ EFFECT_TIRE_MARK,
    /* 5 */ EFFECT_MAX
} EffectType;

/* Effect Soft Sprites */

struct PlayState* Effect_GetPlayState(void);
void* Effect_GetByIndex(s32 index);
void Effect_Init(struct PlayState* play);
void Effect_Add(struct PlayState* play, s32* pIndex, EffectType type, u8 arg3, u8 arg4, void* initParams);
void Effect_DrawAll(struct GraphicsContext* gfxCtx);
void Effect_UpdateAll(struct PlayState* play);
void Effect_Destroy(struct PlayState* play, s32 index);
void Effect_DestroyAll(struct PlayState* play);

#endif
