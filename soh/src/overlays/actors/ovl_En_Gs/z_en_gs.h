#ifndef Z_EN_GS_H
#define Z_EN_GS_H

#include "ultra64.h"
#include "global.h"

struct EnGs;

typedef void (*EnGsActionFunc)(struct EnGs*, PlayState*);

typedef struct EnGs {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ ColliderCylinder collider;
    /* 0x0198 */ EnGsActionFunc actionFunc;
    /* 0x019C */ u8 unk_19C;
    /* 0x019D */ u8 unk_19D;
    /* 0x019E */ u8 unk_19E;
    /* 0x019F */ u8 unk_19F;
    /* 0x01A0 */ Vec3s unk_1A0[3];
    /* 0x01B4 */ Vec3f unk_1B4[2];
    /* 0x01CC */ char unk_1CC[0xC];
    /* 0x01D8 */ Vec3f unk_1D8;
    /* 0x01E4 */ Color_RGBA8 flashColor;
    /* 0x01E8 */ f32 unk_1E8;
    /* 0x01EC */ f32 unk_1EC;
    /* 0x01F0 */ f32 unk_1F0;
    /* 0x01F4 */ f32 unk_1F4;
    /* 0x01F8 */ f32 unk_1F8;
    /* 0x01FC */ f32 unk_1FC;
    /* 0x0200 */ u16 unk_200;
    /* 0x0202 */ char unk_202[0x6];
} EnGs; // size = 0x0208

#endif
