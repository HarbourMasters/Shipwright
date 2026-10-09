#ifndef Z_EN_ENDING_HERO6_H
#define Z_EN_ENDING_HERO6_H

#include "global.h"
#include "objects/object_dt/object_dt.h"
#include "objects/object_daiku/object_daiku.h"
#include "objects/object_bai/object_bai.h"
#include "objects/object_toryo/object_toryo.h"
#include "objects/object_sdn/object_sdn.h"

struct EnEndingHero6;

typedef void (*EnEndingHero6ActionFunc)(struct EnEndingHero6*, PlayState*);

#define ENDING_HERO6_LIMB_MAX \
    MAX(MAX(MAX(MAX((s32)OBJECT_DT_LIMB_MAX, \
                    (s32)OBJECT_BAI_LIMB_MAX), \
                    (s32)OBJECT_TORYO_LIMB_MAX), \
                    (s32)SOLDIER_LIMB_MAX), \
                    (s32)OBJECT_DAIKU_LIMB_MAX)

typedef struct EnEndingHero6 {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ Vec3s jointTable[ENDING_HERO6_LIMB_MAX];
    /* 0x200 */ Vec3s morphTable[ENDING_HERO6_LIMB_MAX];
    /* 0x278 */ EnEndingHero6ActionFunc actionFunc;
    /* 0x27C */ s32 animIndex;
    /* 0x280 */ s32 type;
    /* 0x284 */ s8 objectSlot;
    /* 0x286 */ s16 timer;
    /* 0x288 */ s16 blinkTimer;
    /* 0x28A */ s16 eyeState;
    /* 0x28C */ f32 animEndFrame;
    /* 0x290 */ s16 isIdle;
} EnEndingHero6; // size = 0x294

#endif // Z_EN_ENDING_HERO6_H
