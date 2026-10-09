#ifndef Z_EN_DAIKU2_H
#define Z_EN_DAIKU2_H

#include "global.h"
#include "objects/object_daiku/object_daiku.h"

struct EnDaiku2;

typedef void (*EnDaiku2ActionFunc)(struct EnDaiku2*, PlayState*);

#define ENDAIKU2_GET_SWITCH_FLAG(thisx) ((thisx)->params & 0x7F)
#define ENDAIKU2_GET_PATH_INDEX(thisx) (((thisx)->params >> 7) & 0x3F)
#define ENDAIKU2_GET_8000(thisx) (((thisx)->params >> 15) & 0x1)

#define ENDAIKU2_SWITCH_FLAG_NONE 0x7F
#define ENDAIKU2_PATH_INDEX_NONE 0x3F

typedef struct EnDaiku2Effect {
    /* 0x00 */ u8 isEnabled;
    /* 0x04 */ Vec3f unk_04;
    /* 0x10 */ Vec3f unk_10;
    /* 0x1C */ Vec3f unk_1C;
    /* 0x28 */ Vec3s unk_28;
    /* 0x30 */ f32 unk_30;
    /* 0x34 */ s16 unk_34;
} EnDaiku2Effect; // size = 0x38

#define EN_DAIKU2_EFFECT_COUNT 50

typedef struct EnDaiku2 {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ Vec3s jointTable[OBJECT_DAIKU_LIMB_MAX];
    /* 0x1EE */ Vec3s morphTable[OBJECT_DAIKU_LIMB_MAX];
    /* 0x254 */ EnDaiku2ActionFunc actionFunc;
    /* 0x258 */ Path* path;
    /* 0x25C */ s16 unk_25C;
    /* 0x260 */ f32 unk_260;
    /* 0x264 */ f32 unk_264;
    /* 0x268 */ Vec3f unk_268;
    /* 0x274 */ s16 unk_274;
    /* 0x276 */ s16 animIndex;
    /* 0x278 */ s16 switchFlag;
    /* 0x27A */ s16 pathIndex;
    /* 0x27C */ UNK_TYPE1 unk_27C[0x4];
    /* 0x280 */ s32 unk_280;
    /* 0x284 */ f32 animEndFrame;
    /* 0x288 */ s16 unk_288;
    /* 0x28A */ s16 unk_28A;
    /* 0x28C */ EnDaiku2Effect effects[EN_DAIKU2_EFFECT_COUNT];
    /* 0xD7C */ ColliderCylinder collider;
} EnDaiku2; // size = 0xDC8

#endif // Z_EN_DAIKU2_H
