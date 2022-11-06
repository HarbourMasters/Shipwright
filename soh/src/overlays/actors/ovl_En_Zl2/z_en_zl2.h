#ifndef Z_EN_ZL2_H
#define Z_EN_ZL2_H

#include "ultra64.h"
#include "global.h"

struct EnZl2;

typedef void (*EnZl2ActionFunc)(struct EnZl2*, PlayState*);
typedef void (*EnZl2DrawFunc)(struct EnZl2*, PlayState*);

typedef struct EnZl2 {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;
    /* 0x0190 */ s16 eyeTexIndex;
    /* 0x0192 */ s16 blinkTimer;
    /* 0x0194 */ s16 eyeTexIndex2;
    /* 0x0196 */ char unk_196[0x2];
    /* 0x0198 */ s16 mouthTexIndex;
    /* 0x019A */ char unk_19A[0x2];
    /* 0x019C */ s32 action;
    /* 0x01A0 */ s32 drawConfig;
    /* 0x01A4 */ char unk_1A4[0x4];
    /* 0x01A8 */ s32 alpha;
    /* 0x01AC */ s16 unk_1AC[0x18]; // ???
    /* 0x01DC */ s16 unk_1DC[0x18]; // ???
    /* 0x020C */ s16 unk_20C[0x18]; // ???
    /* 0x023C */ f32 unk_23C;
    /* 0x0240 */ s32 unk_240;
    /* 0x0244 */ s32 unk_244;
    /* 0x0248 */ s32 unk_248;
    /* 0x024C */ s32 unk_24C;
    /* 0x0250 */ s32 unk_250;
    /* 0x0254 */ s32 unk_254;
    /* 0x0258 */ char unk_258[0xC];
    /* 0x0264 */ s32 overrideLimbDrawConfig;
    /* 0x0268 */ char unk_268[0xC];
    /* 0x0274 */ s32 unk_274;
    /* 0x0278 */ f32 unk_278;
    /* 0x027C */ f32 unk_27C;
} EnZl2; // size = 0x0280

#endif
