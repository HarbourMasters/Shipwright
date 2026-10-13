#ifndef Z_BOSS_HAKUGIN_H
#define Z_BOSS_HAKUGIN_H

#include "global.h"
#include "objects/object_boss_hakugin/object_boss_hakugin.h"
#include "overlays/effects/ovl_Effect_Ss_Fhg_Flash/z_eff_ss_fhg_flash.h"

struct BossHakugin;

#define GOHT_SHADOW_TEX_WIDTH 64
#define GOHT_SHADOW_TEX_HEIGHT 64
#define GOHT_SHADOW_TEX_SIZE ((s32)sizeof(u8[GOHT_SHADOW_TEX_HEIGHT][GOHT_SHADOW_TEX_WIDTH]))

typedef void (*BossHakuginActionFunc)(struct BossHakugin*, PlayState*);


typedef enum GohtRockEffectType {
    /* 0 */ GOHT_ROCK_EFFECT_TYPE_BOULDER,
    /* 1 */ GOHT_ROCK_EFFECT_TYPE_STALACTITE
} GohtRockEffectType;

typedef struct GohtRockEffect {
    /* 0x00 */ Vec3f pos;
    /* 0x0C */ Vec3f velocity;
    /* 0x18 */ s16 timer;
    /* 0x1A */ s16 type; // See GohtRockEffectType
    /* 0x1C */ Vec3s rot;
    /* 0x24 */ f32 scale;
} GohtRockEffect; // size = 0x28

typedef struct GohtCrushingRock {
    /* 0x00 */ Vec3f pos;
    /* 0x0C */ f32 targetPosY;
    /* 0x10 */ f32 fallingSpeed;
    /* 0x14 */ UNK_TYPE1 pad_14[0x8];
    /* 0x1C */ Vec3s rot;
    /* 0x24 */ f32 scale;
} GohtCrushingRock; // size = 0x28

#define GOHT_ROCK_EFFECT_COUNT 180
#define GOHT_CRUSHING_ROCK_COUNT 36
#define GOHT_ELECTRIC_BALL_COUNT_MAX 10

typedef struct GohtLightningSegment {
    /* 0x00 */ Vec3f pos;
    /* 0x0C */ s16 alpha;
    /* 0x0E */ Vec3s rot;
    /* 0x14 */ ColliderTris collider;
    /* 0x34 */ ColliderTrisElement colliderElements[1];
} GohtLightningSegment; // size = 0x90

#define GOHT_LIGHTNING_SEGMENT_COUNT 20

typedef struct GohtMalfunctionEffect {
    /* 0x00 */ Vec3f pos;
    /* 0x0C */ f32 scaleXY;
    /* 0x10 */ s16 alpha;
    /* 0x12 */ s16 timer;
} GohtMalfunctionEffect; // size = 0x14

#define GOHT_MALFUNCTION_NUM_TYPES (FHGFLASH_SHOCK_GOHT_MAX - FHGFLASH_SHOCK_GOHT_BACK_LEFT_THIGH)
#define GOHT_MALFUNCTION_EFFECTS_PER_TYPE 15

typedef enum GohtBodyPart {
    /*  0 */ GOHT_BODYPART_PELVIS,
    /*  1 */ GOHT_BODYPART_THORAX,
    /*  2 */ GOHT_BODYPART_HEAD,
    /*  3 */ GOHT_BODYPART_FRONT_RIGHT_UPPER_LEG,
    /*  4 */ GOHT_BODYPART_FRONT_RIGHT_LOWER_LEG,
    /*  5 */ GOHT_BODYPART_FRONT_RIGHT_HOOF,
    /*  6 */ GOHT_BODYPART_FRONT_LEFT_UPPER_LEG,
    /*  7 */ GOHT_BODYPART_FRONT_LEFT_LOWER_LEG,
    /*  8 */ GOHT_BODYPART_FRONT_LEFT_HOOF,
    /*  9 */ GOHT_BODYPART_BACK_RIGHT_THIGH,
    /* 10 */ GOHT_BODYPART_BACK_RIGHT_SHIN,
    /* 11 */ GOHT_BODYPART_BACK_RIGHT_HOOF,
    /* 12 */ GOHT_BODYPART_BACK_LEFT_THIGH,
    /* 13 */ GOHT_BODYPART_BACK_LEFT_SHIN,
    /* 14 */ GOHT_BODYPART_BACK_LEFT_HOOF,
    /* 15 */ GOHT_BODYPART_MAX
} GohtBodyPart;

typedef enum GohtColliderBodyPart {
    /*  0 */ GOHT_COLLIDER_BODYPART_HEAD,
    /*  1 */ GOHT_COLLIDER_BODYPART_THORAX,
    /*  2 */ GOHT_COLLIDER_BODYPART_PELVIS,
    /*  3 */ GOHT_COLLIDER_BODYPART_FRONT_RIGHT_UPPER_LEG,
    /*  4 */ GOHT_COLLIDER_BODYPART_FRONT_RIGHT_LOWER_LEG,
    /*  5 */ GOHT_COLLIDER_BODYPART_FRONT_RIGHT_HOOF,
    /*  6 */ GOHT_COLLIDER_BODYPART_FRONT_LEFT_UPPER_LEG,
    /*  7 */ GOHT_COLLIDER_BODYPART_FRONT_LEFT_LOWER_LEG,
    /*  8 */ GOHT_COLLIDER_BODYPART_FRONT_LEFT_HOOF,
    /*  9 */ GOHT_COLLIDER_BODYPART_BACK_RIGHT_THIGH,
    /* 10 */ GOHT_COLLIDER_BODYPART_BACK_RIGHT_SHIN,
    /* 11 */ GOHT_COLLIDER_BODYPART_BACK_RIGHT_PASTERN,
    /* 12 */ GOHT_COLLIDER_BODYPART_BACK_RIGHT_HOOF,
    /* 13 */ GOHT_COLLIDER_BODYPART_BACK_LEFT_THIGH,
    /* 14 */ GOHT_COLLIDER_BODYPART_BACK_LEFT_SHIN,
    /* 15 */ GOHT_COLLIDER_BODYPART_BACK_LEFT_PASTERN,
    /* 16 */ GOHT_COLLIDER_BODYPART_BACK_LEFT_HOOF,
    /* 17 */ GOHT_COLLIDER_BODYPART_RIGHT_HORN,
    /* 18 */ GOHT_COLLIDER_BODYPART_LEFT_HORN,
    /* 19 */ GOHT_COLLIDER_BODYPART_MAX
} GohtColliderBodyPart;

