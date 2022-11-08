#ifndef Z_EN_BW_H
#define Z_EN_BW_H

#include "ultra64.h"
#include "global.h"

struct EnBw;
typedef void (*EnBwActionFunc)(struct EnBw*, PlayState*);

typedef struct EnBw {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;
    /* 0x0190 */ Vec3s jointTable[12];
    /* 0x01D8 */ Vec3s morphTable[12];
    /* 0x0220 */ u8 unk_220;
    /* 0x0221 */ u8 unk_221;
    /* 0x0222 */ s16 unk_222;
    /* 0x0224 */ s16 unk_224;
    /* 0x0228 */ EnBwActionFunc actionFunc;
    /* 0x022C */ Color_RGBA8 color1;
    /* 0x0230 */ u8 unk_230;
    /* 0x0231 */ u8 damageEffect;
    /* 0x0232 */ u8 unk_232;
    /* 0x0234 */ s16 unk_234;
    /* 0x0236 */ s16 unk_236;
    /* 0x0238 */ s16 unk_238;
    /* 0x023A */ u8 unk_23A;
    /* 0x022B */ u8 iceTimer;
    /* 0x023C */ u8 unk_23C;
    /* 0x0240 */ f32 unk_240;
    /* 0x0244 */ f32 unk_244;
    /* 0x0248 */ f32 unk_248;
    /* 0x024C */ f32 unk_24C;
    /* 0x0250 */ f32 unk_250;
    /* 0x0254 */ char unk_254[4];
    /* 0x0258 */ f32 unk_258;
    /* 0x025C */ f32 unk_25C;
    /* 0x0260 */ f32 unk_260;
    /* 0x0264 */ Vec3f unk_264;
    /* 0x0270 */ Vec3f unk_270;
    /* 0x027C */ Vec3f unk_27C;
    /* 0x0288 */ Vec3f unk_288;
    /* 0x0294 */ ColliderCylinder collider1;
    /* 0x02E0 */ ColliderCylinder collider2;
} EnBw; // size = 0x032C

#endif
