#ifndef Z_EN_DS_H
#define Z_EN_DS_H

#include "ultra64.h"
#include "global.h"

struct EnDs;

typedef void (*EnDsActionFunc)(struct EnDs*, PlayState*);

typedef struct EnDs {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;
    /* 0x0190 */ Vec3s jointTable[6];
    /* 0x01B4 */ Vec3s morphTable[6];
    /* 0x01D8 */ Vec3s unk_1D8;
    /* 0x01DE */ Vec3s unk_1DE;
    /* 0x01E4 */ f32 unk_1E4;
    /* 0x01E8 */ u16 unk_1E8;
    /* 0x01EA */ u16 brewTimer;
    /* 0x01EC */ EnDsActionFunc actionFunc;
} EnDs; // size = 0x01F0

#endif
