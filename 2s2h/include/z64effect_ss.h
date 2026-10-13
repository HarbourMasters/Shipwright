#ifndef Z64EFFECT_SS_H
#define Z64EFFECT_SS_H

#include "ultra64.h"

#include "color.h"
#include "z64math.h"

#include "z64eff_ss_dead.h"

struct Actor;
struct PlayState;
struct EffectSs;

/* G Effect Regs */

#define rgTexIndex regs[0]
#define rgScale regs[1]
#define rgTexIndexStep regs[2]
#define rgPrimColorR regs[3]
#define rgPrimColorG regs[4]
#define rgPrimColorB regs[5]
#define rgPrimColorA regs[6]
#define rgEnvColorR regs[7]
#define rgEnvColorG regs[8]
#define rgEnvColorB regs[9]
#define rgEnvColorA regs[10]
#define rgObjectSlot regs[11]

typedef u32 (*EffectSsInitFunc)(struct PlayState* play, u32 index, struct EffectSs* effectSs, void* initParams);
typedef void(*EffectSsUpdateFunc)(struct PlayState* play, u32 index, struct EffectSs* particle);
typedef void(*EffectSsDrawFunc)(struct PlayState* play, u32 index, struct EffectSs* particle);

typedef struct EffectSsProfile {
    /* 0x0 */ u32 type;
    /* 0x4 */ EffectSsInitFunc init;
} EffectSsProfile; // size = 0x8

typedef struct EffectSsOverlay {
    /* 0x00 */ uintptr_t vromStart;
    /* 0x04 */ uintptr_t vromEnd;
    /* 0x08 */ void* vramStart;
    /* 0x0C */ void* vramEnd;
    /* 0x10 */ void* loadedRamAddr;
    /* 0x14 */ EffectSsProfile* profile;
    /* 0x18 */ u8 unk18; // Always 1?
} EffectSsOverlay; // size = 0x1C

typedef struct EffectSs {
    /* 0x00 */ Vec3f pos;
    /* 0x0C */ Vec3f velocity;
    /* 0x18 */ Vec3f accel;
    /* 0x24 */ EffectSsUpdateFunc update;
    /* 0x28 */ EffectSsDrawFunc draw;
    /* 0x2C */ Vec3f vec;
    /* 0x38 */ void* gfx;
    /* 0x3C */ struct Actor* actor;
    /* 0x40 */ s16 regs[13]; // These are particle-specific
    /* 0x5A */ u16 flags; // bit 0: set if this entry is not considered free on a priority tie bit 1: ? bit 2: ?
    /* 0x5C */ s16 life; // -1 means this entry is free
    /* 0x5E */ u8 priority; // Lower number mean higher priority
    /* 0x5F */ u8 type;
    u32 epoch;
} EffectSs; // size = 0x60

typedef struct EffectSsInfo {
    /* 0x0 */ EffectSs* table; // "data_table" from debug assert
    /* 0x4 */ s32 searchStartIndex;
    /* 0x8 */ s32 tableSize;
} EffectSsInfo; // size = 0xC

#define DEFINE_EFFECT_SS(_name, enumValue) enumValue,
#define DEFINE_EFFECT_SS_UNSET(enumValue) enumValue,

typedef enum EffectSsType {
    #include "tables/effect_ss_table.h"
    /* 0x27 */ EFFECT_SS_TYPE_MAX
} EffectSsType;

#undef DEFINE_EFFECT_SS
#undef DEFINE_EFFECT_SS_UNSET

