#ifndef Z_EN_SKB_H
#define Z_EN_SKB_H

#include "global.h"
#include "objects/object_skb/object_skb.h"

struct EnSkb;

typedef void (*EnSkbActionFunc)(struct EnSkb*, PlayState*);

#define ENSKB_GET_F0(thisx) (((thisx)->params >> 4) & 0xF)

typedef enum {
    /* 0 */ ENSKB_PARAM_0
} EnSkbParam;

typedef enum EnSkbBodyPart {
    /*  0 */ ENSKB_BODYPART_0,
    /*  1 */ ENSKB_BODYPART_1,
    /*  2 */ ENSKB_BODYPART_2,
    /*  3 */ ENSKB_BODYPART_3,
    /*  4 */ ENSKB_BODYPART_4,
    /*  5 */ ENSKB_BODYPART_5,
    /*  6 */ ENSKB_BODYPART_6,
    /*  7 */ ENSKB_BODYPART_7,
    /*  8 */ ENSKB_BODYPART_8,
    /*  9 */ ENSKB_BODYPART_9,
    /* 10 */ ENSKB_BODYPART_10,
    /* 11 */ ENSKB_BODYPART_11,
    /* 12 */ ENSKB_BODYPART_12,
    /* 13 */ ENSKB_BODYPART_13,
    /* 14 */ ENSKB_BODYPART_MAX
} EnSkbBodyPart;

typedef struct EnSkb {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ ColliderJntSph collider;
    /* 0x1A8 */ ColliderJntSphElement colliderElements[2];
    /* 0x228 */ EnSkbActionFunc actionFunc;
    /* 0x22C */ f32 drawDmgEffAlpha;
    /* 0x230 */ f32 drawDmgEffScale;
    /* 0x234 */ Vec3f bodyPartsPos[ENSKB_BODYPART_MAX];
    /* 0x2DC */ s32 bodyPartsCount;
    /* 0x2E0 */ Vec3s jointTable[STALCHILD_LIMB_MAX];
    /* 0x358 */ Vec3s morphTable[STALCHILD_LIMB_MAX];
    /* 0x3D0 */ s16 unk_3D0;
    /* 0x3D2 */ s16 drawDmgEffTimer;
    /* 0x3D4 */ s16 unk_3D4;
    /* 0x3D6 */ s16 unk_3D6;
    /* 0x3D8 */ s16 unk_3D8;
    /* 0x3DA */ s16 unk_3DA;
    /* 0x3DC */ s16 unk_3DC;
    /* 0x3DE */ s16 unk_3DE;
    /* 0x3E0 */ s16 unk_3E0;
    /* 0x3E2 */ s16 unk_3E2;
    /* 0x3E4 */ u8 unk_3E4;
    /* 0x3E5 */ u8 unk_3E5;
    /* 0x3E6 */ u8 drawDmgEffType;
} EnSkb; // size = 0x3E8

#endif // Z_EN_SKB_H
