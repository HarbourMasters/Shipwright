#ifndef Z_EN_ST_H
#define Z_EN_ST_H

#include "ultra64.h"
#include "global.h"

struct EnSt;

typedef void (*EnStActionFunc)(struct EnSt* this, PlayState* play);

typedef struct EnSt {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;
    /* 0x0190 */ EnStActionFunc actionFunc;
    /* 0x0194 */ ColliderCylinder colCylinder[6];
    /* 0x035C */ ColliderJntSph colSph;
    /* 0x037C */ ColliderJntSphElement colSphItems[1];
    /* 0x03BC */ s16 initalYaw;
    /* 0x03BE */ s16 deathYawTarget;
    /* 0x03C0 */ s16 groundBounces;
    /* 0x03C2 */ s16 animFrames;
    /* 0x03C4 */ s16 swayTimer;
    /* 0x03C6 */ s16 setTargetYawTimer;
    /* 0x03C8 */ s16 rotAwayTimer;
    /* 0x03CA */ s16 rotTowardsTimer;
    /* 0x03CC */ s16 takeDamageSpinTimer;
    /* 0x03CE */ s16 stunTimer;
    /* 0x03D0 */ s16 invulnerableTimer;
    /* 0x03D2 */ s16 sfxTimer;
    /* 0x03D4 */ s16 gaveDamageSpinTimer;
    /* 0x03D6 */ s16 finishDeathTimer;
    /* 0x03D8 */ s16 deathTimer;
    /* 0x03DA */ s16 absPrevSwayAngle;
    /* 0x03DC */ u8 playSwayFlag;
    /* 0x03DD */ u8 teethR;
    /* 0x03DE */ u8 teethG;
    /* 0x03DF */ u8 teethB;
    /* 0x03DD */ char unk_3E0[4];
    /* 0x03E4 */ Vec3f unusedPos;
    /* 0x03F0 */ Vec3f ceilingPos;
    /* 0x03FC */ char unk_3FC[0x8];
    /* 0x0404 */ s32 blureIdx;
    /* 0x0408 */ f32 colliderScale;
    /* 0x040C */ f32 floorHeightOffset;
    /* 0x0410 */ s16 swayAngle;
    /* 0x0412 */ Vec3s jointTable[30];
    /* 0x04C6 */ Vec3s morphTable[30];
} EnSt; // size = 0x057C

#endif
