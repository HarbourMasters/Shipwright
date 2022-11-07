#ifndef Z_BOSS_GANON_H
#define Z_BOSS_GANON_H

#include "ultra64.h"
#include "global.h"

struct BossGanon;

typedef void (*BossGanonActionFunc)(struct BossGanon*, PlayState*);

typedef enum {
    /*  0 */ GDF_FWORK_0,
    /*  1 */ GDF_FWORK_1,
    /*  2 */ GDF_CENTER_POS,
    /*  3 */ GDF_TRIFORCE_PRIM_B,
    /*  4 */ GDF_TRIFORCE_PRIM_A,
    /*  5 */ GDF_TRIFORCE_ENV_G,
    /*  6 */ GDF_TRIFORCE_SCALE,
    /*  7 */ GDF_VORTEX_ALPHA,
    /*  8 */ GDF_VORTEX_SCALE,
    /*  9 */ GDF_FWORK_UNUSED_9,
    /* 10 */ GDF_FWORK_MAX
} GanondorfFwork;

typedef enum {
    /* 0 */ GDF_WINDOW_SHATTER_OFF,
    /* 1 */ GDF_WINDOW_SHATTER_PARTIAL,
    /* 2 */ GDF_WINDOW_SHATTER_FULL
} WindowShatterState;

typedef enum {
    /* 0 */ GDF_SHOCK_DORF_YELLOW,
    /* 1 */ GDF_SHOCK_PLAYER_YELLOW,
    /* 2 */ GDF_SHOCK_PLAYER_PURPLE
} GanondorfShockType;

typedef enum {
    /* 0 */ GDF_EFF_NONE,
    /* 1 */ GDF_EFF_SPARKLE,
    /* 2 */ GDF_EFF_LIGHT_RAY,
    /* 3 */ GDF_EFF_SHOCK,
    /* 4 */ GDF_EFF_LIGHTNING,
    /* 5 */ GDF_EFF_IMPACT_DUST_DARK,
    /* 6 */ GDF_EFF_IMPACT_DUST_LIGHT,
    /* 7 */ GDF_EFF_SHOCKWAVE,
    /* 8 */ GDF_EFF_BLACK_DOT,
    /* 9 */ GDF_EFF_WINDOW_SHARD
} GanondorfEffectType;

typedef enum {
    /* 0 */ GDF_TRIFORCE_PLAYER,
    /* 1 */ GDF_TRIFORCE_ZELDA,
    /* 2 */ GDF_TRIFORCE_DORF
} GanondorfTriforceType;

typedef struct {
    /* 0x00 */ u8 type;
    /* 0x01 */ u8 timer;
    /* 0x04 */ Vec3f pos;
    /* 0x10 */ Vec3f velocity;
    /* 0x1C */ Vec3f accel;
    /* 0x28 */ Color_RGB8 color;
    /* 0x2C */ s16 alpha;
    /* 0x2E */ s16 unk_2E;
    /* 0x30 */ s16 unk_30;
    /* 0x34 */ f32 scale;
    /* 0x38 */ f32 unk_38; // scale target mostly, but used for other things
    /* 0x3C */ f32 unk_3C; // mostly z rot
    /* 0x40 */ f32 unk_40;
    /* 0x44 */ f32 unk_44; // mostly x rot
    /* 0x48 */ f32 unk_48; // mostly y rot
    u32 epoch;
} GanondorfEffect;         // size = 0x4C

