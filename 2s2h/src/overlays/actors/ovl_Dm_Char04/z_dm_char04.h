#ifndef Z_DM_CHAR04_H
#define Z_DM_CHAR04_H

#include "global.h"
#include "objects/gameplay_keep/gameplay_keep.h"

struct DmChar04;

typedef void (*DmChar04ActionFunc)(struct DmChar04*, PlayState*);

typedef struct DmChar04 {
    /* 0x0000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ Vec3s jointTable[FAIRY_LIMB_MAX];
    /* 0x1B2 */ UNK_TYPE1 unk_1B2[0x30];
    /* 0x1E2 */ Vec3s morphTable[FAIRY_LIMB_MAX];
    /* 0x20C */ UNK_TYPE1 unk_20C[0x30];
    /* 0x23C */ DmChar04ActionFunc actionFunc;
    /* 0x240 */ Color_RGBAf primColors;
    /* 0x250 */ Color_RGBAf envColors;
    /* 0x260 */ u8 cueId;
    /* 0x261 */ u8 animIndex;
    /* 0x262 */ u16 timer;
} DmChar04; // size = 0x264

#endif // Z_DM_CHAR04_H
