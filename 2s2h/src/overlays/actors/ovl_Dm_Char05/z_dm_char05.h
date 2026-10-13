#ifndef Z_DM_CHAR05_H
#define Z_DM_CHAR05_H

#include "global.h"

struct DmChar05;

typedef void (*DmChar05ActionFunc)(struct DmChar05*, PlayState*);

#define DMCHAR05_GET(thisx) ((thisx)->params)

typedef enum {
    /* 0 */ DMCHAR05_0,
    /* 1 */ DMCHAR05_1,
    /* 2 */ DMCHAR05_2,
    /* 3 */ DMCHAR05_3,
    /* 4 */ DMCHAR05_4,
    /* 5 */ DMCHAR05_5,
    /* 6 */ DMCHAR05_6,
    /* 7 */ DMCHAR05_7,
    /* 8 */ DMCHAR05_8,
    /* 9 */ DMCHAR05_9,
    /* 10 */ DMCHAR05_10,
    /* 11 */ DMCHAR05_11,
    /* 12 */ DMCHAR05_12
} DmChar05Param;

typedef struct DmChar05 {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ DmChar05ActionFunc actionFunc;
    /* 0x18C */ s16 animIndex;
    /* 0x18E */ u8 unk_18E;
    /* 0x18F */ u8 objectSlot;
    /* 0x190 */ Vec3f unk_190;
    /* 0x19C */ u32 unk_19C;
    /* 0x1A0 */ u32 unk_1A0;
    /* 0x1A4 */ u32 unk_1A4;
    /* 0x1A8 */ s32 unk_1A8;
    /* 0x1AC */ s32 unk_1AC;
    /* 0x1B0 */ s32 unk_1B0;
    /* 0x1B4 */ u8 unk_1B4;
} DmChar05; // size = 0x1B8

#endif // Z_DM_CHAR05_H
