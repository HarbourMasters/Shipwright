#ifndef Z_BOSS_GANON2_H
#define Z_BOSS_GANON2_H

#include "ultra64.h"
#include "global.h"


typedef struct {
    /* 0x00 */ u8 type;
    /* 0x01 */ u8 unk_01;
    /* 0x04 */ Vec3f position;
    /* 0x10 */ Vec3f velocity;
    /* 0x1C */ Vec3f accel;
    /* 0x28 */ char unk_28[0x6];
    /* 0x2E */ s16 unk_2E;
    /* 0x30 */ char unk_30[0x4];
    /* 0x34 */ f32 scale;
    /* 0x38 */ Vec3f unk_38;
} BossGanon2Effect; // size = 0x44

struct BossGanon2;

typedef void (*BossGanon2ActionFunc)(struct BossGanon2*, PlayState*);

typedef struct BossGanon2 {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;
    /* 0x0190 */ BossGanon2ActionFunc actionFunc;
    /* 0x0194 */ f32 unk_194;
    /* 0x0198 */ f32 unk_198;
    /* 0x019C */ s16 unk_19C;
    /* 0x019E */ char unk_19E[0x4];
    /* 0x01A2 */ s16 unk_1A2[5];
    /* 0x01AC */ s16 unk_1AC;
    /* 0x01B0 */ f32 unk_1B0;
    /* 0x01B4 */ f32 unk_1B4;
    /* 0x01B8 */ Vec3f unk_1B8;
    /* 0x01C4 */ Vec3f unk_1C4;
    /* 0x01D0 */ Vec3f unk_1D0;
    /* 0x01D0 */ Vec3f unk_1DC;
    /* 0x01E8 */ Vec3f unk_1E8;
    /* 0x01F4 */ Vec3f unk_1F4;
    /* 0x0200 */ Vec3f unk_200;
    /* 0x020C */ Vec3f unk_20C;
    /* 0x0218 */ Vec3f unk_218;
    /* 0x0224 */ f32 unk_224;
    /* 0x0228 */ f32 unk_228;
    /* 0x022C */ char unk_22C[0x8];
    /* 0x0234 */ Vec3f unk_234[16];
    /* 0x02F4 */ s16 unk_2F4[5];
    /* 0x02FE */ s16 unk_2FE[5];
    /* 0x0308 */ char unk_308[4];
    /* 0x030C */ f32 unk_30C;
    /* 0x0310 */ u8 unk_310;
    /* 0x0311 */ u8 unk_311;
    /* 0x0312 */ u8 unk_312;
    /* 0x0313 */ u8 unk_313; // "look_on"
    /* 0x0314 */ u8 unk_314;
    /* 0x0316 */ s16 unk_316; // "no_hit_time"
    /* 0x0318 */ s16 unk_318;
    /* 0x031A */ s16 unk_31A;
    /* 0x031C */ s16 unk_31C;
    /* 0x0320 */ f32 unk_320;
    /* 0x0324 */ f32 unk_324;
    /* 0x0328 */ s16 unk_328;
    /* 0x032C */ f32 unk_32C;
    /* 0x0330 */ s16 unk_330;
    /* 0x0332 */ s16 unk_332;
    /* 0x0334 */ u8 unk_334;
    /* 0x0335 */ u8 unk_335;
    /* 0x0336 */ u8 unk_336;
    /* 0x0337 */ u8 unk_337;
    /* 0x0338 */ u8 unk_338;
    /* 0x0339 */ s8 unk_339;
    /* 0x033C */ f32 unk_33C;
    /* 0x0340 */ s16 unk_340;
    /* 0x0342 */ s16 unk_342;
    /* 0x0344 */ s16 unk_344;
    /* 0x0346 */ s16 unk_346;
    /* 0x0348 */ s16 unk_348[5];
    /* 0x0352 */ s16 unk_352[5];
    /* 0x035C */ f32 unk_35C;
    /* 0x0360 */ Vec3f unk_360;
    /* 0x036C */ f32 unk_36C;
    /* 0x0370 */ Vec3f unk_370;
    /* 0x037C */ f32 unk_37C;
    /* 0x0380 */ f32 unk_380;
    /* 0x0384 */ f32 unk_384;
    /* 0x0388 */ f32 unk_388;
    /* 0x038C */ f32 unk_38C;
    /* 0x0390 */ s16 unk_390;
    /* 0x0392 */ s16 unk_392;
    /* 0x0394 */ f32 unk_394;
    /* 0x0398 */ u32 unk_398;
    /* 0x039C */ s16 unk_39C;
    /* 0x039E */ s16 unk_39E;
    /* 0x03A0 */ char unk_3A0[0x4];
    /* 0x03A4 */ Vec3f unk_3A4;
    /* 0x03B0 */ Vec3f unk_3B0;
    /* 0x03BC */ Vec3f unk_3BC;
    /* 0x03C8 */ char unk_3C8[0x48];
    /* 0x0410 */ Vec3f unk_410;
    /* 0x041C */ f32 unk_41C;
    /* 0x0420 */ f32 unk_420;
    /* 0x0424 */ ColliderJntSph unk_424;
    /* 0x0444 */ ColliderJntSph unk_444;
    /* 0x0464 */ ColliderJntSphElement unk_464[16];
    /* 0x0864 */ ColliderJntSphElement unk_864[2];
} BossGanon2; // size = 0x08E4

#endif
