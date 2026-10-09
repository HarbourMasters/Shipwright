#ifndef Z_EN_MARUTA_H
#define Z_EN_MARUTA_H

#include "global.h"

struct EnMaruta;

typedef void (*EnMarutaActionFunc)(struct EnMaruta*, PlayState*);

#define ENMARUTA_GET_FF(thisx) ((thisx)->params & 0xFF)
#define ENMARUTA_GET_FF00(thisx) (((thisx)->params & 0xFF00) >> 8)

typedef struct EnMaruta {
    /* 0x000 */ Actor actor;
    /* 0x144 */ EnMarutaActionFunc actionFunc;
    /* 0x148 */ ColliderCylinder collider;
    /* 0x194 */ Vec3f unk_194;
    /* 0x1A0 */ Vec3f* unk_1A0;
    /* 0x1A4 */ Vec3f unk_1A4[8];
    /* 0x204 */ Vec3f unk_204;
    /* 0x210 */ s32 unk_210;
    /* 0x214 */ s32 unk_214;
    /* 0x218 */ s16 unk_218;
    /* 0x21A */ s16 unk_21A;
    /* 0x21C */ s16 unk_21C;
    /* 0x21E */ s16 unk_21E;
    /* 0x220 */ s16 unk_220;
} EnMaruta; // size = 0x224

#endif // Z_EN_MARUTA_H
