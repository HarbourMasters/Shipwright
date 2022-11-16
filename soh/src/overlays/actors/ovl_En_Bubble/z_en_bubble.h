#ifndef Z_EN_BUBBLE_H
#define Z_EN_BUBBLE_H

#include "ultra64.h"
#include "global.h"

struct EnBubble;

typedef void (*EnBubbleActionFunc)(struct EnBubble*, PlayState*);

typedef struct EnBubble {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ EnBubbleActionFunc actionFunc;
    /* 0x0150 */ ColliderJntSph colliderSphere;
    /* 0x0170 */ ColliderJntSphElement colliderSphereItems[2];
    /* 0x01F0 */ Vec3f unk_1F0; // set but never used
    /* 0x01FC */ Vec3f unk_1FC; // randomly generated, set but never used
    /* 0x0208 */ s16 unk_208;   // set to 8 when about to pop
    /* 0x020A */ s16 explosionCountdown;
    /* 0x020C */ char unk_20C[4]; // unused
    /* 0x0210 */ f32 graphicRotSpeed;
    /* 0x0214 */ f32 graphicEccentricity;
    /* 0x0218 */ f32 unk_218; // set to 1.0f, never used
    /* 0x021C */ f32 unk_21C; // set to 1.0f, never used
    /* 0x0220 */ f32 expansionWidth;
    /* 0x0224 */ f32 expansionHeight;
    /* 0x0228 */ u8 bounceCount;
    /* 0x022C */ Vec3f bounceDirection;
    /* 0x0238 */ Vec3f velocityFromBounce;
    /* 0x0244 */ Vec3f normalizedBumpVelocity;
    /* 0x0250 */ Vec3f velocityFromBump;
    /* 0x025C */ f32 sinkSpeed;
} EnBubble; // size = 0x0260

#endif
