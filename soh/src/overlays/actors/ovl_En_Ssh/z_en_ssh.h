#ifndef Z_EN_SSH_H
#define Z_EN_SSH_H

#include "ultra64.h"
#include "global.h"

struct EnSsh;

typedef void (*EnSshActionFunc)(struct EnSsh*, PlayState*);

typedef struct EnSsh {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;
    /* 0x0190 */ Vec3s jointTable[30];
    /* 0x0244 */ Vec3s morphTable[30];
    /* 0x02F8 */ EnSshActionFunc actionFunc;
    /* 0x02FC */ ColliderCylinder colCylinder[6];
    /* 0x04C4 */ ColliderJntSph colSph;
    /* 0x04E4 */ ColliderJntSphElement colSphElements[1];
    /* 0x0524 */ s16 initialYaw;
    /* 0x0526 */ s16 maxTurnRate;
    /* 0x0528 */ s16 unkTimer;
    /* 0x052A */ s16 spinTimer;
    /* 0x052C */ s16 hitTimer;
    /* 0x052E */ s16 invincibilityTimer;
    /* 0x0530 */ s16 sfxTimer;
    /* 0x0532 */ s16 stunTimer;
    /* 0x0534 */ s16 animTimer;
    /* 0x0536 */ s16 swayTimer;
    /* 0x0538 */ s32 blureIdx;
    /* 0x053C */ f32 colliderScale;
    /* 0x0540 */ f32 floorHeightOffset;
    /* 0x0544 */ Vec3f ceilingPos;
    /* 0x0558 */ char unk_558[0x78];
    /* 0x05C8 */ s16 swayAngle;
    /* 0x05CA */ u16 stateFlags;
    /* 0x05CC */ u8 hitCount;
    /* 0x05CE */ s16 blinkState;
    /* 0x05D0 */ s16 blinkTimer;
} EnSsh; // size = 0x05D4

#define ENSSH_FATHER 0

#endif
