#ifndef Z_EN_HEISHI3_H
#define Z_EN_HEISHI3_H

#include "ultra64.h"
#include "global.h"

struct EnHeishi3;

typedef void (*EnHeishi3ActionFunc)(struct EnHeishi3*, PlayState*);

typedef struct EnHeishi3 {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;
    /* 0x0190 */ Vec3s jointTable[17];
    /* 0x01F6 */ Vec3s morphTable[17];
    /* 0x025C */ EnHeishi3ActionFunc actionFunc;
    /* 0x0260 */ char unk_260[0x2];
    /* 0x0262 */ s16 unk_262;
    /* 0x0264 */ s16 unk_264;
    /* 0x0266 */ char unk_266[0x8];
    /* 0x026E */ s16 unk_26E; // All these s16 are probably part of a Vec3s
    /* 0x0270 */ char unk_270[0x2];
    /* 0x0272 */ s16 caughtTimer;
    /* 0x0274 */ s16 unk_274;
    /* 0x0276 */ u8 respawnFlag; // set to 1 when reloading area after being thrown out
    /* 0x0278 */ s16 unk_278;
    /* 0x027A */ char unk_27A[0x2];
    /* 0x027C */ ColliderCylinder collider;
} EnHeishi3; // size = 0x02C8

#endif
