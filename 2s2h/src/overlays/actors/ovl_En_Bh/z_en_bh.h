#ifndef Z_EN_BH_H
#define Z_EN_BH_H

#include "global.h"
#include "objects/object_bh/object_bh.h"

struct EnBh;

typedef void (*EnBhActionFunc)(struct EnBh*, PlayState*);

typedef struct EnBh {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ Vec3s jointTable[OBJECT_BH_LIMB_MAX];
    /* 0x1AC */ Vec3s morphTable[OBJECT_BH_LIMB_MAX];
    /* 0x1D0 */ Vec3f pos;
    /* 0x1DC */ s16 timer;
    /* 0x1DE */ s16 timer2;
    /* 0x1E0 */ s16 step;
    /* 0x1E2 */ s16 unk1E2;
    /* 0x1E4 */ s16 unk1E4;
    /* 0x1E8 */ EnBhActionFunc actionFunc;
} EnBh; // size = 0x1EC

#endif // Z_EN_BH_H
