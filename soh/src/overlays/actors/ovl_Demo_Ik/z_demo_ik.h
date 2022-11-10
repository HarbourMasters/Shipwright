#ifndef Z_DEMO_IK_H
#define Z_DEMO_IK_H

#include "ultra64.h"
#include "global.h"

struct DemoIk;

typedef void (*DemoIkActionFunc)(struct DemoIk* this, PlayState* play);
typedef void (*DemoIkDrawFunc)(struct DemoIk* this, PlayState* play);

typedef struct DemoIk {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;
    /* 0x0190 */ Vec3s jointTable[2];
    /* 0x019C */ Vec3s morphTable[2];
    /* 0x01A8 */ s32 actionMode;
    /* 0x01AC */ s32 drawMode;
    /* 0x01B0 */ s32 csAction;
} DemoIk; // size = 0x01B4

#endif
