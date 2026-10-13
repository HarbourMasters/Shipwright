#ifndef Z_EN_INVISIBLE_RUPPE_H
#define Z_EN_INVISIBLE_RUPPE_H

#include "global.h"

struct EnInvisibleRuppe;

typedef void (*EnInvisibleRuppeActionFunc)(struct EnInvisibleRuppe*, PlayState*);

#define INVISIBLERUPPE_GET_3(thisx) ((thisx)->params & 3)
#define INVISIBLERUPPE_GET_SWITCH_FLAG(thisx) (((thisx)->params & 0x1FC) >> 2)

#define INVISIBLERUPPE_SWITCH_FLAG_NONE 0x7F

typedef struct EnInvisibleRuppe {
    /* 0x000 */ Actor actor;
    /* 0x144 */ ColliderCylinder collider;
    /* 0x190 */ s16 switchFlag;
    /* 0x192 */ s16 unk_192;
    /* 0x194 */ EnInvisibleRuppeActionFunc actionFunc;
} EnInvisibleRuppe; // size = 0x198

#endif // Z_EN_INVISIBLE_RUPPE_H
