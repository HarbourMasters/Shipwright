#ifndef Z_EN_GOMA_H
#define Z_EN_GOMA_H

#include "ultra64.h"
#include "global.h"

typedef enum {
    /* 0 */ ENGOMA_NORMAL,
    /* 1 */ ENGOMA_EGG,
    /* 2 */ ENGOMA_HATCH_DEBRIS,
    /* 3 */ ENGOMA_BOSSLIMB
} GomaType;

struct EnGoma;

typedef void (*EnGomaActionFunc)(struct EnGoma*, PlayState*);

typedef enum {
    /*  0 */ GOMA_LIMB_NONE,
    /*  1 */ GOMA_LIMB_ROOT1,
    /*  2 */ GOMA_LIMB_ROOT2,
    /*  3 */ GOMA_LIMB_BODY,
    /*  4 */ GOMA_LIMB_ANTENNA_ROOT1,
    /*  5 */ GOMA_LIMB_ANTENNA_ROOT2,
    /*  6 */ GOMA_LIMB_ANTENNA,
    /*  7 */ GOMA_LIMB_EYE_IRIS_ROOT1,
    /*  8 */ GOMA_LIMB_EYE_IRIS_ROOT2,
    /*  9 */ GOMA_LIMB_EYE_IRIS,
    /* 10 */ GOMA_LIMB_L_LEG_ROOT1,
    /* 11 */ GOMA_LIMB_L_LEG_ROOT2,
    /* 12 */ GOMA_LIMB_L_SHIN_ROOT,
    /* 13 */ GOMA_LIMB_L_FOOT_ROOT,
    /* 14 */ GOMA_LIMB_L_FOOT,
    /* 15 */ GOMA_LIMB_L_SHIN,
    /* 16 */ GOMA_LIMB_L_THIGH,
    /* 17 */ GOMA_LIMB_R_LEG_ROOT1,
    /* 18 */ GOMA_LIMB_R_LEG_ROOT2,
    /* 19 */ GOMA_LIMB_R_SHIN_ROOT,
    /* 20 */ GOMA_LIMB_R_FOOT_ROOT,
    /* 21 */ GOMA_LIMB_R_FOOT,
    /* 22 */ GOMA_LIMB_R_SHIN,
    /* 23 */ GOMA_LIMB_R_THIGH,
    /* 24 */ GOMA_LIMB_MAX
} EnGomaLimb;

typedef struct EnGoma {
    /* 0x000 */ Actor actor;
    /* 0x14C */ SkelAnime skelanime;
    /* 0x190 */ Vec3s jointTable[24];
    /* 0x220 */ Vec3s morphTable[24];
    /* 0x2B0 */ EnGomaActionFunc actionFunc;
    /* 0x2B4 */ s16 slopePitch;
    /* 0x2B6 */ s16 slopeRoll;
    /* 0x2B8 */ s16 gomaType;
    /* 0x2BA */ s16 eyePitch;
    /* 0x2BC */ s16 eyeYaw;
    /* 0x2BE */ s16 hatchState;
    /* 0x2C0 */ s16 eggTimer;
    /* 0x2C2 */ s16 hurtTimer;
    /* 0x2C4 */ s16 visualState;
    /* 0x2C6 */ s16 playerDetectionTimer;
    /* 0x2C8 */ s16 spawnNum; // some debug spawn ID
    /* 0x2CA */ s16 invincibilityTimer;
    /* 0x2CC */ s16 actionTimer;
    /* 0x2D0 */ f32 eggScale;
    /* 0x2D4 */ f32 eggPitch;
    /* 0x2D8 */ f32 eggSquishAngle;
    /* 0x2DC */ f32 eggSquishAccel;
    /* 0x2E0 */ f32 eyeEnvColor[3];
    /* 0x2EC */ f32 eggSquishAmount;
    /* 0x2F0 */ f32 eggYOffset;
    /* 0x2F4 */ s32 unk_2F4;
    /* 0x2F8 */ s16 stunTimer;
    /* 0x2FC */ Vec3f shieldKnockbackVel;
    /* 0x308 */ Gfx* bossLimbDL; // set by z_boss_goma
    /* 0x30C */ ColliderCylinder colCyl1;
    /* 0x358 */ ColliderCylinder colCyl2;
} EnGoma; // size = 0x03A4

#endif
