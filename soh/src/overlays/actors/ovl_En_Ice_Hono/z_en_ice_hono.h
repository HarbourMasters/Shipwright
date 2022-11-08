#ifndef Z_EN_ICE_HONO_H
#define Z_EN_ICE_HONO_H

#include "ultra64.h"
#include "global.h"

struct EnIceHono;

typedef void (*EnIceHonoActionFunc)(struct EnIceHono*, PlayState*);

typedef struct EnIceHono {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ EnIceHonoActionFunc actionFunc;
    /* 0x0150 */ s16 alpha;
    /* 0x0152 */ s16 timer;
    /* 0x0154 */ s16 unk_154;
    /* 0x0156 */ s16 unk_156;
    /* 0x0158 */ f32 smallFlameTargetYScale;
    /* 0x015C */ ColliderCylinder collider;
    /* 0x01A8 */ LightNode* lightNode;
    /* 0x01AC */ LightInfo lightInfo;
} EnIceHono; // size = 0x01BC

#endif
