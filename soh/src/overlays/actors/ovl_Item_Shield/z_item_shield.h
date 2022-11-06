#ifndef Z_ITEM_SHIELD_H
#define Z_ITEM_SHIELD_H

#include "ultra64.h"
#include "global.h"

struct ItemShield;

typedef void (*ItemShieldActionFunc)(struct ItemShield*, PlayState*);

typedef struct ItemShield {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ ColliderCylinder collider;
    /* 0x0198 */ s16 unk_198;
    /* 0x019A */ s16 timer;
    /* 0x019C */ s16 unk_19C;
    /* 0x019E */ u8 unk_19E[8];
    /* 0x01A8 */ Vec3f unk_1A8[8];
    /* 0x0208 */ ItemShieldActionFunc actionFunc;
} ItemShield; // size = 0x020C

#endif
