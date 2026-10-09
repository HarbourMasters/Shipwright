#ifndef Z_EN_FIRE_ROCK_H
#define Z_EN_FIRE_ROCK_H

#include "global.h"

struct EnFireRock;

typedef struct EnFireRock {
    /* 0x000 */ Actor actor;
    /* 0x144 */ char unk_144[0x94];
} EnFireRock; // size = 0x1D8

#endif // Z_EN_FIRE_ROCK_H
