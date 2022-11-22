#ifndef Z_DEMO_GEFF_H
#define Z_DEMO_GEFF_H

#include "ultra64.h"
#include "global.h"

#include "../ovl_Demo_Gt/z_demo_gt.h"

struct DemoGeff;

typedef void (*DemoGeffInitFunc)(struct DemoGeff*, PlayState*);
typedef void (*DemoGeffActionFunc)(struct DemoGeff*, PlayState*);
typedef void (*DemoGeffDrawFunc)(struct DemoGeff*, PlayState*);

typedef struct DemoGeff {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ s32 action;
    /* 0x0150 */ s32 drawConfig;
    /* 0x0154 */ s32 objBankIndex;
    /* 0x0158 */ DemoGt* demoGt;
    /* 0x015C */ f32 deltaPosX;
    /* 0x0160 */ f32 deltaPosY;
    /* 0x0164 */ f32 deltaPosZ;
} DemoGeff; // size = 0x0168

#endif
