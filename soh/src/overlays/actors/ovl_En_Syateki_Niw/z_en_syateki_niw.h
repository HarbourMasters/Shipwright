#ifndef Z_EN_SYATEKI_NIW_H
#define Z_EN_SYATEKI_NIW_H

#include "ultra64.h"
#include "global.h"

struct EnSyatekiNiw;

typedef void (*EnSyatekiNiwActionFunc)(struct EnSyatekiNiw*, PlayState*);

typedef struct {
    /* 0x00 */ u8    unk_00;
    /* 0x0C */ Vec3f unk_04;
    /* 0x10 */ Vec3f unk_10;
    /* 0x1C */ Vec3f unk_1C;
    /* 0x28 */ s16   unk_28;
    /* 0x2A */ s16   unk_2A;
    /* 0x2C */ f32   unk_2C;
    /* 0x30 */ f32   unk_30;
    /* 0x34 */ u8    unk_34;
    u32 epoch;
} EnSyatekiNiw_1; // size = 0x38

typedef struct EnSyatekiNiw {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;
    /* 0x0190 */ Vec3s jointTable[16];
    /* 0x01F0 */ Vec3s morphTable[16];
    /* 0x0250 */ EnSyatekiNiwActionFunc actionFunc;
    /* 0x0254 */ s16 unk_254;
    /* 0x0256 */ s16 unk_256;
    /* 0x0258 */ s16 unk_258;
    /* 0x025A */ s16 unk_25A;
    /* 0x025C */ s16 unk_25C;
    /* 0x025E */ s16 unk_25E;
    /* 0x0260 */ s16 unk_260;
    /* 0x0262 */ s16 unk_262;
    /* 0x0264 */ f32 unk_264;
    /* 0x0268 */ f32 unk_268;
    /* 0x026C */ f32 unk_26C;
    /* 0x0270 */ char unk_270[0x8];
    /* 0x0278 */ f32 unk_278;
    /* 0x027C */ f32 unk_27C;
    /* 0x0284 */ f32 unk_280;
    /* 0x0280 */ f32 unk_284;
    /* 0x0288 */ f32 unk_288;
    /* 0x028C */ s16 unk_28C;
    /* 0x028E */ s16 unk_28E;
    /* 0x0290 */ s16 unk_290;
    /* 0x0292 */ s16 unk_292;
    /* 0x0294 */ s16 unk_294;
    /* 0x0296 */ s16 unk_296;
    /* 0x0298 */ s16 unk_298;
    /* 0x029C */ s16 unk_29A;
    /* 0x029C */ s16 unk_29C;
    /* 0x029E */ s16 unk_29E;
    /* 0x02A0 */ s16 unk_2A0;
    /* 0x02A4 */ Vec3f unk_2A4;
    /* 0x02B0 */ Vec3f unk_2B0;
    /* 0x02BC */ Vec3f unk_2BC;
    /* 0x02C8 */ Vec3f unk_2C8;
    /* 0x02D4 */ f32 unk_2D4;
    /* 0x02D8 */ f32 unk_2D8;
    /* 0x02DC */ Vec3f unk_2DC;
    /* 0x02E8 */ Vec3f unk_2E8;
    /* 0x02F4 */ f32 unk_2F4;
    /* 0x02F8 */ u8 unk_2F8;
    /* 0x02FC */ ColliderCylinder collider;
    /* 0x0348 */ EnSyatekiNiw_1 unk_348[5];
} EnSyatekiNiw; // size = 0x0460

#endif
