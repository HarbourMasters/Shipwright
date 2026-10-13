#ifndef Z_EN_KNIGHT_H
#define Z_EN_KNIGHT_H

#include "assert.h"
#include "global.h"
#include "objects/object_knight/object_knight.h"

static_assert((u32)IGOS_LIMB_MAX == (u32)KNIGHT_LIMB_MAX,
              "The skeletons for Igos du Ikana and his aides must match in structure");

struct EnKnight;

typedef void (*EnKnightActionFunc)(struct EnKnight*, PlayState*);

typedef enum {
    /*   0 */ EN_KNIGHT_PARAM_IGOS_MAIN = 0,
    /*  35 */ EN_KNIGHT_PARAM_OTHERS = 35,
    /* 100 */ EN_KNIGHT_PARAM_IGOS_HEAD = 100,
    /* 200 */ EN_KNIGHT_PARAM_KNIGHT_BODY_AFTERIMAGE = 200,
    /* 201 */ EN_KNIGHT_PARAM_IGOS_BODY_AFTERIMAGE,
    /* 202 */ EN_KNIGHT_PARAM_IGOS_HEAD_AFTERIMAGE
} EnKnightParam;

typedef enum {
    /* 0 */ KNIGHT_SUB_ACTION_CURTAIN_REACT_WAIT,
    /* 1 */ KNIGHT_SUB_ACTION_CURTAIN_REACT_BEGIN,
    /* 2 */ KNIGHT_SUB_ACTION_CURTAIN_REACT_PERFORM
} EnKnightSubActionCurtainReact;

typedef enum {
    /* 0 */ KNIGHT_CLOSE_CURTAIN_ACTION_0,
    /* 1 */ KNIGHT_CLOSE_CURTAIN_ACTION_1,
    /* 2 */ KNIGHT_CLOSE_CURTAIN_ACTION_2
} EnKnightCloseCurtainAction;

typedef enum KnightBodyPart {
    /*  0 */ KNIGHT_BODYPART_JAW,
    /*  1 */ KNIGHT_BODYPART_TORSO,
    /*  2 */ KNIGHT_BODYPART_PELVIS,
    /*  3 */ KNIGHT_BODYPART_LEFT_UPPER_ARM,
    /*  4 */ KNIGHT_BODYPART_LEFT_FOREARM,
    /*  5 */ KNIGHT_BODYPART_SHIELD,
    /*  6 */ KNIGHT_BODYPART_RIGHT_UPPER_ARM,
    /*  7 */ KNIGHT_BODYPART_RIGHT_FOREARM,
    /*  8 */ KNIGHT_BODYPART_SWORD,
    /*  9 */ KNIGHT_BODYPART_LEFT_LEG_UPPER,
    /* 10 */ KNIGHT_BODYPART_LEFT_LEG_LOWER,
    /* 11 */ KNIGHT_BODYPART_LEFT_FOOT,
    /* 12 */ KNIGHT_BODYPART_RIGHT_LEG_UPPER,
    /* 13 */ KNIGHT_BODYPART_RIGHT_LEG_LOWER,
    /* 14 */ KNIGHT_BODYPART_RIGHT_FOOT,
    /* 15 */ KNIGHT_BODYPART_MAX
} KnightBodyPart;

