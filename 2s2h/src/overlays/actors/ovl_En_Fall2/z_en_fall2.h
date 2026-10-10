#ifndef Z_EN_FALL2_H
#define Z_EN_FALL2_H

#include "global.h"
#include "assets/objects/object_fall2/object_fall2.h"

struct EnFall2;

typedef void (*EnFall2ActionFunc)(struct EnFall2*, PlayState*);

typedef struct EnFall2 {
    /* 0x000 */ Actor actor;
    /* 0x144 */ KFSkelAnimeFlex kfSkelAnime;
    /* 0x174 */ Vec3s jointTable[30];
    /* 0x228 */ Vec3s morphTable[30];
    /* 0x2DC */ AnimatedMaterial* unk2DC;
    /* 0x2E0 */ f32 alphaLevel;
    /* 0x2E4 */ s16 cueId;
    /* 0x2E6 */ u16 cueType;
    /* 0x2E8 */ EnFall2ActionFunc actionFunc;
} EnFall2; // size = 0x2EC

#endif // Z_EN_FALL2_H
