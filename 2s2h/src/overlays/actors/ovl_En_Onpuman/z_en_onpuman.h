#ifndef Z_EN_ONPUMAN_H
#define Z_EN_ONPUMAN_H

#include "global.h"

struct EnOnpuman;

typedef void (*EnOnpumanActionFunc)(struct EnOnpuman*, PlayState*);

typedef struct EnOnpuman {
    /* 0x000 */ Actor actor;
    /* 0x144 */ UNK_TYPE1 pad_144[0x110];
    /* 0x254 */ ColliderCylinder collider;
    /* 0x2A0 */ Actor* unk_2A0;
    /* 0x2A4 */ u16 unk_2A4; // flags
    /* 0x2A8 */ EnOnpumanActionFunc actionFunc;
} EnOnpuman; // size = 0x2AC

#endif // Z_EN_ONPUMAN_H
