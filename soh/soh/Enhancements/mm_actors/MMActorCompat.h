#pragma once

#ifdef __cplusplus
#ifndef MM_ACTOR_COMPAT_CPLUSPLUS_CHECK_BYPASS
#error This header file is only to be included in the c files of MM actors
#endif
#endif

#include "z64.h"
#include <libultraship/bridge.h>
#include "align_asset_macro.h"

#undef SFX_FLAG
#define SFX_FLAG 0

#define NA_SE_EN_HIPLOOP_FOOTSTEP 0
#define NA_SE_EN_HIPLOOP_FOOT 0
#define NA_SE_EN_HIPLOOP_RUN 0
#define NA_SE_EN_HIPLOOP_PAUSE 0
#define NA_SE_EN_HIPLOOP_DAMAGE 0
#define NA_SE_EN_HIPLOOP_DEAD 0
#define NA_SE_EN_HIPLOOP_MASC_OFF 0
#define NA_SE_EN_COMMON_FREEZE 0
#define NA_SE_EN_COMMON_EXTINCT_LEV 0
#define NA_SE_EN_BAKUO_APPEAR 0
#define NA_SE_EN_BAKUO_VOICE 0
#define NA_SE_EN_BAKUO_ROLL 0
#define NA_SE_EN_BAKUO_DEAD 0
#define NA_SE_EN_FAMOS_FLOAT_REVERSE 0
#define NA_SE_EN_FAMOS_FLOAT 0
#define NA_SE_EN_FAMOS_REVERSE1 0
#define NA_SE_EN_FAMOS_REVERSE2 0
#define NA_SE_EN_EYEGOLE_DAMAGE 0
#define NA_SE_EN_EYEGOLE_DEAD 0
#define NA_SE_EN_LAST1_FALL_OLD 0
#define NA_SE_EN_SLIME_JUMP 0
#define NA_SE_EN_SLIME_DEFENCE 0
#define NA_SE_EN_SLIME_DEAD 0
#define NA_SE_EN_SLIME_DAMAGE 0
#define NA_SE_EN_SLIME_BREAK 0
#define NA_SE_EN_SLIME_SURFACE 0
#define NA_SE_EN_ANSATSUSYA_ENTRY 0
#define NA_SE_EN_ANSATSUSYA_SWORD 0
#define NA_SE_EN_ANSATSUSYA_MOVING 0
#define NA_SE_EN_ANSATSUSYA_CRYING 0
#define NA_SE_EN_ANSATSUSYA_SKIP 0
#define NA_SE_EN_ANSATSUSYA_DASH_2 0
#define NA_SE_EN_ANSATSUSYA_DAMAGE 0
#define NA_SE_EN_ANSATSUSYA_DEAD 0

#define COLORFILTER_BUFFLAG_OPA 0x0000
#define COLORFILTER_COLORFLAG_BLUE 0x0000
#define COLORFILTER_COLORFLAG_RED 0x4000
#define COLORFILTER_COLORFLAG_GRAY 0x8000

#define COLORFILTER_INTENSITY_FLAG 0x8000

#define COLORFILTER_BUFFLAG_XLU 0x2000
#define COLORFILTER_BUFFLAG_OPA 0x0000

#define ACTOR_DRAW_DMGEFF_FIRE 0
#define ACTOR_DRAW_DMGEFF_BLUE_FIRE 1
#define ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX 10
#define ACTOR_DRAW_DMGEFF_FROZEN_SFX 11
#define ACTOR_DRAW_DMGEFF_LIGHT_ORBS 20
#define ACTOR_DRAW_DMGEFF_BLUE_LIGHT_ORBS 21
#define ACTOR_DRAW_DMGEFF_ELECTRIC_SPARKS_SMALL 30
#define ACTOR_DRAW_DMGEFF_ELECTRIC_SPARKS_MEDIUM 31
#define ACTOR_DRAW_DMGEFF_ELECTRIC_SPARKS_LARGE 32

