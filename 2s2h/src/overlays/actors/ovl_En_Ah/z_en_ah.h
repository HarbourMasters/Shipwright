#ifndef Z_EN_AH_H
#define Z_EN_AH_H

#include "global.h"
#include "objects/object_ah/object_ah.h"

struct EnAh;

typedef void (*EnAhActionFunc)(struct EnAh*, PlayState*);
typedef void (*EnAhUnkFunc)(struct EnAh*, PlayState*);

typedef struct EnAh {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ EnAhActionFunc actionFunc;
    /* 0x18C */ EnAhUnkFunc unk_18C;
    /* 0x190 */ ColliderCylinder collider;
    /* 0x1DC */ u8 scheduleResult;
    /* 0x1E0 */ s32 msgScriptPos;
    /* 0x1E4 */ Actor* unk_1E4;
    /* 0x1E8 */ Vec3f unk_1E8[2];
    /* 0x200 */ Vec3s unk_200[2];
    /* 0x20C */ Vec3s jointTable[OBJECT_AH_LIMB_MAX];
    /* 0x272 */ Vec3s morphTable[OBJECT_AH_LIMB_MAX];
    /* 0x2D8 */ u16 unk_2D8;
    /* 0x2DA */ u16 unk_2DA;
    /* 0x2DC */ f32 animPlaySpeed;
    /* 0x2E0 */ UNK_TYPE1 unk2E0[0xC];
    /* 0x2EC */ s16 unk_2EC;
    /* 0x2EE */ s16 unk_2EE;
    /* 0x2F0 */ s16 unk_2F0;
    /* 0x2F2 */ s16 unk_2F2;
    /* 0x2F4 */ s16 unk_2F4;
    /* 0x2F6 */ s16 unk_2F6;
    /* 0x2F8 */ s16 unk_2F8;
    /* 0x2FA */ s16 unk_2FA;
    /* 0x2FC */ s16 unk_2FC;
    /* 0x2FE */ s16 unk_2FE;
    /* 0x300 */ s32 animIndex;
} EnAh; // size = 0x304

#endif // Z_EN_AH_H
