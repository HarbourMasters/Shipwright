#ifndef Z_EN_HEISHI4_H
#define Z_EN_HEISHI4_H

#include "ultra64.h"
#include "global.h"

typedef enum {
    /* 0x00 */ HEISHI4_AT_KAKRIKO_ENTRANCE,
    /* 0x04 */ HEISHI4_AT_IMPAS_HOUSE = 4,
    /* 0x07 */ HEISHI4_AT_MARKET_DYING = 7,
    /* 0x08 */ HEISHI4_AT_MARKET_NIGHT
} Heishi4Type;

struct EnHeishi4;

typedef void (*EnHeishi4ActionFunc)(struct EnHeishi4*, PlayState*);

typedef struct EnHeishi4 {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;
    /* 0x0190 */ Vec3s jointTable[17];
    /* 0x01F6 */ Vec3s morphTable[17];
    /* 0x025C */ EnHeishi4ActionFunc actionFunc;
    /* 0x0260 */ Vec3s unk_260;
    /* 0x0266 */ Vec3s unk_266;
    /* 0x026C */ Vec3f pos;
    /* 0x0278 */ f32 height;
    /* 0x027C */ s16 unk_27C;
    /* 0x027E */ s16 unk_27E;
    /* 0x0280 */ s16 type;
    /* 0x0282 */ s16 unk_282;
    /* 0x0284 */ s16 unk_284;
    /* 0x0288 */ f32 unk_288;
    /* 0x028C */ struct_80034A14_arg1 unk_28C;
    /* 0x02B4 */ u8 unk_2B4;
    /* 0x02B6 */ char unk_2B6[7];
    /* 0x02BC */ ColliderCylinder collider;
} EnHeishi4; // size = 0x0308

#endif
