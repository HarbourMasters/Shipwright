#ifndef Z_EN_RD_H
#define Z_EN_RD_H

#include "ultra64.h"
#include "global.h"

struct EnRd;

typedef void (*EnRdActionFunc)(struct EnRd*, PlayState*);

typedef struct EnRd {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ Vec3s firePos[10];
    /* 0x0188 */ SkelAnime skelAnime;
    /* 0x01CC */ Vec3s jointTable[26];
    /* 0x0268 */ Vec3s morphTable[26];
    /* 0x0304 */ u8 unk_304;
    /* 0x0305 */ u8 unk_305;
    /* 0x0306 */ u8 unk_306;
    /* 0x0307 */ u8 unk_307;
    /* 0x0308 */ EnRdActionFunc actionFunc;
    /* 0x030C */ s16 unk_30C;
    /* 0x030E */ s16 unk_30E;
    /* 0x0310 */ s16 unk_310;
    /* 0x0312 */ s16 unk_312;
    /* 0x0314 */ s16 unk_314;
    /* 0x0316 */ s16 unk_316;
    /* 0x0318 */ u8 unk_318;
    /* 0x0319 */ u8 unk_319;
    /* 0x031A */ u8 unk_31A;
    /* 0x031B */ u8 unk_31B;
    /* 0x031C */ u8 unk_31C;
    /* 0x031D */ u8 unk_31D;
    /* 0x0320 */ ColliderCylinder collider;
} EnRd; // size = 0x036C

#endif
