#ifndef Z_EN_HEISHI_H
#define Z_EN_HEISHI_H

#include "global.h"
#include "objects/object_sdn/object_sdn.h"

struct EnHeishi;

typedef void (*EnHeishiActionFunc)(struct EnHeishi*, PlayState*);

typedef struct EnHeishi {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ Vec3s jointTable[SOLDIER_LIMB_MAX];
    /* 0x1EE */ Vec3s morphTable[SOLDIER_LIMB_MAX];
    /* 0x254 */ EnHeishiActionFunc actionFunc;
    /* 0x258 */ s16 headRotY;
    /* 0x25A */ s16 headRotX;
    /* 0x25C */ s16 headRotZ;
    /* 0x25E */ s16 headRotYTarget;
    /* 0x260 */ s16 headRotXTarget;
    /* 0x262 */ s16 headRotZTarget;
    /* 0x264 */ s32 animIndex;
    /* 0x268 */ s32 paramsCopy;
    /* 0x26C */ s32 shouldSetHeadRotation;
    /* 0x270 */ s16 timer;
    /* 0x272 */ s16 yawTowardsPlayer;
    /* 0x274 */ f32 animEndFrame;
    /* 0x278 */ s16 unk278;    // set and not used
    /* 0x27C */ UNK_TYPE1 unk_27C[0x8];
    /* 0x284 */ ColliderCylinder colliderCylinder;
} EnHeishi; // size = 0x2D0

#endif // Z_EN_HEISHI_H
