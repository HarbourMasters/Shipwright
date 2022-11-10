#ifndef Z_EN_DNT_JIJI_H
#define Z_EN_DNT_JIJI_H

#include "ultra64.h"
#include "global.h"

struct EnDntJiji;

typedef void (*EnDntJijiActionFunc)(struct EnDntJiji*, PlayState*);

typedef struct EnDntJiji {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;
    /* 0x0190 */ Vec3s jointTable[13];
    /* 0x01DE */ Vec3s morphTable[13];
    /* 0x022C */ EnDntJijiActionFunc actionFunc;
    /* 0x0230 */ Vec3f flowerPos;
    /* 0x023C */ u8 isSolid;
    /* 0x023E */ s16 action;
    /* 0x0240 */ s16 timer;
    /* 0x0242 */ s16 sfxTimer;
    /* 0x0244 */ s16 blinkTimer;
    /* 0x0246 */ s16 unkTimer;
    /* 0x0248 */ s16 endFrame;
    /* 0x024A */ s16 unburrow;
    /* 0x024C */ s16 eyeState;
    /* 0x024E */ s16 stageSignal;
    /* 0x0250 */ s16 unused; // always set to 5
    /* 0x0252 */ s16 attackFlag; // Is never set
    /* 0x0254 */ s32 getItemId;
    /* 0x0258 */ struct EnDntDemo* stage;
    /* 0x025C */ ColliderCylinder collider;
} EnDntJiji; // size = 0x02A8

#endif
