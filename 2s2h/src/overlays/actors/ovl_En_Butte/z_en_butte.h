#ifndef Z_EN_BUTTE_H
#define Z_EN_BUTTE_H

#include "global.h"
#include "objects/gameplay_field_keep/gameplay_field_keep.h"

struct EnButte;

typedef void (*EnButteActionFunc)(struct EnButte*, PlayState*);

#define BUTTERFLY_GET(thisx) ((thisx)->params)
#define BUTTERFLY_GET_1(thisx) ((thisx)->params & 1)

#define BUTTERFLY_MINUS1 -1
#define BUTTERFLY_0 0
#define BUTTERFLY_1 1

#define BUTTERFLY_PARAMS(param) (param)

typedef struct EnButte {
    /* 0x000 */ Actor actor;
    /* 0x144 */ ColliderJntSph collider;
    /* 0x164 */ ColliderJntSphElement colldierElements[1];
    /* 0x1A4 */ SkelAnime skelAnime;
    /* 0x1E8 */ Vec3s jointTable[BUTTERFLY_LIMB_MAX];
    /* 0x218 */ Vec3s morphTable[BUTTERFLY_LIMB_MAX];
    /* 0x248 */ EnButteActionFunc actionFunc;
    /* 0x24C */ s16 unk_24C;
    /* 0x24E */ u8 unk_24E;
    /* 0x24F */ u8 unk_24F;
    /* 0x250 */ u8 unk_250;
    /* 0x252 */ s16 unk_252;
    /* 0x254 */ s16 unk_254;
    /* 0x256 */ s16 unk_256;
    /* 0x258 */ s16 unk_258;
    /* 0x25C */ f32 unk_25C;
} EnButte; // size = 0x260

#endif // Z_EN_BUTTE_H
