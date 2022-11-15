#ifndef Z_EN_BOOM_H
#define Z_EN_BOOM_H

#include "ultra64.h"
#include "global.h"

struct EnBoom;

typedef void (*EnBoomActionFunc)(struct EnBoom*, PlayState*);

typedef struct EnBoom {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ ColliderQuad collider;
    /* 0x01CC */ Actor* moveTo; // actor boomerang moves toward
    /* 0x01D0 */ Actor* grabbed; // actor grabbed by the boomerang
    /* 0x01D4 */ u8 returnTimer; // returns to Link when 0
    /* 0x01D5 */ u8 activeTimer; // increments once every update
    /* 0x01D8 */ s32 effectIndex;
    /* 0x01DC */ WeaponInfo boomerangInfo;
    /* 0x01F8 */ EnBoomActionFunc actionFunc;
} EnBoom; // size = 0x01FC

#endif
