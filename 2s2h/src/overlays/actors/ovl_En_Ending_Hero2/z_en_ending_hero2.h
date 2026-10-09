#ifndef Z_EN_ENDING_HERO2_H
#define Z_EN_ENDING_HERO2_H

#include "global.h"
#include "objects/object_bai/object_bai.h"

struct EnEndingHero2;

typedef void (*EnEndingHero2ActionFunc)(struct EnEndingHero2*, PlayState*);

typedef struct EnEndingHero2 {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ Vec3s jointTable[OBJECT_BAI_LIMB_MAX];
    /* 0x200 */ Vec3s morphTable[OBJECT_BAI_LIMB_MAX];
    /* 0x278 */ EnEndingHero2ActionFunc actionFunc;
    /* 0x27C */ u16 isIdle;
} EnEndingHero2; // size = 0x280

#endif // Z_EN_ENDING_HERO2_H
