#ifndef Z_DEMO_KEKKAI_H
#define Z_DEMO_KEKKAI_H

#include "ultra64.h"
#include "global.h"

struct DemoKekkai;

typedef void (*DemoKekkaiUpdateFunc)(struct DemoKekkai* this, PlayState* play);

typedef struct DemoKekkai {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ ColliderCylinder collider1;
    /* 0x0198 */ ColliderCylinder collider2;
    /* 0x01E4 */ f32 energyAlpha;
    /* 0x01E8 */ f32 orbScale;
    /* 0x01EC */ f32 barrierScroll;
    /* 0x01F0 */ f32 barrierScrollRate;
    /* 0x01F4 */ u16 timer;
    /* 0x01F6 */ u16 sfxFlag;
    /* 0x01F8 */ DemoKekkaiUpdateFunc updateFunc;
} DemoKekkai; // size = 0x01FC

typedef enum {
    /* 0 */ KEKKAI_TOWER,
    /* 1 */ KEKKAI_WATER,
    /* 2 */ KEKKAI_LIGHT,
    /* 3 */ KEKKAI_FIRE,
    /* 4 */ KEKKAI_SHADOW,
    /* 5 */ KEKKAI_SPIRIT,
    /* 6 */ KEKKAI_FOREST
} DemoKekkaiType;

#endif
