#ifndef Z_EN_ATTACK_NIW_H
#define Z_EN_ATTACK_NIW_H

#include "ultra64.h"
#include "global.h"

struct EnAttackNiw;

typedef void (*EnAttackNiwActionFunc)(struct EnAttackNiw*, PlayState*);

typedef struct EnAttackNiw {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;
    /* 0x0190 */ Vec3s jointTable[16];
    /* 0x01F0 */ Vec3s morphTable[16];
    /* 0x0250 */ EnAttackNiwActionFunc actionFunc;
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
    /* 0x0280 */ f32 unk_280;
    /* 0x0284 */ f32 unk_284;
    /* 0x0284 */ f32 unk_288;
    /* 0x028C */ s16 unk_28C;
    /* 0x028E */ s16 unk_28E;
    /* 0x0290 */ char unk_290[0x2];
    /* 0x0292 */ s16 unk_292;
    /* 0x0294 */ char unk_294[0x2];
    /* 0x0296 */ s16 unk_296;
    /* 0x0298 */ Vec3f unk_298;
    /* 0x02A4 */ Vec3f unk_2A4;
    /* 0x02B0 */ Vec3f unk_2B0;
    /* 0x02BC */ f32 unk_2BC;
    /* 0x02C0 */ f32 unk_2C0;
    /* 0x02C4 */ char unk_2C4[0xC];
    /* 0x02D0 */ f32 unk_2D0;
    /* 0x02D4 */ f32 unk_2D4;
    /* 0x02D8 */ char unk_2D8[0x4];
    /* 0x02DC */ f32 unk_2DC;
    /* 0x02E0 */ f32 unk_2E0;
    /* 0x02E4 */ f32 unk_2E4;
} EnAttackNiw; // size = 0x02E8

#endif
