#ifndef Z_EN_DODOJR_H
#define Z_EN_DODOJR_H

#include "ultra64.h"
#include "global.h"

struct EnDodojr;

typedef void (*EnDodojrActionFunc)(struct EnDodojr*, PlayState*);

typedef struct EnDodojr {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;
    /* 0x0190 */ EnDodojrActionFunc actionFunc;
    /* 0x0194 */ ColliderCylinder collider;
    /* 0x01E0 */ Actor* bomb;
    /* 0x01E4 */ Vec3f headPos;
    /* 0x01F0 */ Vec3f dustPos;
    /* 0x01FC */ s16 unk_1FC;
    /* 0x01FE */ s16 timer1;
    /* 0x0200 */ s16 timer2;
    /* 0x0202 */ s16 timer3;
    /* 0x0204 */ s16 timer4;
    /* 0x0208 */ f32 rootScale; // scale used with the root limb
    /* 0x020C */ Vec3s jointTable[15];
    /* 0x0266 */ Vec3s morphTable[15];
} EnDodojr; // size = 0x02C0

#endif
