#ifndef Z_EN_DEMO_HEISHI_H
#define Z_EN_DEMO_HEISHI_H

#include "global.h"
#include "objects/object_sdn/object_sdn.h"

struct EnDemoheishi;

typedef void (*EnDemoheishiActionFunc)(struct EnDemoheishi*, PlayState*);

typedef struct EnDemoheishi {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ Vec3s jointTable[SOLDIER_LIMB_MAX];
    /* 0x1EE */ Vec3s morphTable[SOLDIER_LIMB_MAX];
    /* 0x254 */ EnDemoheishiActionFunc actionFunc;
    /* 0x258 */ s16 headRotY;
    /* 0x25A */ s16 headRotX;
    /* 0x25C */ s16 headRotZ;
    /* 0x25E */ s16 headRotYTarget;
    /* 0x260 */ s16 headRotXTarget;
    /* 0x262 */ s16 headRotZTarget;
    /* 0x264 */ s32 animIndex;
    /* 0x268 */ s16 timer;
    /* 0x26C */ f32 animEndFrame;
    /* 0x270 */ s16 isTalking;
    /* 0x272 */ s16 textIdIndex;
    /* 0x274 */ ColliderCylinder colliderCylinder;
} EnDemoheishi; // size = 0x2C0

#endif // Z_EN_DEMO_HEISHI_H
