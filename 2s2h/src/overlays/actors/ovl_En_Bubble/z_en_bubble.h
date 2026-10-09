#ifndef Z_EN_BUBBLE_H
#define Z_EN_BUBBLE_H

#include "global.h"
#include "objects/object_bubble/object_bubble.h"

struct EnBubble;

typedef void (*EnBubbleActionFunc)(struct EnBubble*, PlayState*);

typedef struct EnBubble {
    /* 0x000 */ Actor actor;
    /* 0x144 */ EnBubbleActionFunc actionFunc;
    /* 0x148 */ ColliderJntSph colliderSphere;
    /* 0x168 */ ColliderJntSphElement colliderElements[2];
    /* 0x1F4 */ Vec3f unk1F4; // set but never used
    /* 0x1F4 */ Vec3f unk1F8; // randomly generated, set but never used
    /* 0x200 */ s16 timer; // set to 8 when about to pop
    /* 0x202 */ s16 explosionCountdown;
    /* 0x204 */ UNK_TYPE1 pad204[4]; // unused
    /* 0x208 */ f32 modelRotSpeed;
    /* 0x20C */ f32 modelEllipticity;
    /* 0x210 */ f32 unk_210; // set to 1.0f, never used
    /* 0x214 */ f32 unk_214; // set to 1.0f, never used
    /* 0x218 */ f32 modelWidth;
    /* 0x21C */ f32 modelHeight;
    /* 0x220 */ u8 bounceCount;
    /* 0x224 */ Vec3f bounceDirection;
    /* 0x230 */ Vec3f velocityFromBounce;
    /* 0x23C */ Vec3f normalizedBumpVelocity;
    /* 0x248 */ Vec3f velocityFromBump;
    /* 0x254 */ f32 yVelocity;
} EnBubble; // size = 0x258

#endif // Z_EN_BUBBLE_H