void EffectSs_InitInfo(struct PlayState* play, s32 tableSize);
void EffectSs_ClearAll(struct PlayState* play);
EffectSs* EffectSs_GetTable(void);
void EffectSs_Delete(EffectSs* effectSs);
void EffectSs_Insert(struct PlayState* play, EffectSs* effectSs);
void EffectSs_Spawn(struct PlayState* play, s32 type, s32 priority, void* initData);
void EffectSs_UpdateAll(struct PlayState* play);
void EffectSs_DrawAll(struct PlayState* play);
s16 EffectSs_LerpInv(s16 a, s16 b, s32 weightInv);
s16 EffectSs_LerpS16(s16 a, s16 b, f32 weight);
u8 EffectSs_LerpU8(u8 a, u8 b, f32 weight);
void EffectSs_DrawGEffect(struct PlayState* play, EffectSs* thisx, void* texture);
void EffectSsDust_Spawn(struct PlayState* play, u16 drawFlags, Vec3f* pos, Vec3f* velocity, Vec3f* accel, Color_RGBA8* primColor, Color_RGBA8* envColor, s16 scale, s16 scaleStep, s16 life, u8 updateMode);
void func_800B0DE0(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, Color_RGBA8* primColor, Color_RGBA8* envColor, s16 scale, s16 scaleStep);
void func_800B0E48(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, Color_RGBA8* primColor, Color_RGBA8* envColor, s16 scale, s16 scaleStep);
void func_800B0EB0(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, Color_RGBA8* primColor, Color_RGBA8* envColor, s16 scale, s16 scaleStep, s16 life);
void func_800B0F18(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, Color_RGBA8* primColor, Color_RGBA8* envColor, s16 scale, s16 scaleStep, s16 life);
void func_800B0F80(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, Color_RGBA8* primColor, Color_RGBA8* envColor, s16 scale, s16 scaleStep, s16 life);
void func_800B0FE8(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, Color_RGBA8* primColor, Color_RGBA8* envColor, s16 scale, s16 scaleStep);
void func_800B1054(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, Color_RGBA8* primColor, Color_RGBA8* envColor, s16 scale, s16 scaleStep);
void func_800B10C0(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel);
void func_800B1130(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel);
void func_800B11A0(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, s16 scale, s16 scaleStep);
void func_800B1210(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, s16 scale, s16 scaleStep);
void func_800B1280(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, s16 scale, s16 scaleStep, s16 life);
void func_800B12F0(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, s16 scale, s16 scaleStep, s16 life);
void func_800B1360(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, Color_RGBA8* primColor, Color_RGBA8* envColor);
void func_800B139C(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, Color_RGBA8* primColor, Color_RGBA8* envColor);
void func_800B14D4(struct PlayState* play, f32 randScale, Vec3f* srcPos);
void func_800B1598(struct PlayState* play, f32 randScale, Vec3f* srcPos);
void EffectSsKirakira_SpawnSmallYellow(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel);
void EffectSsKirakira_SpawnSmall(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, Color_RGBA8* primColor, Color_RGBA8* envColor);
void EffectSsKirakira_SpawnDispersed(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, Color_RGBA8* primColor, Color_RGBA8* envColor, s16 scale, s32 life);
void EffectSsKirakira_SpawnFocused(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, Color_RGBA8* primColor, Color_RGBA8* envColor, s16 scale, s32 life);
void EffectSsBomb2_SpawnFade(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel);
void EffectSsBomb2_SpawnLayered(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, s16 scale, s16 scaleStep);
void EffectSsBlast_Spawn(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, Color_RGBA8* primColor, Color_RGBA8* envColor, s16 scale, s16 scaleStep, s16 scaleStepDecay, s16 life);
void EffectSsBlast_SpawnWhiteCustomScale(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, s16 scale, s16 scaleStep, s16 life);
void EffectSsBlast_SpawnShockwave(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, Color_RGBA8* primColor, Color_RGBA8* envColor, s16 life);
void EffectSsBlast_SpawnWhiteShockwave(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel);
void EffectSsGSpk_SpawnAccel(struct PlayState* play, struct Actor* actor, Vec3f* pos, Vec3f* velocity, Vec3f* accel, Color_RGBA8* primColor, Color_RGBA8* envColor, s16 scale, s16 scaleStep);
void EffectSsGSpk_SpawnNoAccel(struct PlayState* play, struct Actor* actor, Vec3f* pos, Vec3f* velocity, Vec3f* accel, Color_RGBA8* primColor, Color_RGBA8* envColor, s16 scale, s16 scaleStep);
void EffectSsGSpk_SpawnFuse(struct PlayState* play, struct Actor* actor, Vec3f* pos, Vec3f* velocity, Vec3f* accel);
void EffectSsGSpk_SpawnRandColor(struct PlayState* play, struct Actor* actor, Vec3f* pos, Vec3f* velocity, Vec3f* accel, s16 scale, s16 scaleStep);
void EffectSsGSpk_SpawnSmall(struct PlayState* play, struct Actor* actor, Vec3f* pos, Vec3f* velocity, Vec3f* accel, Color_RGBA8* primColor, Color_RGBA8* envColor);
void EffectSsDFire_Spawn(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, s16 scale, s16 scaleStep, s16 alpha, s16 alphaStep, s16 fadeDelay, s32 life);
void EffectSsBubble_Spawn(struct PlayState* play, Vec3f* pos, f32 yPosOffset, f32 yPosRandScale, f32 xzPosRandScale, f32 scale);
void EffectSsGRipple_Spawn(struct PlayState* play, Vec3f* pos, s16 radius, s16 radiusMax, s16 life);
void EffectSsGSplash_Spawn(struct PlayState* play, Vec3f* pos, Color_RGBA8* primColor, Color_RGBA8* envColor, s16 type, s16 scale);
void EffectSsGFire_Spawn(struct PlayState* play, Vec3f* pos);
void EffectSsLightning_Spawn(struct PlayState* play, Vec3f* pos, Color_RGBA8* primColor, Color_RGBA8* envColor, s16 scale, s16 yaw, s16 life, s16 numBolts);
void EffectSsDtBubble_SpawnColorProfile(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, s16 scale, s16 life, s16 colorProfile, s16 randXZ);
void EffectSsDtBubble_SpawnCustomColor(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, Color_RGBA8* primColor, Color_RGBA8* envColor, s16 scale, s16 life, s16 randXZ);
void EffectSsHahen_Spawn(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, s16 flags, s16 scale, s16 objectId, s16 life, Gfx* dList);
void EffectSsHahen_SpawnBurst(struct PlayState* play, Vec3f* pos, f32 burstScale, s16 flags, s16 scale, s16 randScaleRange, s16 count, s16 objectId, s16 life, Gfx* dList);
void func_800B2364(struct PlayState* play, Vec3f* pos, Gfx* dList);
void EffectSsStick_Spawn(struct PlayState* play, Vec3f* pos, s16 yaw);
void EffectSsSibuki_Spawn(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, s16 moveDelay, s16 direction, s16 scale);
void EffectSsSibuki_SpawnBurst(struct PlayState* play, Vec3f* pos);
void EffectSsStone1_Spawn(struct PlayState* play, Vec3f* pos, s32 reg0);
void EffectSsHitmark_SpawnFixedScale(struct PlayState* play, s32 type, Vec3f* pos);
void EffectSsHitmark_SpawnCustomScale(struct PlayState* play, s32 type, s16 scale, Vec3f* pos);
void EffectSsFhgFlash_SpawnShock(struct PlayState* play, struct Actor* actor, Vec3f* pos, s16 scale, u8 params);
void EffectSsKFire_Spawn(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, s16 scaleMax, u8 type);
void EffectSsSolderSrchBall_Spawn(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, s16 scale, s16* playerDetected, s16 flags);
void EffectSsKakera_Spawn(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* arg3, s16 gravity, s16 arg5, s16 arg6, s16 arg7, s16 arg8, s16 scale, s16 arg10, s16 arg11, s32 life, s16 colorIdx, s16 objectId, Gfx* dList);
void EffectSsIcePiece_Spawn(struct PlayState* play, Vec3f* pos, f32 scale, Vec3f* velocity, Vec3f* accel, s32 life);
void EffectSsIcePiece_SpawnBurst(struct PlayState* play, Vec3f* refPos, f32 scale);
void func_800B2B44(struct PlayState* play, struct Actor* actor, Vec3f* pos, f32 scale);
void func_800B2B7C(struct PlayState* play, struct Actor* actor, Vec3s* arg2, f32 scale);
void EffectSsEnIce_Spawn(struct PlayState* play, Vec3f* pos, f32 scale, Vec3f* velocity, Vec3f* accel, Color_RGBA8* primColor, Color_RGBA8* envColor, s32 life);
void EffectSsFireTail_Spawn(struct PlayState* play, struct Actor* actor, Vec3f* pos, f32 scale, Vec3f* arg4, s16 arg5, Color_RGBA8* primColor, Color_RGBA8* envColor, s16 type, s16 bodyPart, s32 life);
void EffectSsFireTail_SpawnFlame(struct PlayState* play, struct Actor* actor, Vec3f* pos, f32 arg3, s16 bodyPart, f32 colorIntensity);
void EffectSsFireTail_SpawnFlameOnPlayer(struct PlayState* play, f32 scale, s16 bodyPart, f32 colorIntensity);
void EffectSsEnFire_SpawnVec3f(struct PlayState* play, struct Actor* actor, Vec3f* pos, s16 scale, s16 params, s16 flags, s16 bodyPart);
void EffectSsEnFire_SpawnVec3s(struct PlayState* play, struct Actor* actor, Vec3s* pos, s16 scale, s16 params, s16 flags, s16 bodyPart);
void EffectSsExtra_Spawn(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, s16 scale, s16 scoreIndex);
void EffectSsDeadDb_Spawn(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, Color_RGBA8* prim, Color_RGBA8* env, s16 scale, s16 scaleStep, s32 life);
void func_800B3030(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, s16 scale, s16 scaleStep, s32 colorIndex);
void EffectSsDeadDd_Spawn(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, Color_RGBA8* prim, Color_RGBA8* env, s16 scale, s16 scaleStep, s16 alphaStep, s32 life);
void EffectSsDeadDs_Spawn(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, s16 scale, s16 scaleStep, s16 alpha, s32 life);
void func_800B31BC(struct PlayState* play, Vec3f* pos, s16 scale, s16 scaleStep, s16 alpha, s32 life);
void EffectSsIceSmoke_Spawn(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, s16 scale);
void EffectSsIceBlock_Spawn(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, s16 scale);


extern EffectSsOverlay gEffectSsOverlayTable[EFFECT_SS_TYPE_MAX];

#endif
