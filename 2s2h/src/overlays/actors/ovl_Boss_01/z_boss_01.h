#ifndef Z_BOSS_01_H
#define Z_BOSS_01_H

#include "global.h"
#include "assets/objects/object_boss01/object_boss01.h"

struct Boss01;

#define ODOLWA_GET_TYPE(thisx) ((thisx)->params)
#define ODOLWA_GET_AFTERIMAGE_DESPAWN_TIMER(thisx) ((thisx)->world.rot.z)
#define ODOLWA_PARAMS(type) (type)

#define ODOLWA_SHADOW_TEX_WIDTH 64
#define ODOLWA_SHADOW_TEX_HEIGHT 64
#define ODOLWA_SHADOW_TEX_SIZE ((s32)sizeof(u8[ODOLWA_SHADOW_TEX_HEIGHT][ODOLWA_SHADOW_TEX_WIDTH]))

typedef void (*Boss01ActionFunc)(struct Boss01*, PlayState*);

typedef enum OdolwaType {
    /*  0 */ ODOLWA_TYPE_ODOLWA,
    /* 10 */ ODOLWA_TYPE_BUG = 10,
    /* 35 */ ODOLWA_TYPE_AFTERIMAGE = 35
} OdolwaType;

typedef enum OdolwaBodyPart {
    /*  0 */ ODOLWA_BODYPART_HEAD,
    /*  1 */ ODOLWA_BODYPART_TORSO,
    /*  2 */ ODOLWA_BODYPART_PELVIS,
    /*  3 */ ODOLWA_BODYPART_LEFT_UPPER_ARM,
    /*  4 */ ODOLWA_BODYPART_LEFT_FOREARM,
    /*  5 */ ODOLWA_BODYPART_SHIELD,
    /*  6 */ ODOLWA_BODYPART_RIGHT_UPPER_ARM,
    /*  7 */ ODOLWA_BODYPART_RIGHT_FOREARM,
    /*  8 */ ODOLWA_BODYPART_SWORD,
    /*  9 */ ODOLWA_BODYPART_RIGHT_THIGH,
    /* 10 */ ODOLWA_BODYPART_RIGHT_SHIN,
    /* 11 */ ODOLWA_BODYPART_RIGHT_FOOT,
    /* 12 */ ODOLWA_BODYPART_LEFT_THIGH,
    /* 13 */ ODOLWA_BODYPART_LEFT_SHIN,
    /* 14 */ ODOLWA_BODYPART_LEFT_FOOT,
    /* 15 */ ODOLWA_BODYPART_MAX
} OdolwaBodyPart;

typedef enum OdolwaColliderBodyPart {
    /*  0 */ ODOLWA_COLLIDER_BODYPART_HEAD,
    /*  1 */ ODOLWA_COLLIDER_BODYPART_TORSO,
    /*  2 */ ODOLWA_COLLIDER_BODYPART_PELVIS,
    /*  3 */ ODOLWA_COLLIDER_BODYPART_LEFT_UPPER_ARM,
    /*  4 */ ODOLWA_COLLIDER_BODYPART_LEFT_FOREARM,
    /*  5 */ ODOLWA_COLLIDER_BODYPART_RIGHT_UPPER_ARM,
    /*  6 */ ODOLWA_COLLIDER_BODYPART_RIGHT_FOREARM,
    /*  7 */ ODOLWA_COLLIDER_BODYPART_LEFT_THIGH,
    /*  8 */ ODOLWA_COLLIDER_BODYPART_LEFT_SHIN,
    /*  9 */ ODOLWA_COLLIDER_BODYPART_RIGHT_THIGH,
    /* 10 */ ODOLWA_COLLIDER_BODYPART_RIGHT_SHIN,
    /* 11 */ ODOLWA_COLLIDER_BODYPART_MAX
} OdolwaColliderBodyPart;

typedef enum OdolwaSwordCollider {
    /* 0 */ ODOLWA_SWORD_COLLIDER_SWORD_BASE,
    /* 1 */ ODOLWA_SWORD_COLLIDER_SWORD_TIP,
    /* 2 */ ODOLWA_SWORD_COLLIDER_PELVIS,
    /* 3 */ ODOLWA_SWORD_COLLIDER_MAX
} OdolwaSwordCollider;

typedef enum OdolwaShieldCollider {
    /* 0 */ ODOLWA_SHIELD_COLLIDER_SHIELD,
    /* 1 */ ODOLWA_SHIELD_COLLIDER_MAX
} OdolwaShieldCollider;

typedef enum OdolwaKickAndShieldBashCollider {
    /* 0 */ ODOLWA_KICK_AND_SHIELD_BASH_COLLIDER_SHIELD,
    /* 1 */ ODOLWA_KICK_AND_SHIELD_BASH_COLLIDER_LEFT_FOOT,
    /* 2 */ ODOLWA_KICK_AND_SHIELD_BASH_COLLIDER_MAX
} OdolwaKickAndShieldBashCollider;

