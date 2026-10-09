#ifndef Z_EN_SHN_H
#define Z_EN_SHN_H

#include "global.h"
#include "objects/object_shn/object_shn.h"

struct EnShn;

typedef void (*EnShnActionFunc)(struct EnShn*, PlayState*);

typedef struct EnShn {
    /* 0x000 */ Actor actor;
    /* 0x144 */ EnShnActionFunc actionFunc;
    /* 0x148 */ SkelAnime skelAnime;
    /* 0x18C */ UNK_TYPE1 pad_18C[0x4C];
    /* 0x1D8 */ u16 unk_1D8;
    /* 0x1DA */ u16 unk_1DA;
    /* 0x1DC */ MsgScript* msgScript;
    /* 0x1E0 */ s32 msgScriptPos;
    /* 0x1E4 */ Player* shnPlayerRef;
    /* 0x1E8 */ Vec3f unk_1E8;
    /* 0x1F4 */ Vec3s unk_1F4;
    /* 0x1FA */ Vec3s jointTable[BURLY_GUY_LIMB_MAX];
    /* 0x25A */ Vec3s morphTable[BURLY_GUY_LIMB_MAX];
    /* 0x2BA */ s16 unk_2BA;
    /* 0x2BC */ s16 unk_2BC;
    /* 0x2BE */ s16 unk_2BE;
    /* 0x2C0 */ s16 unk_2C0;
    /* 0x2C2 */ s16 unk_2C2;
    /* 0x2C4 */ s16 unk_2C4;
    /* 0x2C6 */ s16 unk_2C6;
    /* 0x2C8 */ s16 unk_2C8;
    /* 0x2CA */ s16 unk_2CA;
    /* 0x2CC */ f32 animPlaySpeed;
    /* 0x2D0 */ f32 unk_2D0;
    /* 0x2D4 */ f32 unk_2D4;
    /* 0x2D8 */ MsgScriptCallback msgScriptCallback;
    /* 0x2DC */ s32 prevTalkState;
    /* 0x2E0 */ s32 unk_2E0;
    /* 0x2E4 */ s32 unk_2E4;
    /* 0x2E8 */ s32 animIndex;
    /* 0x2EC */ s32 unk_2EC;
} EnShn; // size = 0x2F0

#endif // Z_EN_SHN_H
