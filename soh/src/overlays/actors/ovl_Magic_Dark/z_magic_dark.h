#ifndef Z_MAGIC_DARK_H
#define Z_MAGIC_DARK_H

#include "ultra64.h"
#include "global.h"

struct MagicDark;

typedef struct MagicDark {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ s16 timer;
    /* 0x014E */ u8 primAlpha;
    /* 0x0150 */ Vec3f orbOffset;
    /* 0x015C */ f32 scale;
    /* 0x0160 */ char unk_160[0x4];
} MagicDark; // size = 0x0164

#endif