typedef struct BossGanon {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ s32 animBankIndex;
    /* 0x0150 */ SkelAnime skelAnime;
    /* 0x0194 */ BossGanonActionFunc actionFunc;
    /* 0x0198 */ u8 unk_198;
    /* 0x0199 */ u8 legSwayEnabled;
    /* 0x019A */ u8 unk_19A;
    /* 0x019C */ s16 unk_19C; // timer
    /* 0x019E */ u8 unk_19E;
    /* 0x019F */ u8 unk_19F;
    /* 0x01A0 */ s8 envLightMode;
    /* 0x01A2 */ s16 unk_1A2;
    /* 0x01A4 */ s16 unk_1A4;
    /* 0x01A6 */ s16 unk_1A6;
    /* 0x01A8 */ s16 unk_1A8;
    /* 0x01AA */ s16 unk_1AA;
    /* 0x01AC */ s16 unk_1AC;
    /* 0x01AE */ s16 triforceType;
    /* 0x01B0 */ char unk_1B0[0x6];
    /* 0x01B6 */ s16 timers[5];
    /* 0x01C0 */ u8 startVolley;
    /* 0x01C2 */ s16 unk_1C2;
    /* 0x01C4 */ s16 screenFlashTimer;
    /* 0x01C8 */ f32 fwork[GDF_FWORK_MAX];
    /* 0x01F0 */ Vec3f unk_1F0;
    /* 0x01FC */ Vec3f unk_1FC;
    /* 0x0208 */ Vec3f unk_208;
    /* 0x0214 */ Vec3f unk_214;
    /* 0x0220 */ Vec3f unk_220;
    /* 0x022C */ Vec3f unk_22C;
    /* 0x0238 */ Vec3f unk_238;
    /* 0x0244 */ char unk_244[0x10];
    /* 0x0254 */ f32 handLightBallScale;
    /* 0x0258 */ f32 unk_258;
    /* 0x025C */ u8 unk_25C;
    /* 0x0260 */ Vec3f unk_260; // hand position?
    /* 0x026C */ s16 unk_26C; // timer?
    /* 0x0270 */ f32 unk_270;
    /* 0x0274 */ u8 unk_274;
    /* 0x0278 */ Vec3f unk_278;
    /* 0x0284 */ f32 unk_284; // scale for something
    /* 0x0288 */ f32 unk_288;
    /* 0x028C */ f32 unk_28C; // scale for something
    /* 0x0290 */ f32 unk_290;
    /* 0x0294 */ f32 unk_294[15];
    /* 0x02D0 */ f32 unk_2D0; // scale for something
    /* 0x02D4 */ s16 unk_2D4; // timer
    /* 0x02D8 */ Vec3f triforcePos;
    /* 0x02E4 */ u8 shockGlow;
    /* 0x02E6 */ s16 unk_2E6; // timer
    /* 0x02E8 */ s16 unk_2E8; // timer
    /* 0x02EC */ Vec3f unk_2EC[18]; // body parts pos for ganondorf, used for other things as well 
    /* 0x03C4 */ Vec3f unk_3C4[18];
    /* 0x049C */ f32 unk_49C[18];
    /* 0x04E4 */ s16 unk_4E4[18];
    /* 0x0508 */ f32 unk_508;
    /* 0x050C */ Vec3f legRot;
    /* 0x0518 */ char unk_518[0xF8];
    /* 0x0610 */ ColliderCylinder collider;
    /* 0x065C */ char unk_65C[0x10];
    /* 0x066C */ u8 lensFlareMode;
    /* 0x066E */ s16 lensFlareTimer;
    /* 0x0670 */ f32 lensFlareScale;
    /* 0x0674 */ u32 csTimer;
    /* 0x0678 */ s16 csState;
    /* 0x067A */ s16 csCamIndex;
    /* 0x067C */ char unk_67C[0x4];
    /* 0x0680 */ Vec3f csCamEye;
    /* 0x068C */ Vec3f csCamAt;
    /* 0x0698 */ char unk_698[0xC];
    /* 0x06A4 */ Vec3f csCamEyeMaxStep;
    /* 0x06B0 */ Vec3f csCamAtMaxStep;
    /* 0x06BC */ Vec3f csCamTargetEye;
    /* 0x06C8 */ char unk_6C8[0xC];
    /* 0x06D4 */ Vec3f csCamTargetAt;
    /* 0x06E0 */ char unk_6E0[0xC];
    /* 0x06EC */ f32 csCamMaxStepScale;
    /* 0x06F0 */ f32 csCamMovementScale;
    /* 0x06F4 */ f32 csCamFov;
    /* 0x06F8 */ char unk_6F8[0xC];
    /* 0x0704 */ f32 unk_704;
    /* 0x0708 */ char unk_708[0x4];
    /* 0x070C */ f32 unk_70C;
    /* 0x070C */ f32 unk_710;
    /* 0x0714 */ f32 whiteFillAlpha;
    /* 0x0718 */ s16 organAlpha;
    /* 0x071A */ u8 useOpenHand;
    /* 0x071B */ u8 windowShatterState;
} BossGanon; // size = 0x71C

#endif
