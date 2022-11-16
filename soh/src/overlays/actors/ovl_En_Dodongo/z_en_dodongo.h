#ifndef Z_EN_DODONGO_H
#define Z_EN_DODONGO_H

#include "ultra64.h"
#include "global.h"

struct EnDodongo;

typedef void (*EnDodongoActionFunc)(struct EnDodongo*, PlayState*);

typedef struct EnDodongo {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;
    /* 0x0190 */ Vec3s jointTable[31];
    /* 0x024A */ Vec3s morphTable[31];
    /* 0x0304 */ s32 actionState;
    /* 0x0308 */ EnDodongoActionFunc actionFunc;
    /* 0x030C */ s16 timer;
    /* 0x030E */ s16 retreatTimer;
    /* 0x0310 */ s16 tailSwipeSpeed;
    /* 0x0312 */ s16 iceTimer;
    /* 0x0314 */ char unk_314[2];
    /* 0x0316 */ s16 rightFootStep;
    /* 0x0318 */ char unk_318[4];
    /* 0x031C */ Vec3f leftFootPos;
    /* 0x0328 */ Vec3f rightFootPos;
    /* 0x0334 */ Vec3f mouthPos;
    /* 0x0334 */ Vec3f headPos;
    /* 0x034C */ Vec3f bodyScale;
    /* 0x0358 */ Vec3f icePos[9];
    /* 0x03C4 */ Color_RGBA8 bombSmokePrimColor;
    /* 0x03C8 */ Color_RGBA8 bombSmokeEnvColor; 
    /* 0x03CC */ u8 damageEffect;
    /* 0x03D0 */ s32 blureIdx;
    /* 0x03D4 */ ColliderQuad colliderAT;
    /* 0x0454 */ ColliderTris colliderHard;
    /* 0x0474 */ ColliderTrisElement trisElements[3];
    /* 0x0588 */ ColliderJntSph colliderBody;
    /* 0x05A8 */ ColliderJntSphElement sphElements[6];
} EnDodongo; // size = 0x0728

typedef enum {
    EN_DODONGO_NORMAL = -1,
    EN_DODONGO_SMOKE_DEATH
} EnDodongoParam;

#endif
