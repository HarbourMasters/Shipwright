#ifndef Z_EN_SC_RUPPE_H
#define Z_EN_SC_RUPPE_H

#include "global.h"

#define SCRUPPE_GET_TYPE(thisx) ((thisx)->params & 0x1F)

struct EnScRuppe;

typedef void (*EnScRuppeActionFunc)(struct EnScRuppe*, PlayState*);

typedef struct EnScRuppe {
    /* 0x000 */ Actor actor;
    /* 0x144 */ ColliderCylinder collider;
    /* 0x190 */ EnScRuppeActionFunc actionFunc;
    /* 0x194 */ s16 ruppeDisplayTime;
    /* 0x196 */ s16 ruppeIndex;
} EnScRuppe; // size = 0x198

typedef enum {
    /* 0 */ RUPEE_GREEN,
    /* 1 */ RUPEE_BLUE,
    /* 2 */ RUPEE_RED,
    /* 3 */ RUPEE_ORANGE,
    /* 4 */ RUPEE_PURPLE,
    /* 5 */ RUPEE_UNUSED
} RupeeType;

#endif // Z_EN_SC_RUPPE_H
