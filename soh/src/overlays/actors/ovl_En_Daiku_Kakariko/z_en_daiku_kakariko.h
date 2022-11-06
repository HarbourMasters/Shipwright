#ifndef Z_EN_DAIKU_KAKARIKO_H
#define Z_EN_DAIKU_KAKARIKO_H

#include "ultra64.h"
#include "global.h"

struct EnDaikuKakariko;

typedef void (*EnDaikuKakarikoActionFunc)(struct EnDaikuKakariko*, PlayState*);

typedef struct EnDaikuKakariko {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;
    /* 0x0190 */ EnDaikuKakarikoActionFunc actionFunc;
    /* 0x0194 */ ColliderCylinder collider;
    /* 0x01E0 */ s32 talkState;
    /* 0x01E4 */ s32 waypoint;
    /* 0x01E8 */ f32 runSpeed;
    /* 0x01EC */ s32 currentAnimIndex;
    /* 0x01F0 */ char unk_1F0[8];  // Unused variables?
    /* 0x01F8 */ s32 pathContinue; // If true, continue looping through the path data
    /* 0x01FC */ s32 run;          // If true the carpenter will run
    /* 0x0200 */ u16 flags;
    /* 0x0202 */ u16 runFlag;
    /* 0x0204 */ struct_80034A14_arg1 npcInfo; // Info related to NPCs and turning their head towards the player
    /* 0x022C */ Vec3s jointTable[17];
    /* 0x0292 */ Vec3s morphTable[17];
    /* 0x02F8 */ s32 timer;
    /* 0x02FC */ Vec3s neckAngle;
    /* 0x0302 */ Vec3s neckAngleTarget;
} EnDaikuKakariko; // size = 0x0308

#endif
