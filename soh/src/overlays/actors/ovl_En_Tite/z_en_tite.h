#ifndef Z_EN_TITE_H
#define Z_EN_TITE_H

#include "ultra64.h"
#include "global.h"

struct EnTite;

typedef void (*EnTiteActionFunc)(struct EnTite*, PlayState*);

typedef enum {
    /* -2 */ TEKTITE_BLUE = -2,
    /* -1 */ TEKTITE_RED
} EnTiteType;

typedef struct EnTite {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;
    /* 0x0190 */ Vec3s jointTable[25];
    /* 0x0226 */ Vec3s morphTable[25];
    /* 0x02BC */ u8 action;
    /* 0x02BD */ u8 flipState;
    /* 0x02C0 */ EnTiteActionFunc actionFunc;
    /* 0x02C4 */ BodyBreak bodyBreak;
    /* 0x02DC */ s32 unk_2DC; // flags related to bgCheck drawn effects
    /* 0x02E0 */ s16 actionVar1; // Usage depends on current action function
    /* 0x02E2 */ u8 actionVar2; // Usage depends on current action function
    /* 0x02E3 */ u8 spawnIceTimer;
    /* 0x02E4 */ u8 damageEffect;
    /* 0x02E8 */ ColliderJntSph collider;
    /* 0x0308 */ ColliderJntSphElement colliderItem;
    /* 0x0348 */ Vec3f frontLeftFootPos;
    /* 0x0354 */ Vec3f frontRightFootPos;
    /* 0x0360 */ Vec3f backRightFootPos;
    /* 0x036C */ Vec3f backLeftFootPos;
} EnTite; // size = 0x0378

#endif
