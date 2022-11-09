#ifndef Z_EN_BILI_H
#define Z_EN_BILI_H

#include "ultra64.h"
#include "global.h"

struct EnBili;

typedef void (*EnBiliActionFunc)(struct EnBili*, PlayState*);

typedef enum {
    /* 0 */ EN_BILI_LIMB_NONE,
    /* 1 */ EN_BILI_LIMB_ROOT,
    /* 2 */ EN_BILI_LIMB_INNER_HOOD,
    /* 3 */ EN_BILI_LIMB_OUTER_HOOD,
    /* 4 */ EN_BILI_LIMB_TENTACLES,
    /* 5 */ EN_BILI_LIMB_MAX
} EnBiliLimb;

typedef struct EnBili {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;
    /* 0x0190 */ EnBiliActionFunc actionFunc;
    /* 0x0194 */ u8 tentaclesTexIndex;
    /* 0x0195 */ u8 playFlySound;
    /* 0x0196 */ s16 timer;
    /* 0x0198 */ Vec3s jointTable[EN_BILI_LIMB_MAX];
    /* 0x01B6 */ Vec3s morphTable[EN_BILI_LIMB_MAX];
    /* 0x01D4 */ ColliderCylinder collider;
} EnBili; // size = 0x0220

typedef enum {
    /* -1 */ EN_BILI_TYPE_NORMAL = -1,
    /*  0 */ EN_BILI_TYPE_VALI_SPAWNED,
    /*  1 */ EN_BILI_TYPE_DYING
} EnBiliType;

#endif
