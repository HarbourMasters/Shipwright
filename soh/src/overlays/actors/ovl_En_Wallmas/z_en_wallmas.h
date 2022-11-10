#ifndef Z_EN_WALLMAS_H
#define Z_EN_WALLMAS_H

#include "ultra64.h"
#include "global.h"

typedef enum {
    /* 0x00 */ WMT_TIMER,
    /* 0x01 */ WMT_PROXIMITY,
    /* 0x02 */ WMT_FLAG
} WallmasType;

struct EnWallmas;

typedef void (*EnWallmasActionFunc)(struct EnWallmas*, PlayState*);

typedef struct EnWallmas {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;
    /* 0x0190 */ EnWallmasActionFunc actionFunc;
    /* 0x0194 */ s16 timer;
    /* 0x0196 */ s16 switchFlag;
    /* 0x0198 */ Vec3s jointTable[25];
    /* 0x022E */ Vec3s morphTable[25];
    /* 0x02C4 */ f32 yTarget;
    /* 0x02C8 */ ColliderCylinder collider;
} EnWallmas; // size = 0x0314

#endif
