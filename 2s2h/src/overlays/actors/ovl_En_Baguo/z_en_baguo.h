#ifndef Z_EN_BAGUO_H
#define Z_EN_BAGUO_H

#include "global.h"
#include "objects/object_gmo/object_gmo.h"

struct EnBaguo;

typedef void (*EnBaguoActionFunc)(struct EnBaguo*, PlayState*);

/**
 * When a Nejiron explodes, rock particles fly out from where it exploded.
 * This struct governs how these rock particles behave.
 */
typedef struct NejironEffect {
    /* 0x00 */ u8 isEnabled;
    /* 0x04 */ Vec3f pos;
    /* 0x10 */ Vec3f velocity;
    /* 0x1C */ Vec3f accel;
    /* 0x28 */ Vec3s rot;
    /* 0x30 */ f32 scale;
    /* 0x34 */ s16 timer;
} NejironEffect; // size = 0x38

#define EN_BAGUO_EFFECT_COUNT 30

typedef struct EnBaguo {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ Vec3s jointTable[NEJIRON_LIMB_MAX];
    /* 0x19A */ Vec3s morphTable[NEJIRON_LIMB_MAX];
    /* 0x1AC */ EnBaguoActionFunc actionFunc;
    /* 0x1B0 */ s16 eyeIndex;
    /* 0x1B2 */ s16 blinkTimer;
    /* 0x1B4 */ s16 timer;
    /* 0x1B6 */ s16 action;
    /* 0x1B8 */ s16 zRollDirection;
    /* 0x1BC */ f32 maxDistanceFromHome;
    /* 0x1C0 */ u8 bouncedFlag;
    /* 0x1C4 */ Vec3f currentRotation;
    /* 0x1D0 */ Vec3f targetRotation;
    /* 0x1DC */ ColliderJntSph collider;
    /* 0x1FC */ ColliderJntSphElement colliderElements[1];
    /* 0x23C */ NejironEffect effects[EN_BAGUO_EFFECT_COUNT];
} EnBaguo; // size = 0x8CC

#endif // Z_EN_BAGUO_H
