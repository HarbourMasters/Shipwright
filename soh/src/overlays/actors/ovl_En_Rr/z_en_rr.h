#ifndef Z_EN_RR_H
#define Z_EN_RR_H

#include "ultra64.h"
#include "global.h"

struct EnRr;

typedef void (*EnRrActionFunc)(struct EnRr*, PlayState*);

typedef struct {
    /* 0x00 */ f32 height;
    /* 0x04 */ f32 heightTarget;
    /* 0x08 */ Vec3f scale;
    /* 0x14 */ Vec3f scaleTarget;
    /* 0x20 */ Vec3f scaleMod;
    /* 0x2C */ Vec3f rotTarget;
    /* 0x38 */ Vec3s rot;
} EnRrBodySegment; // size = 0x40

typedef struct EnRr {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ EnRrActionFunc actionFunc;
    /* 0x0150 */ ColliderCylinder collider1;
    /* 0x019C */ ColliderCylinder collider2;
    /* 0x01E8 */ s16 frameCount;
    /* 0x01EA */ s16 actionTimer;
    /* 0x01EC */ s16 scrollTimer;
    /* 0x01EE */ s16 grabTimer;
    /* 0x01F0 */ s16 invincibilityTimer;
    /* 0x01F2 */ s16 effectTimer;
    /* 0x01F4 */ s16 ocTimer;
    /* 0x01F6 */ s16 segMovePhase; // phase angle for wobble and pulsing motion
    /* 0x01F8 */ f32 segPhaseVel; // rate at which motion phase changes
    /* 0x01FC */ f32 segPhaseVelTarget;
    /* 0x0200 */ f32 segPulsePhaseDiff; // Phase diff between segment pulses. Affects how wave-y the pulse is.
    /* 0x0204 */ f32 segWobblePhaseDiffX; // Phase diff between segment X rot. Affects how circular the wobble is.
    /* 0x0208 */ f32 segWobbleXTarget;
    /* 0x020C */ f32 segWobblePhaseDiffZ; // Phase diff between segment Z rot. Affects how circular the wobble is.
    /* 0x0210 */ f32 segWobbleZTarget;
    /* 0x0214 */ f32 pulseSize; // Amplitude of the scale pulsations
    /* 0x0218 */ f32 pulseSizeTarget;
    /* 0x021C */ f32 wobbleSize; // Amplitude of the wobbling motion
    /* 0x0220 */ f32 wobbleSizeTarget;
    /* 0x0224 */ EnRrBodySegment bodySegs[5];
    /* 0x0364 */ f32 segMoveRate;
    /* 0x0368 */ f32 shrinkRate;
    /* 0x036C */ f32 swallowOffset;
    /* 0x0370 */ u8 reachState;
    /* 0x0371 */ u8 isDead;
    /* 0x0372 */ u8 eatenShield;
    /* 0x0373 */ u8 eatenTunic;
    /* 0x0374 */ u8 dropType;
    /* 0x0375 */ u8 retreat;
    /* 0x0376 */ u8 stopScroll;
    /* 0x0378 */ s16 hasPlayer;
    /* 0x037C */ Vec3f mouthPos;
    /* 0x0388 */ Vec3f effectPos[5];
    /* 0x03C4 */ char unk_3C4[0x2000]; //! @bug This is a huge amount of wasted memory.
} EnRr; // size = 0x23C4

#endif