// Allows Tatl to fly over the actor and lock-on it (using the Z-target)
#define ACTOR_FLAG_TARGETABLE (1 << 0)
// Unused
#define ACTOR_FLAG_2 (1 << 1)
// Changes the targeting behaviour for unfriendly actors (sound effects, Player's stance, etc)
#define ACTOR_FLAG_UNFRIENDLY (1 << 2)
// Opposite of the UNFRIENDLY flag. It is not checked explictly in the original game.
#define ACTOR_FLAG_FRIENDLY (1 << 3)
//
#define ACTOR_FLAG_10 (1 << 4)
//
#define ACTOR_FLAG_20 (1 << 5)
//
#define ACTOR_FLAG_40 (1 << 6)
// hidden or revealed by Lens of Truth (depending on room lensMode)
#define ACTOR_FLAG_REACT_TO_LENS (1 << 7)
// Player has requested to talk to the actor; Player uses this flag differently than every other actor
#define ACTOR_FLAG_TALK_REQUESTED (1 << 8)
//
#define ACTOR_FLAG_200 (1 << 9)
//
#define ACTOR_FLAG_400 (1 << 10)
//
#define ACTOR_FLAG_800 (1 << 11)
// Actor will not shake when a quake occurs
#define ACTOR_FLAG_IGNORE_QUAKE (1 << 12)
//
#define ACTOR_FLAG_2000 (1 << 13)
//
#define ACTOR_FLAG_4000 (1 << 14)
//! Carried by arrow
#define ACTOR_FLAG_8000 (1 << 15)
//
#define ACTOR_FLAG_10000 (1 << 16)
// actor can press and hold down heavy switches
#define ACTOR_FLAG_CAN_PRESS_HEAVY_SWITCH (1 << 17)
//
#define ACTOR_FLAG_40000 (1 << 18)
//
#define ACTOR_FLAG_80000 (1 << 19)
//
#define ACTOR_FLAG_100000 (1 << 20)
//
#define ACTOR_FLAG_200000 (1 << 21)
//
#define ACTOR_FLAG_400000 (1 << 22)
//
#define ACTOR_FLAG_800000 (1 << 23)
//
#define ACTOR_FLAG_1000000 (1 << 24)
//
#define ACTOR_FLAG_2000000 (1 << 25)
// actor can press and hold down switches
#define ACTOR_FLAG_CAN_PRESS_SWITCH (1 << 26)
// Prevents locking on with Z targeting an actor even if Tatl is floating over it
#define ACTOR_FLAG_CANT_LOCK_ON (1 << 27)
//
#define ACTOR_FLAG_10000000 (1 << 28)
//
#define ACTOR_FLAG_20000000 (1 << 29)
//
#define ACTOR_FLAG_40000000 (1 << 30)
//
#define ACTOR_FLAG_80000000 (1 << 31)

#define BGCHECKFLAG_GROUND (1 << 0)       // Standing on the ground
#define BGCHECKFLAG_GROUND_TOUCH (1 << 1) // Has touched the ground (only active for 1 frame)
#define BGCHECKFLAG_GROUND_LEAVE (1 << 2) // Has left the ground (only active for 1 frame)
#define BGCHECKFLAG_WALL (1 << 3)         // Touching a wall
#define BGCHECKFLAG_CEILING (1 << 4)      // Touching a ceiling
#define BGCHECKFLAG_WATER (1 << 5)        // In water
#define BGCHECKFLAG_WATER_TOUCH (1 << 6)  // Has touched water (reset when leaving water)
#define BGCHECKFLAG_GROUND_STRICT \
    (1 << 7) // Similar to BGCHECKFLAG_GROUND but with no velocity check and is cleared every frame
#define BGCHECKFLAG_CRUSHED (1 << 8) // Crushed between a floor and ceiling (triggers a void for player)

// Flags for Actor_UpdateBgCheckInfo
#define UPDBGCHECKINFO_FLAG_1 (1 << 0) // check wall
#define UPDBGCHECKINFO_FLAG_2 (1 << 1) // check ceiling
#define UPDBGCHECKINFO_FLAG_4 (1 << 2) // check floor and water
#define UPDBGCHECKINFO_FLAG_8 (1 << 3)
#define UPDBGCHECKINFO_FLAG_10 (1 << 4)
#define UPDBGCHECKINFO_FLAG_20 (1 << 5) // unused
#define UPDBGCHECKINFO_FLAG_40 (1 << 6) // disable water ripples
#define UPDBGCHECKINFO_FLAG_80 (1 << 7)
#define UPDBGCHECKINFO_FLAG_100 (1 << 8)
#define UPDBGCHECKINFO_FLAG_200 (1 << 9)
#define UPDBGCHECKINFO_FLAG_400 (1 << 10) // check water
#define UPDBGCHECKINFO_FLAG_800 (1 << 11)

