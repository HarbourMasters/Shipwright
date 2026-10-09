#ifndef Z_EN_GUARD_NUTS_H
#define Z_EN_GUARD_NUTS_H

#include "global.h"
#include "objects/object_dnk/object_dnk.h"

struct EnGuardNuts;

typedef void (*EnGuardNutsActionFunc)(struct EnGuardNuts*, PlayState*);

typedef struct EnGuardNuts {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ Vec3s jointTable[DEKU_PALACE_GUARD_LIMB_MAX];
    /* 0x1CA */ Vec3s morphTable[DEKU_PALACE_GUARD_LIMB_MAX];
    /* 0x20C */ EnGuardNutsActionFunc actionFunc;
    /* 0x210 */ s16 eyeState;
    /* 0x212 */ s16 blinkTimer;
    /* 0x214 */ s16 timer;
    /* 0x218 */ s32 animIndex;
    /* 0x21C */ s16 state;
    /* 0x21E */ s16 guardNumber; // Unused - Number used to identify guard
    /* 0x220 */ s32 guardTextIndex;
    /* 0x224 */ u8 hasCompletedConversation; // Flag set when player has gone through all text.
    /* 0x228 */ Vec3f guardPos;
    /* 0x234 */ Vec3s headRot;
    /* 0x23A */ Vec3s targetHeadPos;
    /* 0x240 */ f32 animEndFrame;
    /* 0x244 */ ColliderCylinder collider;
} EnGuardNuts; // size = 0x290

#endif // Z_EN_GUARD_NUTS_H
