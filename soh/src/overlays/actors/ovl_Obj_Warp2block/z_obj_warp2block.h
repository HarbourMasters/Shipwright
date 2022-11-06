#ifndef Z_OBJ_WARP2BLOCK_H
#define Z_OBJ_WARP2BLOCK_H

#include "ultra64.h"
#include "global.h"

struct ObjWarp2block;

typedef void (*ObjWarp2blockActionFunc)(struct ObjWarp2block*, PlayState*);
typedef s32 (*ObjWarp2blockFunc168)(struct ObjWarp2block*, PlayState*);

typedef struct ObjWarp2block {
    /* 0x0000 */ DynaPolyActor dyna;
    /* 0x0164 */ ObjWarp2blockActionFunc actionFunc;
    /* 0x0168 */ ObjWarp2blockFunc168 func_168;
    /* 0x016C */ s16 unk_16C;
    /* 0x016E */ s16 unk_16E;
    /* 0x0170 */ s16 unk_170;
    /* 0x0172 */ u16 unk_172;
    /* 0x0174 */ s16 unk_174;
} ObjWarp2block; // size = 0x0178

#endif
