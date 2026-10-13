#ifndef Z_EN_ENDING_HERO_H
#define Z_EN_ENDING_HERO_H

#include "global.h"
#include "objects/object_dt/object_dt.h"

struct EnEndingHero;

typedef void (*EnEndingHeroActionFunc)(struct EnEndingHero*, PlayState*);

typedef struct EnEndingHero {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ Vec3s jointTable[OBJECT_DT_LIMB_MAX];
    /* 0x1E2 */ Vec3s morphTable[OBJECT_DT_LIMB_MAX];
    /* 0x23C */ EnEndingHeroActionFunc actionFunc;
    /* 0x240 */ s16 unk240;
    /* 0x242 */ s16 unk242;
    /* 0x244 */ u16 isIdle;
} EnEndingHero; // size = 0x248

#endif // Z_EN_ENDING_HERO_H
