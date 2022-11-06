#ifndef Z_EN_SKB_H
#define Z_EN_SKB_H

#include "ultra64.h"
#include "global.h"

struct EnSkb;

typedef void (*EnSkbActionFunc)(struct EnSkb*, PlayState*);

typedef struct EnSkb {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;
    /* 0x0190 */ Vec3s jointTable[20];
    /* 0x0208 */ Vec3s morphTable[20];
    /* 0x0280 */ u8 unk_280;
    /* 0x0281 */ u8 unk_281;
    /* 0x0282 */ u8 unk_282;
    /* 0x0283 */ u8 unk_283;
    /* 0x0284 */ EnSkbActionFunc actionFunc; 
    /* 0x0288 */ s16 unk_288;
    /* 0x028C */ BodyBreak bodyBreak; 
    /* 0x02A4 */ ColliderJntSph collider;
    /* 0x02C4 */ ColliderJntSphElement colliderItem[2];
} EnSkb; // size = 0x0344

#endif
