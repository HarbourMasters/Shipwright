#ifndef Z_EN_HINT_SKB_H
#define Z_EN_HINT_SKB_H

#include "global.h"
#include "objects/object_skb/object_skb.h"

struct EnHintSkb;

typedef void (*EnHintSkbActionFunc)(struct EnHintSkb*, PlayState*);

#define ENHINTSKB_GET_FF(thisx) ((thisx)->params & 0xFF)

typedef enum EnHintSkbBodyPart {
    /*  0 */ ENHINTSKB_BODYPART_0,
    /*  1 */ ENHINTSKB_BODYPART_1,
    /*  2 */ ENHINTSKB_BODYPART_2,
    /*  3 */ ENHINTSKB_BODYPART_3,
    /*  4 */ ENHINTSKB_BODYPART_4,
    /*  5 */ ENHINTSKB_BODYPART_5,
    /*  6 */ ENHINTSKB_BODYPART_6,
    /*  7 */ ENHINTSKB_BODYPART_7,
    /*  8 */ ENHINTSKB_BODYPART_8,
    /*  9 */ ENHINTSKB_BODYPART_9,
    /* 10 */ ENHINTSKB_BODYPART_10,
    /* 11 */ ENHINTSKB_BODYPART_11,
    /* 12 */ ENHINTSKB_BODYPART_12,
    /* 13 */ ENHINTSKB_BODYPART_13,
    /* 14 */ ENHINTSKB_BODYPART_MAX
} EnHintSkbBodyPart;

typedef struct EnHintSkb {
    /* 0x000 */ Actor actor;
    /* 0x144 */ ColliderJntSph collider;
    /* 0x164 */ ColliderJntSphElement colliderElement[2];
    /* 0x1E4 */ SkelAnime skelAnime;
    /* 0x228 */ EnHintSkbActionFunc actionFunc;
    /* 0x22C */ Vec3f bodyPartsPos[ENHINTSKB_BODYPART_MAX];
    /* 0x2D4 */ f32 drawDmgEffAlpha;
    /* 0x2D8 */ f32 drawDmgEffScale;
    /* 0x2DC */ s32 bodyPartsCount;
    /* 0x2E0 */ Vec3s jointTable[STALCHILD_LIMB_MAX];
    /* 0x358 */ Vec3s morphTable[STALCHILD_LIMB_MAX];
    /* 0x3D0 */ UNK_TYPE1 unk3D0[0xC];
    /* 0x3DC */ s16 unk_3DC;
    /* 0x3DE */ s16 unk_3DE;
    /* 0x3E0 */ s16 unk_3E0;
    /* 0x3E2 */ s16 drawDmgEffTimer;
    /* 0x3E4 */ s16 unk_3E4;
    /* 0x3E6 */ u16 unk_3E6;
    /* 0x3E8 */ u8 unk_3E8;
    /* 0x3E9 */ u8 drawDmgEffType;
} EnHintSkb; // size = 0x3EC

#endif // Z_EN_HINT_SKB_H
