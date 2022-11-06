#ifndef Z_BG_SPOT08_ICEBLOCK_H
#define Z_BG_SPOT08_ICEBLOCK_H

#include "ultra64.h"
#include "global.h"

struct BgSpot08Iceblock;

typedef void (*BgSpot08IceblockActionFunc)(struct BgSpot08Iceblock*, PlayState*);

typedef struct BgSpot08Iceblock {
    /* 0x0000 */ DynaPolyActor dyna;
    /* 0x0164 */ BgSpot08IceblockActionFunc actionFunc;
    /* 0x0168 */ Vec3f surfaceNormal;
    /* 0x0170 */ Vec3f normalDelta; // y always 0
    /* 0x0180 */ Vec3f rotationAxis;
    /* 0x018C */ s16 bobPhaseSlow;
    /* 0x018E */ s16 bobPhaseFast;
    /* 0x0190 */ s16 bobIncrSlow;
    /* 0x0192 */ s16 bobIncrFast;
    /* 0x0194 */ f32 sinkOffset;
    /* 0x0198 */ f32 bobOffset;
} BgSpot08Iceblock; // size = 0x019C

// Params
/**
 * 0x200: Shape
 * 0x200 is the ice ramp where Jabu is as child
 */

/**
 * 0x100: Twins
 * 0x100 is the spawned twin platform
 */

/**
 * 0xF0: Size
 * 0x00 is large
 * 0x10 is medium
 * 0x20 is small
 */

/**
 * 0xF: Action
 * 0 Floating, nonrotating
 * 1 same as 0, but does not roll as much (?)
 * 2 Floating, rotating
 * 3 Floating orbiting twins
 * 4 Completely static, does nothing
 */

#endif
