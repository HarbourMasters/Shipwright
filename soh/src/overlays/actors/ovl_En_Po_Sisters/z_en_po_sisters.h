#ifndef Z_EN_PO_SISTERS_H
#define Z_EN_PO_SISTERS_H

#include "ultra64.h"
#include "global.h"

struct EnPoSisters;

typedef void (*EnPoSistersActionFunc)(struct EnPoSisters*, PlayState*);

typedef struct EnPoSisters {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;
    /* 0x0190 */ EnPoSistersActionFunc actionFunc;
    /* 0x0194 */ u8 unk_194;
    /* 0x0195 */ u8 unk_195;
    /* 0x0196 */ u8 unk_196;
    /* 0x0197 */ u8 unk_197;
    /* 0x0198 */ u8 unk_198;
    /* 0x0199 */ u8 unk_199;
    /* 0x019A */ s16 unk_19A;
    /* 0x019A */ s16 unk_19C;
    /* 0x019E */ Vec3s jointTable[12];
    /* 0x01E6 */ Vec3s morphTable[12];
    /* 0x022E */ Color_RGBA8 unk_22E;
    /* 0x0234 */ Vec3f unk_234[8];
    /* 0x0294 */ f32 unk_294;
    /* 0x0298 */ LightNode* lightNode;
    /* 0x029C */ LightInfo lightInfo;
    /* 0x02AC */ ColliderCylinder collider;
    /* 0x02F8 */ MtxF unk_2F8;
    u32 epoch;
} EnPoSisters; // size = 0x0338

#endif
