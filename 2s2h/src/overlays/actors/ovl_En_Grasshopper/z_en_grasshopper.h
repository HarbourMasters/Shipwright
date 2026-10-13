#ifndef Z_EN_GRASSHOPPER_H
#define Z_EN_GRASSHOPPER_H

#include "global.h"
#include "objects/object_grasshopper/object_grasshopper.h"

#define DRAGONFLY_GET_TYPE(thisx) ((thisx)->params)

#define DRAGONFLY_PARAMS(type) (type)

struct EnGrasshopper;

typedef void (*EnGrasshopperActionFunc)(struct EnGrasshopper*, PlayState*);

typedef enum EnGrasshopperType {
    /* -1 */ DRAGONFLY_TYPE_UNUSED_NORMAL = -1, // Acts exactly like DRAGONFLY_TYPE_NORMAL
    /*  0 */ DRAGONFLY_TYPE_NORMAL,
    /*  1 */ DRAGONFLY_TYPE_GROWS_WHEN_SPAWNED, // Spawned by EnEncount1
    /*  2 */ DRAGONFLY_TYPE_WOODFALL_TEMPLE_FINAL_ROOM,
    /*  3 */ DRAGONFLY_TYPE_WOODFALL
} EnGrasshopperType;

typedef struct EnGrasshopperEffect {
    /* 0x00 */ u8 isEnabled;
    /* 0x04 */ Vec3f pos;
    /* 0x10 */ Vec3f velocity;
    /* 0x1C */ f32 yaw;
    /* 0x20 */ Vec3f scale;
    /* 0x2C */ s16 timer;
    /* 0x2E */ s16 lightningIndex;
} EnGrasshopperEffect; // size = 0x30

#define DRAGONFLY_EFFECT_COUNT 100

typedef enum EnGrasshopperBodyPart {
    /*  0 */ DRAGONFLY_BODYPART_0,
    /*  1 */ DRAGONFLY_BODYPART_1,
    /*  2 */ DRAGONFLY_BODYPART_2,
    /*  3 */ DRAGONFLY_BODYPART_3,
    /*  4 */ DRAGONFLY_BODYPART_4,
    /*  5 */ DRAGONFLY_BODYPART_5,
    /*  6 */ DRAGONFLY_BODYPART_6,
    /*  7 */ DRAGONFLY_BODYPART_7,
    /*  8 */ DRAGONFLY_BODYPART_8,
    /*  9 */ DRAGONFLY_BODYPART_9,
    /* 10 */ DRAGONFLY_BODYPART_10,
    /* 11 */ DRAGONFLY_BODYPART_11,
    /* 12 */ DRAGONFLY_BODYPART_MAX
} EnGrasshopperBodyPart;

typedef enum EnGrasshopperShadowBodyPart {
    /*  0 */ DRAGONFLY_SHADOW_BODYPART_0,
    /*  1 */ DRAGONFLY_SHADOW_BODYPART_1,
    /*  2 */ DRAGONFLY_SHADOW_BODYPART_2,
    /*  3 */ DRAGONFLY_SHADOW_BODYPART_3,
    /*  4 */ DRAGONFLY_SHADOW_BODYPART_4,
    /*  5 */ DRAGONFLY_SHADOW_BODYPART_5,
    /*  6 */ DRAGONFLY_SHADOW_BODYPART_6,
    /*  7 */ DRAGONFLY_SHADOW_BODYPART_7,
    /*  8 */ DRAGONFLY_SHADOW_BODYPART_8,
    /*  9 */ DRAGONFLY_SHADOW_BODYPART_9,
    /* 10 */ DRAGONFLY_SHADOW_BODYPART_10,
    /* 11 */ DRAGONFLY_SHADOW_BODYPART_11,
    /* 12 */ DRAGONFLY_SHADOW_BODYPART_12,
    /* 13 */ DRAGONFLY_SHADOW_BODYPART_13,
    /* 14 */ DRAGONFLY_SHADOW_BODYPART_MAX
} EnGrasshopperShadowBodyPart;

typedef struct EnGrasshopper {
    /* 0x0000 */ Actor actor;
    /* 0x0144 */ SkelAnime skelAnime;
    /* 0x0188 */ Vec3s jointTable[DRAGONFLY_LIMB_MAX];
    /* 0x0218 */ Vec3s morphTable[DRAGONFLY_LIMB_MAX];
    /* 0x02A8 */ EnGrasshopperActionFunc actionFunc;
    /* 0x02AC */ u8 decision;
    /* 0x02AD */ u8 shouldTurn;
    /* 0x02AE */ UNK_TYPE1 unk_2AE[2];
    /* 0x02B0 */ s16 timer;
    /* 0x02B2 */ s16 waitTimer;
    /* 0x02B4 */ s16 postBankTimer;
    /* 0x02B6 */ s16 drawDmgEffTimer;
    /* 0x02B8 */ s16 drawDmgEffType;
    /* 0x02BC */ f32 drawDmgEffScale;
    /* 0x02C0 */ f32 drawDmgEffFrozenSteamScale;
    /* 0x02C4 */ Vec3f bodyPartsPos[DRAGONFLY_BODYPART_MAX];
    /* 0x0354 */ s16 bodyPartIndex;
    /* 0x0356 */ s16 splashCount;
    /* 0x0358 */ s16 action;
    /* 0x035A */ s16 index; // Has a minor effect on speed for some reason.
    /* 0x035C */ s16 bankState;
    /* 0x035E */ s16 type;
    /* 0x0360 */ f32 dragonflyScale;
    /* 0x0364 */ f32 angularVelocity;
    /* 0x0368 */ f32 animEndFrame;
    /* 0x036C */ f32 approachSpeed;
    /* 0x0370 */ f32 baseFlyHeight;
    /* 0x0374 */ f32 targetPosY;
    /* 0x0376 */ Vec3s targetRot;
    /* 0x0380 */ Vec3f flyingHomePos;
    /* 0x038C */ s16 bobPhase;
    /* 0x038E */ UNK_TYPE1 unk_38E[0x26];
    /* 0x03B4 */ Vec3f targetApproachPos;
    /* 0x03C0 */ Vec3f tailTipPos;
    /* 0x03CC */ Vec3f effectBasePos;
    /* 0x03D8 */ Vec3f shadowBodyPartsPos[DRAGONFLY_LIMB_MAX]; // Should be `DRAGONFLY_SHADOW_BODYPART_MAX`
    /* 0x04F8 */ f32 waterSurface;
    /* 0x04FC */ Vec3f damagedVelocity;
    /* 0x0508 */ Vec3s targetBankRot;
    /* 0x0510 */ ColliderJntSph collider;
    /* 0x0530 */ ColliderJntSphElement colliderElements[2];
    /* 0x0570 */ EnGrasshopperEffect effects[DRAGONFLY_EFFECT_COUNT];
} EnGrasshopper; // size = 0x1870

#endif // Z_EN_GRASSHOPPER_H
