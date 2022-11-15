#ifndef Z_EN_BIGOKUTA_H
#define Z_EN_BIGOKUTA_H

#include "ultra64.h"
#include "global.h"

struct EnBigokuta;

typedef void (*EnBigokutaActionFunc)(struct EnBigokuta*, PlayState*);

typedef struct EnBigokuta {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;
    /* 0x0190 */ EnBigokutaActionFunc actionFunc;
    /* 0x0194 */ s8 unk_194;
    /* 0x0195 */ u8 unk_195;
    /* 0x0196 */ s16 unk_196;
    /* 0x0198 */ s16 unk_198;
    /* 0x019A */ s16 unk_19A;
    /* 0x019C */ Vec3s jointTable[20];
    /* 0x0214 */ Vec3s morphTable[20];
    /* 0x028C */ ColliderJntSph collider;
    /* 0x02AC */ ColliderJntSphElement element;
    /* 0x02EC */ ColliderCylinder cylinder[2];
} EnBigokuta; // size = 0x0384

#endif
