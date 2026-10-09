#ifndef Z_EN_FISH_H
#define Z_EN_FISH_H

#include "global.h"
#include "objects/gameplay_keep/gameplay_keep.h"

struct EnFish;

typedef void (*EnFishUnkFunc)(Actor*, PlayState*);

typedef enum {
    /* -1 */ ENFISH_MINUS1 = -1,
    /*  0 */ ENFISH_0,
    /*  1 */ ENFISH_1,
    /*  2 */ ENFISH_2
} EnFishParam;

#define FISH_PARAMS(param) (param)

typedef struct EnFish {
    /* 0x000 */ Actor actor;
    /* 0x144 */ ColliderJntSph collider;
    /* 0x164 */ ColliderJntSphElement colliderElements[1];
    /* 0x1A4 */ SkelAnime skelAnime;
    /* 0x1E8 */ Vec3s jointTable[FISH_LIMB_MAX];
    /* 0x212 */ Vec3s morphTable[FISH_LIMB_MAX];
    /* 0x23C */ EnFishUnkFunc unkFunc;
    /* 0x240 */ s16 unk_240;
    /* 0x242 */ s16 unk_242;
    /* 0x244 */ s16 unk_244;
    /* 0x246 */ s16 unk_246;
    /* 0x248 */ s32 updBgCheckInfoFlags;
    /* 0x24C */ f32 unk_24C;
    /* 0x250 */ f32 unk_250;
    /* 0x254 */ f32 unk_254;
    /* 0x258 */ f32 unk_258;
    /* 0x25C */ f32 unk_25C;
    /* 0x260 */ f32 unk_260;
    /* 0x264 */ s16 unk_264;
    /* 0x266 */ s16 unk_266;
    /* 0x268 */ s16 unk_268;
    /* 0x26A */ s16 unk_26A;
    /* 0x26C */ s16 unk_26C;
    /* 0x26E */ s16 unk_26E;
    /* 0x270 */ s16 unk_270;
    /* 0x272 */ s16 unk_272;
    /* 0x274 */ s16 unk_274;
    /* 0x276 */ s8 unk_276;
    /* 0x277 */ u8 unk_277;
    /* 0x278 */ s8 unk_278;
    /* 0x279 */ s8 unk_279;
} EnFish; // size = 0x27C

#endif // Z_EN_FISH_H
