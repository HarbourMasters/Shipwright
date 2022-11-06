#ifndef Z_DEMO_GT_H
#define Z_DEMO_GT_H

#include "ultra64.h"
#include "global.h"

struct DemoGt;

typedef void (*DemoGtUpdateFunc)(struct DemoGt*, PlayState*);
typedef void (*DemoGtDrawFunc)(struct DemoGt*, PlayState*);

typedef struct DemoGt {
    /* 0x0000 */ DynaPolyActor dyna;
    /* 0x0164 */ s32 updateMode;
    /* 0x0168 */ s32 drawConfig;
    /* 0x016C */ Vec3s unk_16C;
    /* 0x0172 */ s16 unk_172;
    /* 0x0174 */ s16 unk_174;
    /* 0x0178 */ s32 unk_178[4];
    /* 0x0188 */ s32 unk_188[4];
    /* 0x0198 */ s32 unk_198[4];
} DemoGt; // size = 0x01A8

#endif
