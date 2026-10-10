#ifndef Z_DEMO_EFFECT_H
#define Z_DEMO_EFFECT_H

#include "global.h"
#include "z64curve.h"

struct DemoEffect;

typedef void (*DemoEffectActionFunc)(struct DemoEffect*, PlayState*);

#define DEMO_EFFECT_GET_TYPE(thisx) ((thisx)->params & 0xFF)

typedef enum {
    /* 0 */ DEMO_EFFECT_TIMEWARP_TIMEBLOCK_LARGE,
    /* 1 */ DEMO_EFFECT_TIMEWARP_TIMEBLOCK_SMALL,
    /* 2 */ DEMO_EFFECT_TIMEWARP_LIGHTBLOCK_LARGE,
    /* 3 */ DEMO_EFFECT_TIMEWARP_LIGHTBLOCK_VERY_LARGE,
    /* 4 */ DEMO_EFFECT_TYPE_LIGHT_BASE,
    /* 4 */ DEMO_EFFECT_TYPE_LIGHT_DARK_YELLOW = DEMO_EFFECT_TYPE_LIGHT_BASE,
    /* 5 */ DEMO_EFFECT_TYPE_LIGHT_PINK,
    /* 6 */ DEMO_EFFECT_TYPE_LIGHT_GREEN,
    /* 7 */ DEMO_EFFECT_TYPE_LIGHT_BLUE,
    /* 8 */ DEMO_EFFECT_TYPE_LIGHT_YELLOW // Unused
} DemoEffectType;

typedef struct DemoEffect {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelCurve skelCurve;
    /* 0x164 */ u8 initObjectSlot;
    /* 0x165 */ u8 primXluColor[3];
    /* 0x168 */ u8 envXluColor[3];
    /* 0x16C */ s16 timer;
    /* 0x170 */ DemoEffectActionFunc initActionFunc;
    /* 0x174 */ ActorFunc initDrawFunc;
    /* 0x178 */ DemoEffectActionFunc actionFunc;
} DemoEffect; // size = 0x17C

#endif // Z_DEMO_EFFECT_H
