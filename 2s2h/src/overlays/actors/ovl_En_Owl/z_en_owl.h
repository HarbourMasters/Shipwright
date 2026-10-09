#ifndef Z_EN_OWL_H
#define Z_EN_OWL_H

#include "global.h"
#include "objects/object_owl/object_owl.h"

struct EnOwl;

typedef void (*EnOwlActionFunc)(struct EnOwl*, PlayState*);
typedef void (*EnOwlFunc)(struct EnOwl*);

#define ENOWL_GET_PATH_INDEX(thisx) (((thisx)->params & 0xF000) >> 0xC)
#define ENOWL_GET_TYPE(thisx) (((thisx)->params & 0xF80) >> 7)
#define ENOWL_GET_SWITCH_FLAG(thisx) ((thisx)->params & 0x7F)

#define ENOWL_PATH_INDEX_NONE 0xF
#define ENOWL_SWITCH_FLAG_NONE 0x7F

typedef enum {
    /* 0x001 */ ENOWL_GET_TYPE_1 = 1,
    /* 0x002 */ ENOWL_GET_TYPE_2,
    /* 0x003 */ ENOWL_GET_TYPE_3,
    /* 0x01E */ ENOWL_GET_TYPE_30 = 30,
    /* 0x3E8 */ ENOWL_GET_TYPE_1000 = 1000
} EnOwlType;

typedef struct EnOwl {
    /* 0x000 */ Actor actor;
    /* 0x144 */ ColliderCylinder collider;
    /* 0x190 */ SkelAnime skelAnimeFlying;
    /* 0x1D4 */ Vec3s jointTableFlying[OWL_FLYING_LIMB_MAX];
    /* 0x252 */ Vec3s morphTableFlying[OWL_FLYING_LIMB_MAX];
    /* 0x2D0 */ SkelAnime skelAnimePerching;
    /* 0x314 */ Vec3s jointTablePerching[OWL_PERCHING_LIMB_MAX];
    /* 0x374 */ Vec3s morphTablePerching[OWL_PERCHING_LIMB_MAX];
    /* 0x3D4 */ SkelAnime* curSkelAnime; // Points to the currently active SkelAnime
    /* 0x3D8 */ s16 unk_3D8;
    /* 0x3DA */ s16 unk_3DA;
    /* 0x3DC */ s16 unk_3DC;
    /* 0x3DE */ s16 unk_3DE;
    /* 0x3E0 */ s16 eyeTexIndex;
    /* 0x3E2 */ s16 blinkTimer;
    /* 0x3E4 */ f32 unk_3E4;
    /* 0x3E8 */ u16 actionFlags;
    /* 0x3EA */ s16 unk_3EA;
    /* 0x3EC */ s16 unk_3EC;
    /* 0x3F0 */ f32 unk_3F0;
    /* 0x3F4 */ Path* path;
    /* 0x3F8 */ s32 unk_3F8;
    /* 0x3FC */ s32 pathIndex;
    /* 0x400 */ s16 csIdList[3];
    /* 0x406 */ s16 csIdIndex;
    /* 0x408 */ u8 unk_408;
    /* 0x409 */ u8 unk_409;
    /* 0x40A */ u8 unk_40A;
    /* 0x40B */ u8 unk_40B;
    /* 0x40C */ u8 unk_40C;
    /* 0x40D */ u8 unk_40D;
    /* 0x410 */ EnOwlActionFunc actionFunc;
    /* 0x414 */ EnOwlFunc unk_414;
} EnOwl; // size = 0x418

#endif // Z_EN_OWL_H
