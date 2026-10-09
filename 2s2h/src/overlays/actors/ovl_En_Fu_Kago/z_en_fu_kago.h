#ifndef Z_EN_FU_KAGO_H
#define Z_EN_FU_KAGO_H

#include "global.h"

struct EnFuKago;

typedef void (*EnFuKagoActionFunc)(struct EnFuKago*, PlayState*);

typedef struct {
    /* 0x00 */ Vec3f unk_00;
    /* 0x0C */ Vec3f unk_0C;
    /* 0x18 */ Vec3f unk_18;
    /* 0x24 */ Vec3f unk_24;
    /* 0x30 */ Vec3s unk_30;
    /* 0x36 */ Vec3s unk_36;
    /* 0x3C */ s16 unk_3C;
} EnFuKagoStruct; // size = 0x40

typedef struct EnFuKago {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ EnFuKagoActionFunc actionFunc;
    /* 0x160 */ ColliderSphere collider;
    /* 0x1B8 */ EnFuKagoStruct unk_1B8[6];
    /* 0x338 */ s16 unk_338;
    /* 0x33A */ s16 unk_33A;
    /* 0x33C */ s16 unk_33C;
} EnFuKago; // size = 0x340

#endif // Z_EN_FU_KAGO_H