typedef struct BossHakugin {
    /* 0x0000 */ Actor actor;
    /* 0x0144 */ SkelAnime skelAnime;
    /* 0x0188 */ BossHakuginActionFunc actionFunc;
    /* 0x018C */ u8 lightningHitSomething;
    /* 0x018D */ u8 preventBoulderSpawnCount;
    /* 0x018E */ u8 preventStalactiteSpawnCount;
    /* 0x018F */ u8 preventBombSpawnCount;
    /* 0x0190 */ u8 blockMalfunctionEffects;
    /* 0x0191 */ u8 malfunctionEffectIndex;
    /* 0x0192 */ u8 electricBallState; // See `GohtElectricBallState`
    /* 0x0193 */ u8 iceAlpha;
    /* 0x0194 */ u8 hasAppliedGoronSpikeBoost;
    /* 0x0195 */ u8 finishedFallingDown;
    /* 0x0196 */ u8 drawDmgEffType;
    /* 0x0198 */ s16 lightOrbRotZ;
    /* 0x019A */ s16 damagedSpeedUpCounter;
    /* 0x019C */ union {
                    s16 nextCutsceneType;
                    s16 timer;
                 };
    /* 0x019E */ s16 targetRotY;
    /* 0x01A0 */ s16 baseRotY;
    /* 0x01A2 */ s16 runOffsetRot;
    /* 0x01A4 */ s16 disableBodyCollidersTimer;
    /* 0x01A6 */ s16 frontHalfRotZ;
    /* 0x01A8 */ s16 chargeUpTimer;
    /* 0x01AA */ s16 electricBallCount;
    /* 0x01AC */ s16 subCamId;
    /* 0x01AE */ s16 deathCutsceneRandomHeadRot;
    /* 0x01B0 */ u32 limbDrawFlags;
    /* 0x01B4 */ s32 direction; // See `GohtDirection`
    /* 0x01B8 */ f32 electricBallSpeed;
    /* 0x01BC */ f32 distToRightWall;
    /* 0x01C0 */ f32 distToLeftWall;
    /* 0x01C4 */ f32 targetSpeed;
    /* 0x01C8 */ f32 chargingLightOrbScale;
    /* 0x01CC */ f32 chargingLightningTranslateZ;
    /* 0x01D0 */ f32 chargingLightningScale;
    /* 0x01D4 */ f32 iceScaleY;
    /* 0x01D8 */ UNK_TYPE1 unk_01D8[0x4]; // probably unused float
    /* 0x01DC */ f32 drawDmgEffScale;
    /* 0x01E0 */ f32 drawDmgEffFrozenSteamScale;
    /* 0x01E4 */ f32 drawDmgEffAlpha;
    /* 0x01E8 */ Vec3s jointTable[GOHT_LIMB_MAX];
    /* 0x02AE */ Vec3s morphTable[GOHT_LIMB_MAX];
    /* 0x0374 */ Vec3s headRot;
    /* 0x037A */ Vec3s subCamRot;
    /* 0x0380 */ Vec3f chargingLightningPos;
    /* 0x038C */ Vec3f subCamEye;
    /* 0x0398 */ Vec3f bodyPartsPos[GOHT_BODYPART_MAX];
    /* 0x044C */ Vec3f transformedPlayerPos;
    /* 0x0458 */ Vec3f sfxPos;
    /* 0x0464 */ Vec3f electricBallSfxPos;
    /* 0x0470 */ LightNode* lightNode;
    /* 0x0474 */ LightInfo lightInfo;
    /* 0x0484 */ ColliderJntSph bodyCollider;
    /* 0x04A4 */ ColliderJntSphElement bodyColliderElements[GOHT_COLLIDER_BODYPART_MAX];
    /* 0x0964 */ ColliderCylinder iceCollider;
    /* 0x09B0 */ Actor* boulders[8];
    /* 0x09D0 */ Actor* stalactites[10];
    /* 0x09F8 */ union {
                     GohtRockEffect rockEffects[GOHT_ROCK_EFFECT_COUNT];
                     GohtCrushingRock crushingRocks[GOHT_CRUSHING_ROCK_COUNT]; 
                 };
    /* 0x2618 */ GohtLightningSegment lightningSegments[GOHT_LIGHTNING_SEGMENT_COUNT]; // A chain of lightning used as an attack
    /* 0x3158 */ GohtMalfunctionEffect malfunctionEffects[GOHT_MALFUNCTION_NUM_TYPES][GOHT_MALFUNCTION_EFFECTS_PER_TYPE]; // Black smoke and electric zaps on body parts as damage accumulates 
    /* 0x3734 */ Vec3f electricBallPos[GOHT_ELECTRIC_BALL_COUNT_MAX];
    /* 0x37AC */ Vec3f electricBallRot;
    /* 0x37B8 */ ColliderSphere electricBallCollider;
} BossHakugin; // size = 0x3810

#endif // Z_BOSS_HAKUGIN_H
