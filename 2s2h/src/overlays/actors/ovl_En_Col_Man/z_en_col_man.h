#ifndef Z_EN_COL_MAN_H
#define Z_EN_COL_MAN_H

#include "global.h"

struct EnColMan;

typedef void (*EnColManActionFunc)(struct EnColMan*, PlayState*);

typedef struct EnColMan {
    /* 0x000 */ Actor actor;
    /* 0x144 */ EnColManActionFunc actionFunc;
    /* 0x148 */ s16 type; // never used, only set
    /* 0x14C */ s32 hasSetRandomValues;
    /* 0x150 */ f32 scale;
    /* 0x154 */ ColliderCylinder collider;
} EnColMan; /* size = 0x1A0 */

typedef enum {
    /* 0 */ EN_COL_MAN_HEART_PIECE,
    /* 1 */ EN_COL_MAN_RECOVERY_HEART,
    /* 2 */ EN_COL_MAN_FALLING_ROCK,
    /* 3 */ EN_COL_MAN_CUTSCENE_BOMB,
    /* 4 */ EN_COL_MAN_GAMEPLAY_BOMB
} EnColManType;

#endif // Z_EN_COL_MAN_H
