#ifndef Z_EN_TAG_OBJ_H
#define Z_EN_TAG_OBJ_H

#include "global.h"

struct EnTagObj;

typedef struct EnTagObj {
    /* 0x000 */ Actor actor;
    /* 0x144 */ UNK_TYPE1 unk_144[0x50];
    /* 0x194 */ s32 hasSpawnedSeahorse;
} EnTagObj; // size = 0x198

#endif // Z_EN_TAG_OBJ_H
