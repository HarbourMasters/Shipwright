#ifndef Z_DEMO_EC_H
#define Z_DEMO_EC_H

#include "ultra64.h"
#include "global.h"

struct DemoEc;

typedef void (*DemoEcInitFunc)(struct DemoEc*, PlayState*);
typedef void (*DemoEcUpdateFunc)(struct DemoEc*, PlayState*);
typedef void (*DemoEcDrawFunc)(struct DemoEc*, PlayState*);

typedef struct DemoEc {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;
    /* 0x0190 */ s16 eyeTexIndex;
    /* 0x0192 */ s16 blinkTimer;
    /* 0x0194 */ s32 updateMode;
    /* 0x0198 */ s32 drawConfig;
    /* 0x019C */ s32 npcAction;
    /* 0x01A0 */ s32 drawObjBankIndex;
    /* 0x01A4 */ s32 animObjBankIndex;
} DemoEc; // size = 0x01A8

#endif
