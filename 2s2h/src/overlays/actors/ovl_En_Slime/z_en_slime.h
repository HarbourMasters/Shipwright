#ifndef Z_EN_SLIME_H
#define Z_EN_SLIME_H

#include "global.h"
#include "objects/object_slime/object_slime.h"
#include "objects/gameplay_keep/gameplay_keep.h"

struct EnSlime;

typedef void (*EnSlimeActionFunc)(struct EnSlime*, PlayState*);


#define EN_SLIME_GET_TYPE(thisx) ((thisx)->params)
#define EN_SLIME_GET_REVIVE_TIME(thisx) ((((thisx)->params) >> 8) & 0xFF)

typedef enum EnSlimeType {
    /* 0 */ EN_SLIME_TYPE_BLUE,
    /* 1 */ EN_SLIME_TYPE_GREEN,
    /* 2 */ EN_SLIME_TYPE_YELLOW,
    /* 3 */ EN_SLIME_TYPE_RED,
    /* 4 */ EN_SLIME_TYPE_MAX
} EnSlimeType;

typedef enum EnSlimeBodyPart {
    /* 0 */ EN_SLIME_BODYPART_0,
    /* 1 */ EN_SLIME_BODYPART_1,
    /* 2 */ EN_SLIME_BODYPART_2,
    /* 3 */ EN_SLIME_BODYPART_3,
    /* 4 */ EN_SLIME_BODYPART_4,
    /* 5 */ EN_SLIME_BODYPART_MAX
} EnSlimeBodyPart;

typedef struct EnSlime {
    /* 0x000 */ Actor actor;
    /* 0x144 */ EnSlimeActionFunc actionFunc;
    /* 0x148 */ u8 iceBlockTimer;
    /* 0x149 */ u8 eyeTexIndex;
    /* 0x14A */ u8 drawDmgEffType;
    /* 0x14C */ s16 timer;
    /* 0x14E */ s16 idleRotY;
    /* 0x150 */ s16 reviveRotY;
    /* 0x152 */ s16 reviveTime;
    /* 0x154 */ union {
                    Vec3f iceBlockSnapPos;
                    Vec3f wobbleRot;
                };
    /* 0x160 */ TexturePtr itemDropTex;
    /* 0x164 */ f32 drawDmgEffAlpha;
    /* 0x168 */ f32 drawDmgEffScale;
    /* 0x16C */ f32 drawDmgEffFrozenSteamScale;
    /* 0x170 */ f32 distLimit;
    /* 0x174 */ Vec3f reviveScale;
    /* 0x180 */ Vec3f bodyPartsPos[EN_SLIME_BODYPART_MAX];
    /* 0x1BC */ ColliderCylinder collider;
} EnSlime; // size = 0x208

#endif // Z_EN_SLIME_H
