#ifndef Z_EN_TRT2_H
#define Z_EN_TRT2_H

#include "global.h"

struct EnTrt2;

typedef void (*EnTrt2ActionFunc)(struct EnTrt2*, PlayState*);
typedef void (*EnTrt2UnkFunc)(struct EnTrt2*);
typedef void (*EnTrt2UnkFunc2)(struct EnTrt2*, PlayState*);

#define ENTRT2_GET_PATH_INDEX(thisx) (((thisx)->params & 0xFC00) >> 0xA)

#define ENTRT2_PATH_INDEX_NONE 0x3F

typedef struct EnTrt2 {
    /* 0x000 */ Actor actor;
    /* 0x144 */ EnTrt2ActionFunc actionFunc;
    /* 0x148 */ UNK_TYPE1 unk_148[8];
    /* 0x150 */ SkelAnime skelAnime;
    /* 0x194 */ ColliderCylinder collider;
    /* 0x1E0 */ Path* path;
    /* 0x1E4 */ s32 unk_1E4;
    /* 0x1E8 */ UNK_TYPE1 unk_1E8[0x154];
    /* 0x33C */ s16 unk_33C[27];
    /* 0x372 */ s16 unk_372[27];
    /* 0x3A8 */ u16 unk_3A8;
    /* 0x3AA */ UNK_TYPE1 unk_3AA[4];
    /* 0x3AE */ s16 unk_3AE;
    /* 0x3B0 */ s16 unk_3B0;
    /* 0x3B2 */ s16 unk_3B2;
    /* 0x3B4 */ s16 unk_3B4;
    /* 0x3B6 */ s16 unk_3B6;
    /* 0x3B8 */ s16 unk_3B8;
    /* 0x3BC */ EnTrt2UnkFunc unk_3BC;
    /* 0x3C0 */ s16 unk_3C0;
    /* 0x3C2 */ Vec3s unk_3C2;
    /* 0x3C8 */ Vec3f unk_3C8;
    /* 0x3D4 */ s16 unk_3D4;
    /* 0x3D6 */ s16 unk_3D6;
    /* 0x3D8 */ u8 unk_3D8;
    /* 0x3D9 */ u8 unk_3D9;
    /* 0x3DA */ s16 csId;
} EnTrt2; // size = 0x3DC

#endif // Z_EN_TRT2_H
