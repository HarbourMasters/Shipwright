#ifndef Z_OBJECT_KANKYO_H
#define Z_OBJECT_KANKYO_H

#include "global.h"

struct ObjectKankyo;

typedef void (*ObjectKankyoActionFunc)(struct ObjectKankyo*, PlayState*);

typedef struct {
    /* 0x00 */ f32 unk_00;
    /* 0x04 */ f32 unk_04;
    /* 0x08 */ f32 unk_08;
    /* 0x0C */ f32 unk_0C;
    /* 0x10 */ f32 unk_10;
    /* 0x14 */ f32 unk_14;
    /* 0x18 */ f32 unk_18;
    /* 0x1C */ u8 unk_1C;
    // 2S2H [Port]
    /*      */ s16 epoch; // Tracks when an effect is reset for use with interpolation skipping
} ObjectKankyoStruct; // size = 0x20

typedef struct ObjectKankyo {
    /* 0x0000 */ Actor actor;
    /* 0x0144 */ f32 unk_144;
    /* 0x0148 */ f32 unk_148;
    /* 0x014C */ ObjectKankyoStruct unk_14C[128];
    /* 0x114C */ u16 unk_114C;
    /* 0x114E */ u8 unk_114E;
    /* 0x1150 */ ObjectKankyoActionFunc actionFunc;
} ObjectKankyo; // size = 0x1154

#endif // Z_OBJECT_KANKYO_H