#define TARGET_MODE_5 5

#define Actor_PlaySfx Audio_PlayActorSound2
#define Gfx_SetupDL25_Opa Gfx_SetupDL_25Opa
#define Gfx_SetupDL25_Xlu Gfx_SetupDL_25Xlu
#define Gfx_SetupDL66 Gfx_SetupDL_66
#define Rumble_Request func_800A9F6C
#define Effect_Destroy Effect_Delete
#define GRAPH_ALLOC Graph_Alloc
#define func_800BC154 Actor_ChangeCategory
#define func_800B8D50 func_8002F6D4
#define gRingOfFireDL gEffFireCircleDL

#define Lib_SegmentedToVirtual(addr) SEGMENTED_TO_VIRTUAL(addr)
#define Actor_PlaySfx_Flagged(...) ((void)0)
#define SoundSource_PlaySfxEachFrameAtFixedWorldPos(...) ((void)0)
#define AudioSfx_SetChannelIO(...) ((void)0)
#define SWITCH_FLAG_NONE -1

#define ABS_ALT(x) ABS(x)
#define M_PIf M_PI

typedef enum FloorProperty {
    /*  0 */ FLOOR_PROPERTY_0,
    /*  1 */ FLOOR_PROPERTY_1,
    /*  2 */ FLOOR_PROPERTY_2,
    /*  5 */ FLOOR_PROPERTY_5 = 5,
    /*  6 */ FLOOR_PROPERTY_6,
    /*  7 */ FLOOR_PROPERTY_7,
    /*  8 */ FLOOR_PROPERTY_8,
    /*  9 */ FLOOR_PROPERTY_9,
    /* 11 */ FLOOR_PROPERTY_11 = 11,
    /* 12 */ FLOOR_PROPERTY_12,
    /* 13 */ FLOOR_PROPERTY_13
} FloorProperty;

typedef struct {
    /* 0x0 */ s8 segment;
    /* 0x2 */ s16 type;
    /* 0x4 */ void* params;
} AnimatedMaterial; // size = 0x8

typedef void* TexturePtr;

#define dD_801AEFA0 "__OTR__code/actor/D_801AEFA0"
static const ALIGN_ASSET(2) char D_801AEFA0[] = dD_801AEFA0;

#ifdef __cplusplus
extern "C" {
#endif

void Matrix_RotateXS(s16 x, MatrixMode mode);
void Matrix_RotateYS(s16 y, MatrixMode mode);
void Matrix_RotateZS(s16 z, MatrixMode mode);
void Matrix_MultVecZ(f32 z, Vec3f* dest);
void Matrix_MultZero(Vec3f* dest);
void Matrix_RotateZF(f32 z, MatrixMode mode);
void Matrix_RotateXFApply(f32 x);
s16 Math_Atan2S_XY(f32 x, f32 y);
void Actor_UpdateVelocityWithGravity(Actor* actor);
void Actor_MoveWithGravity(Actor* actor);
void Actor_MoveWithoutGravity(Actor* actor);
void Actor_SpawnIceEffects(PlayState* play, Actor* actor, Vec3f bodyPartsPos[], s32 bodyPartsCount,
                           s32 effectsPerBodyPart, f32 scale, f32 scaleRange);
s32 Collider_InitAndSetJntSph(PlayState* play, ColliderJntSph* sphereGroup, Actor* actor, ColliderJntSphInit* src,
                              ColliderJntSphElement* elements);
s32 Collider_InitAndSetCylinder(PlayState* play, ColliderCylinder* collider, Actor* actor, ColliderCylinderInit* src);
s32 Collider_InitAndSetQuad(PlayState* play, ColliderQuad* collider, Actor* actor, ColliderQuadInit* src);
void ActorShadow_DrawSquare(Actor* actor, Lights* lights, PlayState* play);
FloorProperty SurfaceType_GetFloorProperty2(CollisionContext* colCtx, CollisionPoly* poly, s32 bgId);
void func_800B3030(PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, s16 scale, s16 scaleStep,
                   s32 colorIndex);
void func_800AE5A0(PlayState* play);
s32 BgCheck_SphVsFirstWall(CollisionContext* colCtx, Vec3f* center, f32 radius);
#ifdef __cplusplus
}
#endif