#ifndef Z_EN_DNT_NOMAL_H
#define Z_EN_DNT_NOMAL_H

#include "ultra64.h"
#include "global.h"

struct EnDntNomal;

typedef void (*EnDntNomalActionFunc)(struct EnDntNomal*, PlayState*);

typedef struct EnDntNomal {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;
    /* 0x0190 */ Vec3s jointTable[11];
    /* 0x01D2 */ Vec3s morphTable[11];
    /* 0x0214 */ EnDntNomalActionFunc actionFunc;
    /* 0x0218 */ u8 isSolid;
    /* 0x0219 */ Vec3f flowerPos;
    /* 0x0228 */ Vec3f targetVtx[4];
    /* 0x0258 */ s16 timer1;
    /* 0x025A */ s16 timer2;
    /* 0x025C */ s16 timer4;
    /* 0x025E */ s16 timer5;
    /* 0x0260 */ s16 blinkTimer;
    /* 0x0262 */ s16 unkCounter;
    /* 0x0264 */ s16 timer3;
    /* 0x0266 */ s16 objId;
    /* 0x0268 */ s16 eyeState;
    /* 0x026A */ s16 type;
    /* 0x026C */ s16 hitCounter;
    /* 0x026E */ s16 endFrame;
    /* 0x0270 */ s16 stageSignal;
    /* 0x0272 */ s16 rotDirection;
    /* 0x0274 */ s16 action;
    /* 0x0276 */ u8 ignore;
    /* 0x0277 */ u8 spawnedItem;
    /* 0x0278 */ u8 stagePrize;
    /* 0x0279 */ s8 objIndex;
    /* 0x027C */ Vec3f mouthPos;
    /* 0x0288 */ Vec3f targetPos;
    /* 0x0294 */ ColliderQuad targetQuad;
    /* 0x0314 */ ColliderCylinder bodyCyl;
} EnDntNomal; // size = 0x0360

#define ENDNTNOMAL_TARGET 0
#define ENDNTNOMAL_STAGE 1

#endif
