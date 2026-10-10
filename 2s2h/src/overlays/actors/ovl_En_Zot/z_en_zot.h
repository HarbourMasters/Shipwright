#ifndef Z_EN_ZOT_H
#define Z_EN_ZOT_H

#include "global.h"
#include "assets/objects/object_zo/object_zo.h"

struct EnZot;

typedef void (*EnZotActionFunc)(struct EnZot*, PlayState*);

#define ENZOT_GET_1F(thisx) ((thisx)->params & 0x1F)
#define ENZOT_GET_PATH_INDEX(thisx) (((thisx)->params & 0xFC00) >> 0xA)
#define ENZOT_GET_SWITCH_FLAG(thisx) (((thisx)->home.rot.z) & 0x7F)

#define ENZOT_PATH_INDEX_NONE 0x3F

typedef struct EnZot {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ ColliderCylinder collider;
    /* 0x1D4 */ Vec3s jointTable[ZORA_LIMB_MAX];
    /* 0x24C */ Vec3s morphTable[ZORA_LIMB_MAX];
    /* 0x2C4 */ Vec3s headRot;
    /* 0x2CA */ Vec3s torsoRot;
    /* 0x2D0 */ Path* path;
    /* 0x2D4 */ s32 unk_2D4;
    /* 0x2D8 */ Actor* unk_2D8[5];
    /* 0x2EC */ s16 unk_2EC;
    /* 0x2EE */ s16 unk_2EE;
    /* 0x2F0 */ s16 animIndex;
    /* 0x2F2 */ u16 unk_2F2;
    /* 0x2F4 */ s16 unk_2F4;
    /* 0x2F8 */ EnZotActionFunc actionFunc;
} EnZot; // size = 0x2FC

#endif // Z_EN_ZOT_H
