#ifndef Z_EN_MD_H
#define Z_EN_MD_H

#include "ultra64.h"
#include "global.h"

struct EnMd;

typedef void (*EnMdActionFunc)(struct EnMd*, PlayState*);

typedef struct EnMd {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;
    /* 0x0190 */ EnMdActionFunc actionFunc;
    /* 0x0194 */ ColliderCylinder collider;
    /* 0x01E0 */ struct_80034A14_arg1 unk_1E0;
    /* 0x0208 */ u8 unk_208;
    /* 0x0209 */ u8 unk_209;
    /* 0x020A */ u8 unk_20A;
    /* 0x020B */ u8 unk_20B;
    /* 0x020C */ s16 blinkTimer;
    /* 0x020E */ s16 eyeIdx;
    /* 0x0210 */ s16 alpha;
    /* 0x0212 */ s16 waypoint;
    /* 0x0214 */ s16 unk_214[17];
    /* 0x0236 */ s16 unk_236[17];
    /* 0x0258 */ Vec3s jointTable[17];
    /* 0x02BE */ Vec3s morphTable[17];
} EnMd; // size = 0x0324

#endif
