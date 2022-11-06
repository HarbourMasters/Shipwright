#ifndef Z_DEMO_EXT_H
#define Z_DEMO_EXT_H

#include "ultra64.h"
#include "global.h"

struct DemoExt;

typedef void (*DemoExtActionFunc)(struct DemoExt*, PlayState*);
typedef void (*DemoExtDrawFunc)(Actor*, PlayState*);

typedef struct DemoExt {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ s32 action;
    /* 0x014C */ s32 drawMode;
    /* 0x0154 */ s32 previousCsAction;
    /* 0x015E */ s16 scrollIncr[4];
    /* 0x0160 */ s16 curScroll[4];
    /* 0x0168 */ s16 rotationPitch;
    /* 0x016C */ f32 alphaTimer;
    /* 0x0170 */ s32 primAlpha;
    /* 0x0174 */ s32 envAlpha;
    /* 0x0178 */ Vec3f scale;
} DemoExt; // size = 0x0184

#endif
