#ifndef Z_EN_STH2_H
#define Z_EN_STH2_H

#include "global.h"
#include "objects/object_sth/object_sth.h"

struct EnSth2;

typedef void (*EnSth2ActionFunc)(struct EnSth2*, PlayState*);

typedef struct EnSth2 {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ Vec3s jointTable[STH_LIMB_MAX];
    /* 0x1E8 */ Vec3s morphTable[STH_LIMB_MAX];
    /* 0x248 */ s16 unused;
    /* 0x24A */ u8 objectSlot;
    /* 0x24C */ EnSth2ActionFunc actionFunc;
} EnSth2; // size = 0x250

#endif // Z_EN_STH2_H
