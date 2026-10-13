#ifndef Z_DM_CHAR06_H
#define Z_DM_CHAR06_H

#include "global.h"
#include "assets/objects/object_yukiyama/object_yukiyama.h"

struct DmChar06;

typedef void (*DmChar06ActionFunc)(struct DmChar06*, PlayState*);

typedef struct DmChar06 {
    /* 0x000 */ Actor actor;
    /* 0x144 */ UNK_TYPE1 pad144[0x14C];
    /* 0x290 */ DmChar06ActionFunc actionFunc;
    /* 0x294 */ UNK_TYPE1 pad294[0xC];
    /* 0x2A0 */ u8 alpha;
} DmChar06; // size = 0x2A4

#endif // Z_DM_CHAR06_H
