#ifndef Z_DEMO_GO_H
#define Z_DEMO_GO_H

#include "ultra64.h"
#include "global.h"

struct DemoGo;

typedef void (*DemoGoActionFunc)(struct DemoGo*, PlayState*);
typedef void (*DemoGoDrawFunc)(struct DemoGo*, PlayState*);

typedef struct DemoGo {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;
    /* 0x0190 */ s16 unk_190;
    /* 0x0192 */ s16 unk_192;
    /* 0x0194 */ s32 action;
    /* 0x0198 */ s32 drawConfig;
    /* 0x019C */ f32 unk_19C;
} DemoGo; // size = 0x01A0

#endif
