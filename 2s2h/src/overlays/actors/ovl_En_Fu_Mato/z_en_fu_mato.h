#ifndef Z_EN_FU_MATO_H
#define Z_EN_FU_MATO_H

#include "global.h"

struct EnFuMato;

typedef void (*EnFuMatoActionFunc)(struct EnFuMato*, PlayState*);

#define ENFUMATO_GET(thisx) ((thisx)->params)

typedef struct {
    /* 0x00 */ Vec3f unk_00;
    /* 0x0C */ Vec3f unk_0C;
    /* 0x18 */ Vec3f unk_18;
    /* 0x24 */ Vec3s unk_24;
    /* 0x2A */ Vec3s unk_2A;
    /* 0x30 */ s16 unk_30;
} EnFuMatoStruct; // size = 0x34

typedef struct EnFuMato {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ EnFuMatoActionFunc actionFunc;
    /* 0x160 */ ColliderSphere collider;
    /* 0x1B8 */ EnFuMatoStruct unk_1B8[6];
    /* 0x2F0 */ s32 unk_2F0;
    /* 0x2F4 */ s32 unk_2F4;
    /* 0x2F8 */ Vec3s* unk_2F8;
    /* 0x2FC */ Vec3s unk_2FC;
    /* 0x302 */ s16 unk_302;
    /* 0x304 */ s16 unk_304;
    /* 0x306 */ UNK_TYPE1 unk306[2];
    /* 0x308 */ s16 unk_308;
    /* 0x30A */ s16 unk_30A;
} EnFuMato; // size = 0x30C

#endif // Z_EN_FU_MATO_H