typedef struct EnKnight {
    /* 0x000 */ Actor actor;
    /* 0x144 */ u16 curtainsLightRayAngle;
    /* 0x146 */ s16 randTimer;
    /* 0x148 */ u8 subAction;
    /* 0x14A */ s16 timers[3];
    /* 0x150 */ UNK_TYPE1 unk_150;
    /* 0x151 */ u8 inCurtainCutscene;
    /* 0x152 */ u8 doBgChecks;
    /* 0x153 */ u8 closeCurtainAction;
    /* 0x154 */ u8 roomLightingState;
    /* 0x156 */ s16 invincibilityTimer;
    /* 0x158 */ s16 damageFlashTimer;
    /* 0x15A */ s16 strafeTarget;
    /* 0x15C */ s16 strafeMaxStep;
    /* 0x15E */ s16 effectTimer;
    /* 0x160 */ s16 breathAlpha;
    /* 0x162 */ u8 isHeadless;
    /* 0x164 */ Vec3f afterImageOffset;
    /* 0x170 */ s16 pitchToPlayer;
    /* 0x172 */ s16 yawToPlayer;
    /* 0x174 */ s16 leftLegUpperRotation;
    /* 0x176 */ s16 rightLegUpperRotation;
    /* 0x178 */ s16 leftLegLowerRotation;
    /* 0x17A */ s16 rightLegLowerRotation;
    /* 0x17C */ f32 bodyAlpha;
    /* 0x180 */ f32 shadowAlphaFactor;
    /* 0x184 */ s16 neckYaw;
    /* 0x186 */ s16 neckYawTarget;
    /* 0x188 */ s16 neckRoll;
    /* 0x18A */ s16 neckRollTarget;
    /* 0x18C */ s16 neckRotationMaxStep;
    /* 0x18E */ s16 neckRotationStepScale;
    /* 0x190 */ u8 swordColliderActive;
    /* 0x191 */ u8 shieldColliderActive;
    /* 0x192 */ u8 canRetreat;
    /* 0x193 */ u8 shieldingInvulnerabilityTimer;
    /* 0x194 */ SkelAnime skelAnime;
    /* 0x1D8 */ f32 animEndFrame;
    /* 0x1DC */ Vec3f bodyPartsPos[15];
    /* 0x290 */ u8 igosCurtainReaction;
    /* 0x291 */ u8 lightRayDamageTimer;
    /* 0x292 */ u8 retreatWhileShielding;
    /* 0x294 */ f32 drawDmgEffScale;
    /* 0x298 */ f32 drawDmgEffIceMeltingScale;
    /* 0x29C */ f32 drawDmgEffAlpha;
    /* 0x2A0 */ u8 drawDmgEffType;
    /* 0x2A1 */ u8 drawDmgEffState;
    /* 0x2A2 */ s16 drawDmgEffDuration;
    /* 0x2A4 */ f32 animMovementX;
    /* 0x2A8 */ f32 animMovementZ;
    /* 0x2AC */ Vec3f feetPositions[2];
    /* 0x2C4 */ Vec3s jointTable[KNIGHT_LIMB_MAX];
    /* 0x372 */ Vec3s morphTable[KNIGHT_LIMB_MAX];
    /* 0x420 */ EnKnightActionFunc actionFunc;
    /* 0x424 */ EnKnightActionFunc prevActionFunc;
    /* 0x428 */ s16 jawRotation;
    /* 0x42C */ f32 jawRotationAmplitude;
    /* 0x430 */ f32 jawRotationAmplitudeTarget;
    /* 0x434 */ Vec3f breathBasePos;
    /* 0x440 */ Vec3f retreatTowards;
    /* 0x44C */ u8 blureState;
    /* 0x450 */ f32 blureAlpha;
    /* 0x454 */ Vec3f blureTranslation;
    /* 0x460 */ Vec3f blureRotation;
    /* 0x46C */ f32 swordScale;
    /* 0x470 */ f32 shieldScale;
    /* 0x474 */ f32 shieldLightReflectionScale;
    /* 0x478 */ s16 shieldHitTimer;
    /* 0x47C */ Vec3f shieldParticlesPos;
    /* 0x488 */ ColliderCylinder headAttackCollider;
    /* 0x4D4 */ ColliderJntSph swordCollider;
    /* 0x4F4 */ ColliderJntSphElement swordColliderElements[1];
    /* 0x534 */ ColliderJntSph shieldCollider;
    /* 0x554 */ ColliderJntSphElement shieldColliderElements[1];
    /* 0x594 */ ColliderJntSph bodyCollider;
    /* 0x5B4 */ ColliderJntSphElement bodyColliderElements[2];
    /* 0x634 */ ColliderCylinder headCollider;
    /* 0x680 */ u8 inCurtainReaction;
    /* 0x684 */ u32 csTimer;
    /* 0x688 */ s16 csState;
    /* 0x68A */ s16 subCamId;
    /* 0x68C */ Vec3f subCamEye;
    /* 0x698 */ Vec3f subCamAt;
    /* 0x6A4 */ f32 csStepValue;
    /* 0x6A8 */ UNK_TYPE4 unk_6A8;
    /* 0x6AC */ s16 csPlayerYaw;
    /* 0x6B0 */ f32 subCamFov;
    /* 0x6B4 */ u16 walkSfx;
    /* 0x6B6 */ u16 pauseSfx;
    /* 0x6B8 */ u16 laughSfx;
    /* 0x6BA */ UNK_TYPE2 unk_6BA; // unused sfx id space?
    /* 0x6BC */ u16 attackSfx;
    /* 0x6BE */ u16 damagedSfx;
    /* 0x6C0 */ u16 defeatedSfx;
    /* 0x6C2 */ u16 voiceSfx;
    /* 0x6C4 */ u8 seenCaptainsHat;
    /* 0x6C8 */ Vec3f projectedPos;
} EnKnight; // size = 0x6D4

#endif // Z_EN_KNIGHT_H
