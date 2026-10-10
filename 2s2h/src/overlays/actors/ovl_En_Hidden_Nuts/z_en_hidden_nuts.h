#ifndef Z_EN_HIDDEN_NUTS_H
#define Z_EN_HIDDEN_NUTS_H

#include "global.h"
#include "objects/object_hintnuts/object_hintnuts.h"

struct EnHiddenNuts;

typedef void (*EnHiddenNutsActionFunc)(struct EnHiddenNuts*, PlayState*);

#define ENHIDDENNUTS_GET_SWITCH_FLAG(thisx) ((thisx)->params & 0x7F)
#define ENHIDDENNUTS_GET_PATH_INDEX(thisx) (((thisx)->params >> 7) & 0x1F)

#define ENHIDDENNUTS_SWITCH_FLAG_NONE 0x7F
#define ENHIDDENNUTS_PATH_INDEX_NONE 0x1F
#define ENHIDDENNUTS_PATH_INDEX_NONE_ALT 0x3F

typedef struct EnHiddenNuts {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ Vec3s jointTable[OBJECT_HINTNUTS_LIMB_MAX];
    /* 0x1C4 */ Vec3s morphTable[OBJECT_HINTNUTS_LIMB_MAX];
    /* 0x200 */ EnHiddenNutsActionFunc actionFunc;
    /* 0x204 */ Path* path;
    /* 0x208 */ s16 unk_208;
    /* 0x20A */ u8 unk_20A;
    /* 0x20C */ Vec3f unk_20C;
    /* 0x218 */ s16 unk_218;
    /* 0x21A */ s16 unk_21A;
    /* 0x21C */ s16 switchFlag;
    /* 0x21E */ s16 pathIndex;
    /* 0x220 */ s32 animIndex;
    /* 0x224 */ s16 unk_224;
    /* 0x226 */ s16 csId;
    /* 0x228 */ f32 unk_228;
    /* 0x22C */ f32 animEndFrame;
    /* 0x230 */ UNK_TYPE1 unk230[4];
    /* 0x234 */ ColliderCylinder collider;
} EnHiddenNuts; // size = 0x280

#endif // Z_EN_HIDDEN_NUTS_H
