#ifndef Z_DM_CHAR09_H
#define Z_DM_CHAR09_H

#include "global.h"
#include "objects/object_bee/object_bee.h"

#define DMCHAR09_GET_PATH_INDEX(thisx) (((thisx)->params >> 4) & 0xF)
#define DMCHAR09_GET_100(thisx) ((thisx)->params & 0x100)
#define DMCHAR09_GET_F(thisx) ((thisx)->params & 0xF)

struct DmChar09;

typedef void (*DmChar09ActionFunc)(struct DmChar09*, PlayState*);

typedef struct DmChar09 {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ Vec3s jointTable[OBJECT_BEE_LIMB_MAX];
    /* 0x1C4 */ Vec3s morphTable[OBJECT_BEE_LIMB_MAX];
    /* 0x200 */ DmChar09ActionFunc actionFunc;
    /* 0x204 */ f32 unk_204;
    /* 0x208 */ f32 unk_208;
    /* 0x20C */ f32 unk_20C;
    /* 0x210 */ f32 speed;
    /* 0x214 */ s32 unk_214;
    /* 0x218 */ s32 unk_218;
    /* 0x21C */ s32 unk_21C;
    /* 0x220 */ s32 unk_220;
    /* 0x224 */ Vec3s* pathPoints;
    /* 0x228 */ s16 unk_228;
    /* 0x22A */ s16 unk_22A;
    /* 0x22C */ UNK_TYPE1 pad22C[2];
    /* 0x22E */ u8 unk_22E;
    /* 0x22F */ u8 cueId;
} DmChar09; // size = 0x230

#endif // Z_DM_CHAR09_H
