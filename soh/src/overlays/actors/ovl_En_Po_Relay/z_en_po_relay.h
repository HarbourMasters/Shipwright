#ifndef Z_EN_PO_RELAY_H
#define Z_EN_PO_RELAY_H

#include "ultra64.h"
#include "global.h"

struct EnPoRelay;

typedef void (*EnPoRelayActionFunc)(struct EnPoRelay*, PlayState*);

typedef struct EnPoRelay {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;
    /* 0x0190 */ EnPoRelayActionFunc actionFunc;
    /* 0x0194 */ u8 hookshotSlotFull;
    /* 0x0195 */ u8 unk_195;
    /* 0x0196 */ s16 actionTimer;
    /* 0x0198 */ s16 pathIndex;
    /* 0x019A */ s16 unk_19A;
    /* 0x019C */ u16 textId;
    /* 0x019E */ u16 eyeTextureIdx;
    /* 0x01A0 */ Vec3s jointTable[18];
    /* 0x020C */ Vec3s morphTable[18];
    /* 0x0278 */ Color_RGBA8 lightColor;
    /* 0x027C */ LightNode* lightNode;
    /* 0x0280 */ LightInfo lightInfo;
    /* 0x0290 */ ColliderCylinder collider;
} EnPoRelay; // size = 0x02DC

#endif
