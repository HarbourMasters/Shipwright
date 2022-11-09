#ifndef Z_EN_GB_H
#define Z_EN_GB_H

#include "ultra64.h"
#include "global.h"

struct EnGb;

typedef void (*EnGbActionFunc)(struct EnGb*, PlayState*);

typedef struct {
    /* 0x00 */ Color_RGBA8 prim;
    /* 0x04 */ Color_RGBA8 env;
    /* 0x08 */ void* texture;
    /* 0x0C */ s16 timerMultiplier;
} EnGbCagedSoulInfo; // size = 0x10

typedef struct {
    /* 0x00 */ u8    infoIdx;
    /* 0x01 */ u8    unk_1;
    /* 0x02 */ u8    unk_2;
    /* 0x03 */ u8    unk_3;
    /* 0x04 */ u8    rotate180;
    /* 0x06 */ s16   unk_6;
    /* 0x08 */ Vec3f translation;
    /* 0x14 */ Vec3f unk_14;
    /* 0x20 */ f32   unk_20;
    /* 0x24 */ f32   unk_24;
    /* 0x28 */ f32   unk_28;
    u32 epoch;
} EnGbCagedSoul; // size = 0x2C

typedef struct EnGb {
    /* 0x0000 */ DynaPolyActor dyna;
    /* 0x0164 */ SkelAnime skelAnime;
    /* 0x01A8 */ Vec3s jointTable[12];
    /* 0x01F0 */ Vec3s morphTable[12];
    /* 0x0238 */ EnGbActionFunc actionFunc;
    /* 0x023C */ ColliderCylinder collider;
    /* 0x0288 */ ColliderCylinder bottlesColliders[3];
    /* 0x036C */ LightNode* light;
    /* 0x0370 */ LightInfo lightInfo;
    /* 0x037E */ u16 textId;
    /* 0x0380 */ s16 frameTimer;
    /* 0x0382 */ s16 actionTimer;
    /* 0x0384 */ Color_RGBA8 lightColor;
    /* 0x0388 */ EnGbCagedSoul cagedSouls[4];
} EnGb; // size = 0x0438

#endif
