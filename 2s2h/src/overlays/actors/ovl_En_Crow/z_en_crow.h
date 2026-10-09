#ifndef Z_EN_CROW_H
#define Z_EN_CROW_H

#include "global.h"
#include "objects/object_crow/object_crow.h"

typedef enum {
    /* 0 */ GUAY_TYPE_NORMAL,
    /* 1 */ GUAY_TYPE_MEGA
} GuayType;

struct EnCrow;

typedef void (*EnCrowActionFunc)(struct EnCrow*, PlayState*);

typedef enum GuayBodyPart {
    /* 0 */ GUAY_BODYPART_BODY,
    /* 1 */ GUAY_BODYPART_RIGHT_WING_TIP,
    /* 2 */ GUAY_BODYPART_LEFT_WING_TIP,
    /* 3 */ GUAY_BODYPART_TAIL,
    /* 4 */ GUAY_BODYPART_MAX
} GuayBodyPart;

typedef struct EnCrow {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ EnCrowActionFunc actionFunc;
    /* 0x18C */ u8 drawDmgEffType;
    /* 0x18E */ s16 timer;
    /* 0x190 */ s16 pitchTarget;
    /* 0x192 */ s16 yawTarget;
    /* 0x194 */ Vec3s jointTable[OBJECT_CROW_LIMB_MAX];
    /* 0x1CA */ Vec3s morphTable[OBJECT_CROW_LIMB_MAX];
    /* 0x200 */ ColliderJntSph collider;
    /* 0x220 */ ColliderJntSphElement colliderElements[1];
    /* 0x260 */ Vec3f bodyPartsPos[GUAY_BODYPART_MAX];
    /* 0x290 */ f32 drawDmgEffAlpha;
    /* 0x294 */ f32 drawDmgEffFrozenSteamScale;
    /* 0x298 */ f32 drawDmgEffScale;
} EnCrow; // size = 0x29C

#endif // Z_EN_CROW_H
