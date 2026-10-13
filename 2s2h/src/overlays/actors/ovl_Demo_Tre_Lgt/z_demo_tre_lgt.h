#ifndef Z_DEMO_TRE_LGT_H
#define Z_DEMO_TRE_LGT_H

#include "global.h"
#include "z64curve.h"

struct DemoTreLgt;

typedef void (*DemoTreLgtActionFunc)(struct DemoTreLgt* this, PlayState* play);

typedef struct DemoTreLgt {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelCurve skelCurve;
    /* 0x164 */ s32 action;
    /* 0x168 */ u32 colorAlpha1;
    /* 0x16C */ u32 colorAlpha2;
    /* 0x170 */ u8 status;
    /* 0x174 */ s32 animationType;
} DemoTreLgt; // size = 0x178

#endif // Z_DEMO_TRE_LGT_H
