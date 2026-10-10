#ifndef Z_EN_ENDING_HERO4_H
#define Z_EN_ENDING_HERO4_H

#include "global.h"
#include "objects/object_sdn/object_sdn.h"

struct EnEndingHero4;

typedef void (*EnEndingHero4ActionFunc)(struct EnEndingHero4*, PlayState*);

typedef struct EnEndingHero4 {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ Vec3s jointTable[SOLDIER_LIMB_MAX];
    /* 0x1EE */ Vec3s morphTable[SOLDIER_LIMB_MAX];
    /* 0x254 */ EnEndingHero4ActionFunc actionFunc;
    /* 0x258 */ u16 isIdle;
} EnEndingHero4; // size = 0x25C

#endif // Z_EN_ENDING_HERO4_H
