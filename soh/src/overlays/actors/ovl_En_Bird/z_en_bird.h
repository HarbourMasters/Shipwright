#ifndef Z_EN_BIRD_H
#define Z_EN_BIRD_H

#include "ultra64.h"
#include "global.h"

struct EnBird;

typedef void (*EnBirdActionFunc)(struct EnBird*, PlayState*);

typedef struct EnBird {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;
    /* 0x0190 */ EnBirdActionFunc actionFunc;
    /* 0x0194 */ u32 unk_194;
    /* 0x0198 */ s32 unk_198;
    /* 0x019C */ s16 unk_19C;
    /* 0x019E */ char unk_19E[0x2];
    /* 0x01A0 */ f32 unk_1A0;
    /* 0x01A4 */ f32 unk_1A4;
    /* 0x01A8 */ f32 unk_1A8;
    /* 0x01AC */ f32 unk_1AC;
    /* 0x01B0 */ f32 unk_1B0;
    /* 0x01B4 */ f32 unk_1B4;
    /* 0x01B8 */ f32 unk_1B8;
    /* 0x01BC */ f32 unk_1BC;
    /* 0x01C0 */ s16 unk_1C0;
    /* 0x01C2 */ char unk_1C2[0x1A];
} EnBird; // size = 0x01DC

#endif
