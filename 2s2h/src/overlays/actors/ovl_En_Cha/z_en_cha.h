#ifndef Z_EN_CHA_H
#define Z_EN_CHA_H

#include "global.h"

struct EnCha;

typedef void (*EnChaActionFunc)(struct EnCha*, struct PlayState*);

typedef struct EnCha {
    /* 0x000 */ Actor actor;
    /* 0x144 */ ColliderCylinder collider;
    /* 0x190 */ EnChaActionFunc actionFunc;
} EnCha; // size = 0x194

#endif // Z_EN_CHA_H
