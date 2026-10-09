#ifndef Z_EN_OKARINA_TAG_H
#define Z_EN_OKARINA_TAG_H

#include "global.h"

struct EnOkarinaTag;

typedef void (*EnOkarinaTagActionFunc)(struct EnOkarinaTag*, PlayState*);

#define ENOKARINATAG_GET_SWITCH_FLAG(thisx) (((thisx)->params & 0x7F))
#define ENOKARINATAG_GET_780(thisx) (((thisx)->params >> 0x7) & 0xF)
#define ENOKARINATAG_GET_F800(thisx) (((thisx)->params >> 0xB) & 0x1F)

#define ENOKARINATAG_SWITCH_FLAG_NONE 0x7F

typedef struct EnOkarinaTag {
    /* 0x000 */ Actor actor;
    /* 0x144 */ EnOkarinaTagActionFunc actionFunc;
    /* 0x148 */ s16 unk148;
    /* 0x14A */ s16 unk14A;
    /* 0x14C */ s32 switchFlag;
    /* 0x150 */ s8 unk150;
    /* 0x154 */ f32 unk154;
    /* 0x158 */ f32 unk158;
} EnOkarinaTag; // size = 0x15C

#endif // Z_EN_OKARINA_TAG_H
