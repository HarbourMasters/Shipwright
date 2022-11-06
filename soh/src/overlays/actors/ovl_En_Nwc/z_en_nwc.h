#ifndef Z_EN_NWC_H
#define Z_EN_NWC_H

#include "ultra64.h"
#include "global.h"

struct EnNwc;
struct EnNwcChick;

typedef void (*EnNwcUpdateFunc)(struct EnNwc*, PlayState*);
typedef void (*EnNwcChickFunc)(struct EnNwcChick*, struct EnNwc*, PlayState*);

typedef struct EnNwcChick {
    /* 0x00 */ s8 type;
    /* 0x01 */ u8 bgFlags;
    /* 0x04 */ f32 floorY;
    /* 0x08 */ Vec3f pos;
    /* 0x14 */ char unk_14[8];
    /* 0x1C */ Vec3f lastPos;
    /* 0x28 */ char unk_28[4];
    /* 0x2C */ f32 velY;
    /* 0x30 */ Vec3s rot;
    /* 0x36 */ u16 height;
    /* 0x38 */ CollisionPoly* floorPoly;
    /* 0x44 */ char unk_3C[0x20]; 
    u32 epoch;
} EnNwcChick; // size = 0x5C

typedef struct EnNwc {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ ColliderJntSph collider;
    /* 0x016C */ u8 count;
    /* 0x0170 */ EnNwcChick chicks[16];
    /* 0x0730 */ EnNwcUpdateFunc updateFunc;
} EnNwc; // size = 0x0734

#endif
