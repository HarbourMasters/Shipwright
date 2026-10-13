#ifndef Z_EN_DAIKU_H
#define Z_EN_DAIKU_H

#include "global.h"
#include "objects/object_daiku/object_daiku.h"

struct EnDaiku;

typedef void (*EnDaikuActionFunc)(struct EnDaiku*, PlayState*);

#define ENDAIKU_GET_FF(thisx) ((thisx)->params & 0xFF)
#define ENDAIKU_GET_PATH_INDEX(thisx) (((thisx)->params >> 8) & 0xFF)

#define ENDAIKU_PATH_INDEX_NONE 0x3F

typedef enum {
    /* 0 */ ENDAIKU_PARAM_FF_0,
    /* 1 */ ENDAIKU_PARAM_FF_1,
    /* 2 */ ENDAIKU_PARAM_FF_2,
    /* 3 */ ENDAIKU_PARAM_FF_3
} EnDaikuParam;

typedef struct EnDaiku {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ Vec3s jointTable[OBJECT_DAIKU_LIMB_MAX];
    /* 0x1EE */ Vec3s morphTable[OBJECT_DAIKU_LIMB_MAX];
    /* 0x254 */ EnDaikuActionFunc actionFunc;
    /* 0x258 */ Path* path;
    /* 0x25C */ s16 unk_25C;
    /* 0x25E */ s16 unk_25E;
    /* 0x260 */ s16 unk_260;
    /* 0x262 */ UNK_TYPE1 unk_262[0x2];
    /* 0x264 */ s16 unk_264;
    /* 0x266 */ s16 unk_266;
    /* 0x268 */ UNK_TYPE1 unk_268[0x4];
    /* 0x26C */ Vec3f unk_26C;
    /* 0x278 */ s32 unk_278;
    /* 0x27C */ s16 unk_27C;
    /* 0x27E */ s16 unk_27E;
    /* 0x280 */ s16 unk_280;
    /* 0x282 */ s16 unk_282;
    /* 0x284 */ f32 animEndFrame;
    /* 0x288 */ s16 pathIndex;
    /* 0x28A */ s16 unk_28A;
    /* 0x28C */ s16 unk_28C;
    /* 0x28E */ UNK_TYPE1 unk_28E[0xE];
    /* 0x29C */ ColliderCylinder collider;
} EnDaiku; // size = 0x2E8

#endif // Z_EN_DAIKU_H
