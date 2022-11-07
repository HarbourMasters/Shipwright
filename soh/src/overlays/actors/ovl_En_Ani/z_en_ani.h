#ifndef Z_EN_ANI_H
#define Z_EN_ANI_H

#include "ultra64.h"
#include "global.h"

struct EnAni;

typedef void (*EnAniActionFunc)(struct EnAni*, PlayState*);

typedef struct EnAni {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ ColliderCylinder collider;
    /* 0x0198 */ SkelAnime skelAnime;
    /* 0x01DC */ Vec3s jointTable[16];
    /* 0x023C */ Vec3s morphTable[16];
    /* 0x029C */ Vec3s unk_29C;
    /* 0x02A2 */ Vec3s unk_2A2;
    /* 0x02A8 */ u16 unk_2A8;
    /* 0x02AA */ u16 unk_2AA;
    /* 0x02AC */ s16 eyeIndex;
    /* 0x02AE */ s16 blinkTimer;
    /* 0x02B0 */ EnAniActionFunc actionFunc;
} EnAni; // size = 0x02B4

#endif
