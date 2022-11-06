#ifndef Z_EN_BUTTE_H
#define Z_EN_BUTTE_H

#include "ultra64.h"
#include "global.h"

struct EnButte;

typedef void (*EnButteActionFunc)(struct EnButte*, PlayState*);

typedef struct EnButte {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ ColliderJntSph collider;
    /* 0x016C */ ColliderJntSphElement colliderItems[1];
    /* 0x01AC */ SkelAnime skelAnime;
    /* 0x01F0 */ Vec3s jointTable[8];
    /* 0x0220 */ Vec3s morphTable[8];
    /* 0x0250 */ EnButteActionFunc actionFunc;
    /* 0x0254 */ s16 timer;
    /* 0x0256 */ u8 flightParamsIdx;
    /* 0x0257 */ u8 unk_257;
    /* 0x0258 */ u8 drawSkelAnime;
    /* 0x025A */ s16 swordDownTimer;
    /* 0x025C */ s16 unk_25C;
    /* 0x025E */ s16 unk_25E;
    /* 0x0260 */ s16 unk_260;
    /* 0x0264 */ f32 posYTarget;
} EnButte; // size = 0x0268

#endif
