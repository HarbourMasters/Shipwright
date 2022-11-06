#ifndef Z_EN_NIW_GIRL_H
#define Z_EN_NIW_GIRL_H

#include "ultra64.h"
#include "global.h"
#include "../ovl_En_Niw/z_en_niw.h"

struct EnNiwGirl;

typedef void (*EnNiwGirlActionFunc)(struct EnNiwGirl*, PlayState*);

typedef struct EnNiwGirl {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;
    /* 0x0190 */ Vec3s jointTable[17];
    /* 0x01F6 */ Vec3s morphTable[17];
    /* 0x025C */ EnNiwGirlActionFunc actionFunc;
    /* 0x0260 */ Vec3s unk_260;
    /* 0x0266 */ Vec3s unk_266;
    /* 0x026C */ s16 jumpTimer; // Controls how many frames she jumps for and how long until she jumps again
    /* 0x026E */ s16 unkUpTimer;
    /* 0x0270 */ s16 unk_270;
    /* 0x0272 */ s16 eyeIndex;
    /* 0x0274 */ s16 blinkTimer;
    /* 0x0276 */ s16 path;
    /* 0x0278 */ s16 unk_278;
    /* 0x027A */ s16 unk_27A;
    /* 0x027C */ f32 unk_27C;
    /* 0x0280 */ f32 unk_280;
    /* 0x0284 */ EnNiw* chasedEnNiw;
    /* 0x0288 */ ColliderCylinder collider;
    /* 0x02D4 */ struct_80034A14_arg1 unk_2D4;
} EnNiwGirl; // size = 0x02FC

#endif
