#ifndef Z_EN_EIYER_H
#define Z_EN_EIYER_H

#include "ultra64.h"
#include "global.h"

struct EnEiyer;

typedef void (*EnEiyerActionFunc)(struct EnEiyer*, PlayState*);

typedef struct EnEiyer {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelanime;
    /* 0x0190 */ EnEiyerActionFunc actionFunc;
    /* 0x0194 */ s16 timer;
    /* 0x0196 */ s16 targetYaw;
    /* 0x0198 */ Vec3s jointTable[19];
    /* 0x020A */ Vec3s morphTable[19];
    /* 0x027C */ Vec3f basePos;
    /* 0x0288 */ ColliderCylinder collider;
} EnEiyer; // size = 0x02D4

#endif
