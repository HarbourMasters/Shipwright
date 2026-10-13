#ifndef Z_EN_EGBLOCK_H
#define Z_EN_EGBLOCK_H

#include "global.h"

struct EnEgblock;

typedef void (*EnEgblockActionFunc)(struct EnEgblock*, PlayState*);

#define EGBLOCK_GET_PARAM_7F(thisx) ((thisx)->params & 0x7F)
#define EGBLOCK_GET_PARAM_F80(thisx) (((thisx)->params >> 7) & 0x1F)
#define EGBLOCK_GET_TYPE(thisx) (((thisx)->params >> 12) & 0xF)

#define EGBLOCK_PARAMS(type, param1, param2) ((((type) & 0xF) << 12) | (((param1) & 0x1F) << 7) | ((param2) & 0x7F))

#define EGBLOCK_TYPE_ACTIVE 0

#define EGBLOCK_EFFECT_COUNT 50

typedef struct {
    /* 0x00 */ u8 isActive;
    /* 0x04 */ Vec3f pos;
    /* 0x10 */ s16 alpha;
    /* 0x12 */ s16 timer;
    /* 0x14 */ s16 type;
    /* 0x18 */ Vec3f velocity;
    /* 0x24 */ Vec3f accel;
    /* 0x30 */ Vec3s rot;
    /* 0x38 */ f32 scale;
} EnEgblockEffect; // size = 0x3C

typedef struct EnEgblock {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ UNK_TYPE1 unk_15C[0x44]; // SkelAnime?
    /* 0x1A0 */ EnEgblockActionFunc actionFunc;
    /* 0x1A4 */ s16 type;
    /* 0x1A6 */ s16 paramF80;
    /* 0x1A8 */ s16 param7F;
    /* 0x1AA */ s16 timer;
    /* 0x1AC */ s16 inactive;
    /* 0x1AE */ UNK_TYPE1 unk_1AE[2];
    /* 0x1B0 */ UNK_TYPE1 unk_1B0[0x4C]; // ColliderCylinder?
    /* 0x1FC */ EnEgblockEffect effects[EGBLOCK_EFFECT_COUNT];
} EnEgblock; // size = 0xDB4

#endif // Z_EN_EGBLOCK_H
