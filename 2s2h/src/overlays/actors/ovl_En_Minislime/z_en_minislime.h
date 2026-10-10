#ifndef Z_EN_MINISLIME_H
#define Z_EN_MINISLIME_H

#include "global.h"

struct EnMinislime;

typedef void (*EnMinislimeActionFunc)(struct EnMinislime*, PlayState*);

#define MINISLIME_NUM_SPAWN 15

typedef enum {
    /* 0x0 */ MINISLIME_DISAPPEAR, // Either Bigslime is active or Gekko is defeated
    /* 0x1 */ MINISLIME_INIT_FALL, // Initial fall when Gekko first calls
    /* 0x2 */ MINISLIME_BREAK_BIGSLIME,
    /* 0x3 */ MINISLIME_IDLE,
    /* 0x4 */ MINISLIME_FORM_BIGSLIME,
    /* 0x5 */ MINISLIME_SETUP_DISAPPEAR,
    /* 0x6 */ MINISLIME_UNUSED,
    /* 0x7 */ MINISLIME_DEFEAT_IDLE,
    /* 0x8 */ MINISLIME_DEFEAT_MELT,
    /* 0x9 */ MINISLIME_DESPAWN,
    /* 0xA */ MINISLIME_SETUP_GEKKO_THROW,
    /* 0xB */ MINISLIME_GEKKO_THROW
} EnMinislimeState;

typedef enum {
    /* 0 */ MINISLIME_INACTIVE_STATE,
    /* 1 */ MINISLIME_ACTIVE_CONTINUE_STATE,
    /* 2 */ MINISLIME_ACTIVE_INIT_STATE
} EnMinislimeActiveState;

typedef struct EnMinislime {
    /* 0x000 */ Actor actor;
    /* 0x144 */ EnMinislimeActionFunc actionFunc;
    /* 0x148 */ u8 frozenAlpha;
    /* 0x149 */ u8 id; // 0-14, each of the 15 minislimes gets a unique number
    /* 0x14A */ union {
                    s16 frozenTimer;
                    s16 bounceTimer;
                    s16 growShrinkTimer;
                    s16 idleTimer;
                    s16 knockbackTimer;
                    s16 meltTimer;
                    s16 throwTimer;
                };
    /* 0x14C */ s16 attackTimer;
    /* 0x150 */ f32 frozenScale;
    /* 0x154 */ Vec3f shakeRefPos;
    /* 0x160 */ ColliderCylinder collider;
} EnMinislime; // size = 0x1AC

#endif // Z_EN_MINISLIME_H
