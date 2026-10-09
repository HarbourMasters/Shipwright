#ifndef Z_EN_NIMOTSU_H
#define Z_EN_NIMOTSU_H

#include "global.h"

struct EnNimotsu;

typedef struct EnNimotsu {
    /* 0x000 */ Actor actor;
    /* 0x144 */ UNK_TYPE1 unk_144[0x48];
    /* 0x18C */ ColliderCylinder collider;
    /* 0x1D8 */ u16 dustDone;
    /* 0x1DA */ s16 timer;
} EnNimotsu; // size = 0x1DC

#endif // Z_EN_NIMOTSU_H
