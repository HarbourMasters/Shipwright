#ifndef Z_EN_JC_MATO_H
#define Z_EN_JC_MATO_H

#include "global.h"

struct EnJcMato;

typedef void (*EnJcMatoActionFunc)(struct EnJcMato*, PlayState*);

typedef struct EnJcMato {
    /* 0x000 */ Actor actor;
    /* 0x144 */ EnJcMatoActionFunc actionFunc;
    /* 0x148 */ ColliderSphere collider;
    /* 0x1A0 */ s16 despawnTimer;
    /* 0x1A2 */ s16 hitFlag;
    /* 0x1A4 */ Vec3f pos;
} EnJcMato; // size = 0x1B0

#endif // Z_EN_JC_MATO_H
