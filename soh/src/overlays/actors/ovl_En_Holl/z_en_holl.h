#ifndef Z_EN_HOLL_H
#define Z_EN_HOLL_H

#include "ultra64.h"
#include "global.h"

struct EnHoll;

typedef void (*EnHollActionFunc)(struct EnHoll*, PlayState*);

typedef struct EnHoll {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ s16 planeAlpha;
    /* 0x014E */ u8 side;
    /* 0x014F */ u8 unk_14F;
    /* 0x0150 */ EnHollActionFunc actionFunc;
} EnHoll; // size = 0x0154

#endif
