#ifndef Z_BG_JYA_COBRA_H
#define Z_BG_JYA_COBRA_H

#include <libultraship/libultra.h>
#include "global.h"

struct BgJyaCobra;

typedef void (*BgJyaCobraActionFunc)(struct BgJyaCobra*, PlayState*);

#define COBRA_SHADOW_TEX_WIDTH 64
#define COBRA_SHADOW_TEX_HEIGHT 64
#define COBRA_SHADOW_TEX_SIZE (COBRA_SHADOW_TEX_WIDTH * COBRA_SHADOW_TEX_HEIGHT * G_IM_SIZ_8b_BYTES)
#define COBRA_SHADOW_TEX_PTR(this) (u8*)ALIGN16((uintptr_t)&(this)->shadowTextureBuffer)

typedef struct BgJyaCobra {
    /* 0x0000 */ DynaPolyActor dyna;
    /* 0x0164 */ BgJyaCobraActionFunc actionFunc;
    /* 0x0168 */ s16 unk_168;
    /* 0x016A */ s16 unk_16A;
    /* 0x016C */ s16 unk_16C;
    /* 0x016E */ s16 unk_16E;
    /* 0x0170 */ s16 unk_170;
    /* 0x0172 */ u8 unk_172;
    /* 0x0174 */ Vec3f unk_174;
    /* 0x0180 */ Vec3f unk_180;
    /* 0x018C */ f32 unk_18C;
    /* 0x0190 */ f32 unk_190;
    /* 0x0194 */ u8 shadowTextureBuffer[COBRA_SHADOW_TEX_SIZE / sizeof(u8) + 0xF]; // Extra space to allow aligning actual texture to 0x10
} BgJyaCobra; // size = 0x11A4

#endif
