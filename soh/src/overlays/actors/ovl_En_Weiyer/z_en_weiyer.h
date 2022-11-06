#ifndef Z_EN_WEIYER_H
#define Z_EN_WEIYER_H

#include "ultra64.h"
#include "global.h"

struct EnWeiyer;

typedef void (*EnWeiyerActionFunc)(struct EnWeiyer*, PlayState*);

typedef struct EnWeiyer {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;
    /* 0x0190 */ EnWeiyerActionFunc actionFunc;
    /* 0x0194 */ s16 unk_194;
    /* 0x0196 */ s16 unk_196;
    /* 0x0198 */ Vec3s jointTable[19];
    /* 0x020A */ Vec3s morphTable[19];
    /* 0x027C */ f32 unk_27C;
    /* 0x0280 */ f32 unk_280;
    /* 0x0284 */ ColliderCylinder collider;
} EnWeiyer; // size = 0x02D0

#endif
