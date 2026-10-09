#ifndef Z_EN_SNOWMAN_H
#define Z_EN_SNOWMAN_H

#include "global.h"
#include "objects/object_snowman/object_snowman.h"

#define EN_SNOWMAN_GET_TYPE(thisx) ((thisx)->params)
#define EN_SNOWMAN_GET_ATTACK_RANGE(thisx) (((thisx)->params >> 8) & 0xFF)

typedef enum {
    /* 0 */ EN_SNOWMAN_TYPE_SMALL,
    /* 1 */ EN_SNOWMAN_TYPE_LARGE,
    /* 2 */ EN_SNOWMAN_TYPE_SPLIT,
    /* 3 */ EN_SNOWMAN_TYPE_SMALL_SNOWBALL,
    /* 4 */ EN_SNOWMAN_TYPE_LARGE_SNOWBALL
} EnSnowmanType;

struct EnSnowman;

typedef void (*EnSnowmanActionFunc)(struct EnSnowman*, PlayState*);

typedef enum EnSnowmanBodyPart {
    /* 0 */ EN_SNOWMAN_BODYPART_0,
    /* 1 */ EN_SNOWMAN_BODYPART_1,
    /* 2 */ EN_SNOWMAN_BODYPART_2,
    /* 3 */ EN_SNOWMAN_BODYPART_3,
    /* 4 */ EN_SNOWMAN_BODYPART_4,
    /* 5 */ EN_SNOWMAN_BODYPART_5,
    /* 6 */ EN_SNOWMAN_BODYPART_6,
    /* 7 */ EN_SNOWMAN_BODYPART_7,
    /* 8 */ EN_SNOWMAN_BODYPART_8,
    /* 9 */ EN_SNOWMAN_BODYPART_MAX
} EnSnowmanBodyPart;

typedef struct EnSnowman {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ SkelAnime snowPileSkelAnime;
    /* 0x1CC */ Vec3s jointTable[EENO_LIMB_MAX];
    /* 0x214 */ Vec3s morphTable[EENO_LIMB_MAX];
    /* 0x25C */ Vec3s snowPileJointTable[EENO_SNOW_PILE_LIMB_MAX];
    /* 0x26E */ Vec3s snowPileMorphTable[EENO_SNOW_PILE_LIMB_MAX];
    /* 0x280 */ EnSnowmanActionFunc actionFunc;
    /* 0x284 */ EnSnowmanActionFunc prevActionFunc;
    /* 0x288 */ u8 isHoldingSnowball;
    /* 0x289 */ u8 combineState;
    /* 0x28A */ u8 turningOnSteepSlope;
    /* 0x28B */ u8 drawDmgEffType;
    /* 0x28C */ union {
                    s16 timer;
                    s16 snowballsToThrowBeforeIdling;
                } work;
    /* 0x28E */ s16 snowPileTargetRotY;
    /* 0x290 */ s16 combineTimer;
    /* 0x294 */ f32 eenoScale;
    /* 0x298 */ union {
                    f32 frameToStartHoldingSnowball;
                    f32 frameToThrowSnowball;
                    f32 targetScaleDuringCombine;
                } fwork;
    /* 0x29C */ f32 attackRange;
    /* 0x2A0 */ f32 drawDmgEffAlpha;
    /* 0x2A4 */ f32 drawDmgEffScale;
    /* 0x2A8 */ Vec3f combinePos;
    /* 0x2B4 */ Vec3f snowballPos;
    /* 0x2C0 */ Vec3f bodyPartsPos[EN_SNOWMAN_BODYPART_MAX];
    /* 0x32C */ ColliderCylinder collider;
} EnSnowman; // size = 0x378

#endif // Z_EN_SNOWMAN_H
