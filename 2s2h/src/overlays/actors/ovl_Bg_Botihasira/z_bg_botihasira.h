#ifndef Z_BG_BOTIHASIRA_H
#define Z_BG_BOTIHASIRA_H

#include "global.h"

struct BgBotihasira;

typedef void (*BgBotihasiraActionFunc)(struct BgBotihasira*, PlayState*);

typedef struct BgBotihasira {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ BgBotihasiraActionFunc actionFunc;
    /* 0x160 */ ColliderCylinder collider;
} BgBotihasira; // size = 0x1AC

#endif // Z_BG_BOTIHASIRA_H
