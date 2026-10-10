#ifndef Z_ARROW_FIRE_H
#define Z_ARROW_FIRE_H

#include "global.h"

struct ArrowFire;

typedef void (*ArrowFireActionFunc)(struct ArrowFire*, PlayState*);

typedef struct ArrowFire {
    /* 0x000 */ Actor actor;
    /* 0x144 */ ColliderQuad collider1;
    /* 0x1C4 */ ColliderQuad collider2;
    /* 0x244 */ Vec3f firedPos;
    /* 0x250 */ f32 height;
    /* 0x254 */ f32 screenFillIntensity;
    /* 0x258 */ ArrowFireActionFunc actionFunc;
    /* 0x25C */ s16 radius;
    /* 0x25E */ u16 timer;
    /* 0x260 */ u8 alpha;
} ArrowFire; // size = 0x264


#endif // Z_ARROW_FIRE_H
