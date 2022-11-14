#ifndef Z_EN_DHA_H
#define Z_EN_DHA_H

#include "ultra64.h"
#include "global.h"

struct EnDha;

typedef void (*EnDhaActionFunc)(struct EnDha*, PlayState*);

typedef struct EnDha {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;
    /* 0x0190 */ Vec3s jointTable[4];
    /* 0x01A8 */ Vec3s morphTable[4];
    /* 0x01C0 */ u8 unk_1C0;
    /* 0x01C4 */ EnDhaActionFunc actionFunc;
    /* 0x01C8 */ s16 actionTimer;
    /* 0x01CA */ s16 timer;
    /* 0x01CC */ u8 unk_1CC;
    /* 0x01CE */ s16 limbAngleX[2];
    /* 0x01D2 */ s16 limbAngleY;
    /* 0x01D4 */ Vec3s handAngle;
    /* 0x01DC */ Vec3f handPos[2];
    /* 0x01F4 */ Vec3f armPos;
    /* 0x0200 */ ColliderJntSph collider;
    /* 0x0220 */ ColliderJntSphElement colliderItem[5];
} EnDha; // size = 0x0360

#endif