typedef struct Boss01 {
    /* 0x000 */ Actor actor;
    /* 0x144 */ s16 frameCounter;
    /* 0x148 */ s32 phaseFrameCounter;
    /* 0x14C */ union {
                    u8 jumpIfPlayerIsClose;
                    u8 shouldPerformFallingSlash;
                    u8 bugDrawDmgEffType;
                };
    /* 0x14E */ s16 timers[3];
    /* 0x154 */ f32 animMorphFrames1;
    /* 0x158 */ f32 animMorphFrames2;
    /* 0x15C */ s16 damagedTimer; // TODO: Revisit this name when the equivalent Player variables are named
    /* 0x15E */ s16 damagedFlashTimer;
    /* 0x160 */ u8 isPerformingVerticalSlash; // set, but never checked
    /* 0x160 */ u8 landedFromJump;
    /* 0x162 */ u8 waitType;
    /* 0x163 */ u8 lookAtPlayer;
    /* 0x164 */ SkelAnime skelAnime;
    /* 0x1A8 */ f32 animEndFrame;
    /* 0x1AC */ f32 prevJumpVelocityY;
    /* 0x1B0 */ f32 runTargetPosAngularVelocityY;
    /* 0x1B4 */ f32 runTargetPosRotY;
    /* 0x1B8 */ u8 swordState;
    /* 0x1B9 */ u8 kickAndShieldBashCollisionEnabled;
    /* 0x1BA */ u8 swordAndShieldCollisionEnabled;
    /* 0x1BB */ u8 canGuardOrEvade;
    /* 0x1BC */ u8 bodyInvincibilityTimer;
    /* 0x1BE */ s16 disableCollisionTimer;
    /* 0x1C0 */ u8 afterimageSpawnFrameMask; // used as a bitmask with frameCounter to control how often to spawn afterimages
    /* 0x1C1 */ u8 hasPlayedSummonBugCs;
    /* 0x1C4 */ f32 additionalVelocityX;
    /* 0x1C8 */ f32 additionalVelocityZ;
    /* 0x1CC */ s16 headRotY;
    /* 0x1CE */ s16 headRotX;
    /* 0x1D0 */ Vec3s jointTable[ODOLWA_LIMB_MAX];
    /* 0x308 */ Vec3s morphTable[ODOLWA_LIMB_MAX];
    /* 0x440 */ Vec3f feetPos[2];
    /* 0x458 */ Vec3f pelvisPos;
    /* 0x464 */ Boss01ActionFunc actionFunc;
    /* 0x468 */ ColliderJntSph swordCollider;
    /* 0x488 */ ColliderJntSphElement swordColliderElements[ODOLWA_SWORD_COLLIDER_MAX];
    /* 0x548 */ ColliderJntSph shieldCollider;
    /* 0x568 */ ColliderJntSphElement shieldColliderElements[ODOLWA_SHIELD_COLLIDER_MAX];
    /* 0x5A8 */ ColliderJntSph bodyCollider;
    /* 0x5C8 */ ColliderJntSphElement bodyColliderElements[ODOLWA_COLLIDER_BODYPART_MAX];
    /* 0x888 */ ColliderJntSph kickAndShieldBashCollider;
    /* 0x8A8 */ ColliderJntSphElement kickAndShieldBashColliderElements[ODOLWA_KICK_AND_SHIELD_BASH_COLLIDER_MAX];
    /* 0x928 */ Vec3f bodyPartsPos[ODOLWA_BODYPART_MAX];
    /* 0x9DC */ union {
                    u32 cutsceneTimer;
                    u32 waitTimer;
                    u32 summonMothsTimer;
                };
    /* 0x9E0 */ s16 cutsceneState;
    /* 0x9E2 */ s16 subCamId;
    /* 0x9E4 */ Vec3f subCamEye;
    /* 0x9F0 */ Vec3f subCamAt;
    /* 0x9FC */ Vec3f subCamUp;
    /* 0xA08 */ Vec3f subCamEyeNext;
    /* 0xA14 */ f32 deathCsInitialSubCamRotY;
    /* 0xA18 */ f32 deathCsSubCamRotY;
    /* 0xA1C */ f32 subCamVelocity;
    /* 0xA20 */ f32 deathShrinkSpeed;
    /* 0xA24 */ f32 screenShakeOffsetY;
    /* 0xA28 */ f32 screenShakeMagnitude;
    /* 0xA2C */ ColliderCylinder bugACCollider;
    /* 0xA78 */ ColliderCylinder bugATCollider;
    /* 0xAC4 */ f32 bugDrawDmgEffScale;
    /* 0xAC8 */ f32 bugDrawDmgEffAlpha;
    /* 0xACC */ f32 drawDmgEffScale;
    /* 0xAD0 */ f32 drawDmgEffFrozenSteamScale;
    /* 0xAD4 */ f32 drawDmgEffAlpha;
    /* 0xAD8 */ u8 drawDmgEffType; 
    /* 0xAD9 */ u8 drawDmgEffState;
    /* 0xADA */ s16 drawDmgEffTimer;
} Boss01; // size = 0xADC

#endif // Z_BOSS_01_H
