#ifndef Z_DM_STATUE_H
#define Z_DM_STATUE_H

#include "global.h"

struct DmStatue;

typedef struct DmStatue {
    /* 0x000 */ Actor actor;
    /* 0x144 */ char unk_144[0x50];
} DmStatue; // size = 0x194

#endif // Z_DM_STATUE_H
