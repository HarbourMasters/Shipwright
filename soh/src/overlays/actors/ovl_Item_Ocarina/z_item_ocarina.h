#ifndef Z_ITEM_OCARINA_H
#define Z_ITEM_OCARINA_H

#include "ultra64.h"
#include "global.h"

struct ItemOcarina;

typedef void (*ItemOcarinaActionFunc)(struct ItemOcarina*, PlayState*);

typedef struct ItemOcarina {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ ItemOcarinaActionFunc actionFunc;
    /* 0x0150 */ s16 spinRotOffset;
} ItemOcarina; // size = 0x0154

#endif
