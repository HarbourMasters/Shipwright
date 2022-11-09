#ifndef Z_EN_REEBA_H
#define Z_EN_REEBA_H

#include "ultra64.h"
#include "global.h"

struct EnReeba;

typedef void (*EnReebaActionFunc)(struct EnReeba*, PlayState*);

typedef struct EnReeba {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelanime;
    /* 0x0190 */ Vec3s jointTable[18];
    /* 0x01FC */ Vec3s morphTable[18];
    /* 0x0268 */ char unk_268[0x4];
    /* 0x026C */ EnReebaActionFunc actionfunc;
    /* 0x0270 */ s16 unk_270;
    /* 0x0272 */ s16 unk_272;
    /* 0x0274 */ s16 unk_274;
    /* 0x0276 */ s16 unk_276;
    /* 0x0278 */ s16 unk_278;
    /* 0x027A */ s16 isBig;
    /* 0x027C */ s16 unk_27C;
    /* 0x027E */ s16 unk_27E;
    /* 0x0280 */ s16 unk_280;
    /* 0x0284 */ f32 unk_284;
    /* 0x0288 */ f32 unk_288;
    /* 0x028C */ f32 scale;
    /* 0x0290 */ ColliderCylinder collider;
} EnReeba; // size = 0x02DC

typedef enum {
    /* 0 */ LEEVER_SMALL,
    /* 1 */ LEEVER_BIG
} LeeverParam;

#endif
