#ifndef Z_EN_KZ_H
#define Z_EN_KZ_H

#include "ultra64.h"
#include "global.h"

struct EnKz;

typedef void (*EnKzActionFunc)(struct EnKz*, PlayState*);

typedef struct EnKz {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelanime;
    /* 0x0190 */ EnKzActionFunc actionFunc;
    /* 0x0194 */ ColliderCylinder collider;
    /* 0x01E0 */ struct_80034A14_arg1 unk_1E0;
    /* 0x0208 */ u8 sfxPlayed;
    /* 0x0209 */ u8 isTrading;
    /* 0x020A */ s16 waypoint;
    /* 0x020C */ s16 blinkTimer;
    /* 0x020E */ char unk_20E[2];
    /* 0x0210 */ s16 eyeIdx;
    /* 0x0212 */ s16 cutsceneCamera;
    /* 0x0214 */ s16 gameplayCamera;
    /* 0x0216 */ Vec3s jointTable[12];
    /* 0x025E */ Vec3s morphTable[12];
    /* 0x02A6 */ s16 unk_2A6[12];
    /* 0x02BE */ s16 unk_2BE[12];
} EnKz; // size = 0x02D8

#endif
