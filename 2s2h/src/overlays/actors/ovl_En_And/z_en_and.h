#ifndef Z_EN_AND_H
#define Z_EN_AND_H

#include "global.h"
#include "objects/object_and/object_and.h"

struct EnAnd;

typedef void (*EnAndActionFunc)(struct EnAnd*, PlayState*);

typedef struct EnAnd {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ EnAndActionFunc actionFunc;
    /* 0x18C */ u8 cueId;
    /* 0x18D */ UNK_TYPE1 pad18D[0x7];
    /* 0x194 */ Vec3f headComputedPos;
    /* 0x1A0 */ Vec3f torsoComputedPos;
    /* 0x1AC */ Vec3s headComputedRot;
    /* 0x1B2 */ Vec3s torsoComputedRot;
    /* 0x1B8 */ Vec3s jointTable[OBJECT_AND_LIMB_MAX];
    /* 0x254 */ Vec3s morphTable[OBJECT_AND_LIMB_MAX];
    /* 0x2F0 */ u16 flags;
    /* 0x2F2 */ UNK_TYPE1 pad2F2[0x4];
    /* 0x2F6 */ s16 headRotZ;
    /* 0x2F8 */ s16 headRotY;
    /* 0x2FA */ s16 torsoRotZ;
    /* 0x2FC */ s16 torsoRotY;
    /* 0x2FE */ UNK_TYPE1 pad2FE[0x2];
    /* 0x300 */ s16 blinkTimer;
    /* 0x302 */ s16 eyeTexIndex;
    /* 0x304 */ s32 animIndex;
    /* 0x308 */ s32 prevAnimIndex;
    /* 0x30C */ s32 isCutscenePlaying;
    /* 0x310 */ UNK_TYPE1 pad310[4];
} EnAnd; // size = 0x314

#endif // Z_EN_AND_H
