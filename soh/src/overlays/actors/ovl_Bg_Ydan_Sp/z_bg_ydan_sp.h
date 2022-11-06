#ifndef Z_BG_YDAN_SP_H
#define Z_BG_YDAN_SP_H

#include "ultra64.h"
#include "global.h"

struct BgYdanSp;

typedef void (*BgYdanSpActionFunc)(struct BgYdanSp*, PlayState*);

typedef struct BgYdanSp {
    /* 0x0000 */ DynaPolyActor dyna;
    /* 0x0164 */ BgYdanSpActionFunc actionFunc;
    /* 0x0168 */ u8 isDestroyedSwitchFlag;
    /* 0x0169 */ u8 burnSwitchFlag;
    /* 0x016A */ s16 timer;
    /* 0x016C */ f32 unk_16C;
    /* 0x0170 */ ColliderTris trisCollider;
    /* 0x0190 */ ColliderTrisElement trisColliderItems[2];
} BgYdanSp; // size = 0x0248

#endif
