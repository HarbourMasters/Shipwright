#ifndef Z_DM_CHAR00_H
#define Z_DM_CHAR00_H

#include "global.h"
#include "objects/gameplay_keep/gameplay_keep.h"

struct DmChar00;

typedef void (*DmChar00ActionFunc)(struct DmChar00*, PlayState*);

#define DMCHAR00_GET(thisx) ((thisx)->params)
#define DMCHAR00_GET_F800(thisx) ((thisx)->params << 0xB)

#define DMCHAR00_0 0
#define DMCHAR00_1 1

typedef struct DmChar00 {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ Vec3s jointTable[FAIRY_LIMB_MAX];
    /* 0x1B2 */ UNK_TYPE1 unk_1B2[0x30];
    /* 0x1E2 */ Vec3s morphTable[FAIRY_LIMB_MAX];
    /* 0x20C */ UNK_TYPE1 unk_20C[0x30];
    /* 0x23C */ DmChar00ActionFunc actionFunc;
    /* 0x240 */ Color_RGBAf unk_240;
    /* 0x250 */ Color_RGBAf unk_250;
    /* 0x260 */ u8 cueId;
    /* 0x261 */ u8 animIndex;
    /* 0x262 */ u16 unk_262;
} DmChar00; // size = 0x264

#endif // Z_DM_CHAR00_H
