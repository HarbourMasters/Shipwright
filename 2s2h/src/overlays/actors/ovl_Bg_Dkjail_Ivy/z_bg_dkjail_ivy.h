#ifndef Z_BG_DKJAIL_IVY_H
#define Z_BG_DKJAIL_IVY_H

#include "global.h"

#define BG_DKJAIL_GET_SWITCH_FLAG(thisx) ((thisx)->params & 0x7F)

struct BgDkjailIvy;

typedef void (*BgDkjailIvyActionFunc)(struct BgDkjailIvy*, PlayState*);

typedef struct BgDkjailIvy {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ ColliderCylinder collider;
    /* 0x1A8 */ BgDkjailIvyActionFunc actionFunc;
    /* 0x1AC */ s8 fadeOutTimer;
    /* 0x1AD */ u8 alpha;
} BgDkjailIvy; // size = 0x1B0

#endif // Z_BG_DKJAIL_IVY_H
