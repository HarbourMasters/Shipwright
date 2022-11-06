#ifndef Z_EN_FZ_H
#define Z_EN_FZ_H

#include "ultra64.h"
#include "global.h"

struct EnFz;

typedef void (*EnFzActionFunc)(struct EnFz*, PlayState*);
typedef void (*EnFzSpawnIceSmokeFunc)(struct EnFz*);

typedef struct {
    /* 0x0000 */ u8 type; // 0,1,2: State of freezard (1 not freezing, 2 freezing)
    /* 0x0001 */ u8 timer; // increments primAlphaState after reaching 7 (freezing), used in Gfx_TwoTexScroll
    /* 0x0004 */ Vec3f pos; // Random position within 20.0f of actor
    /* 0x0010 */ Vec3f velocity; 
    /* 0x001C */ Vec3f accel;
    /* 0x0028 */ char unk_28[0x4];
    /* 0x002C */ s16 primAlpha; // transparency in RGBA colour system
    /* 0x002E */ s16 primAlphaState; // 0: increasing (more opaque) 1: decreasing (more transparent) 2: collision
    /* 0x0030 */ f32 xyScale; // 
    /* 0x0034 */ f32 xyScaleTarget; 
    /* 0x0038 */ u8 isTimerMod8; // conditional, used to run CollisionCheck_SetAT 
                 u32 epoch;
} EnFzEffectSsIceSmoke; // size = 0x3C

typedef struct EnFz {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ EnFzActionFunc actionFunc;
    /* 0x0150 */ ColliderCylinder collider1;
    /* 0x019C */ ColliderCylinder collider2;
    /* 0x01E8 */ ColliderCylinder collider3;
    /* 0x0234 */ Vec3f posOrigin; // Spawn position for moving freezard
    /* 0x0240 */ s16 counter; // A perpetual counter
    /* 0x0242 */ s16 unusedTimer1; 
    /* 0x0244 */ s16 timer; // Used to time transition into next action. Can be 10, 20, 40, 60, 80, 100
    /* 0x0246 */ u8 updateBgInfo; // Always true in every instance
    /* 0x0247 */ u8 isMoving; // Freezard is moving in xz plane
    /* 0x0248 */ u8 isFreezing; // Freezard shooting ice projectiles that can freeze Link
    /* 0x0249 */ u8 unusedCounter; // Incremented when Freezard takes damage
    /* 0x024C */ f32 iceSmokeFreezingSpawnHeight; // Height for Ice Smoke Spawn, only when freezing
    /* 0x0250 */ f32 unusedFloat; // Set to 135.0f
    /* 0x0254 */ f32 speedXZ; // Set to 4.0f when moving
    /* 0x0258 */ u32 envAlpha; // transparency in RGBA colour system
    /* 0x025C */ u16 unusedNum1; // Only set to 0
    /* 0x025E */ u16 unusedNum2; // Set to either 0 when hidden or 4000 when growing
    /* 0x0260 */ u8 state; // 0 (hidden) 1 (growning/shrinking) 2 (full size) 3 (melting from fire)
    /* 0x0261 */ u8 isActive; // Default true. Set to false after beginning to despawn
    /* 0x0262 */ u8 isDespawning; // Default false. Set to false after beginning to despawn or melting
    /* 0x0263 */ u8 unusedTimer2; // Timer
    /* 0x0264 */ Vec3f wallHitPos; // Position contact was made with a wall
    /* 0x0270 */ f32 distToTargetSq;
    /* 0x0274 */ EnFzEffectSsIceSmoke iceSmoke[40];
} EnFz; // size = 0x0BD4

#endif
