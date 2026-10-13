#ifndef Z_EN_AL_H
#define Z_EN_AL_H

#include "global.h"
#include "objects/object_al/object_al.h"

struct EnAl;

typedef void (*EnAlActionFunc)(struct EnAl*, PlayState*);
typedef void (*EnAlUnkFunc2)(struct EnAl*, PlayState*);

typedef struct EnAl {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ EnAlActionFunc actionFunc;
    /* 0x18C */ EnAlUnkFunc2 unk_18C;
    /* 0x190 */ MtxF unk_190[6];
    /* 0x310 */ ColliderCylinder unk_310;
    /* 0x35C */ u8 scheduleResult;
    /* 0x360 */ MsgScript* msgScript;
    /* 0x364 */ s32 msgScriptPos;
    /* 0x368 */ Actor* unk_368;
    /* 0x36C */ Vec3f unk_36C;
    /* 0x378 */ Vec3s unk_378;
    /* 0x37E */ Vec3s jointTable[MADAME_AROMA_LIMB_MAX];
    /* 0x420 */ Vec3s morphTable[MADAME_AROMA_LIMB_MAX];
    /* 0x4C2 */ u16 unk_4C2;
    /* 0x4C4 */ u16 unk_4C4;
    /* 0x4C8 */ f32 animPlaySpeed;
    /* 0x4CC */ UNK_TYPE1 unk4CC[0x8];
    /* 0x4D4 */ f32 unk_4D4;
    /* 0x4D8 */ UNK_TYPE1 unk4D8[0x4];
    /* 0x4DC */ s16 unk_4DC;
    /* 0x4DE */ s16 unk_4DE;
    /* 0x4E0 */ s16 unk_4E0;
    /* 0x4E2 */ s16 unk_4E2;
    /* 0x4E4 */ s16 unk_4E4;
    /* 0x4E6 */ s16 unk_4E6;
    /* 0x4E8 */ s16 unk_4E8;
    /* 0x4EA */ s16 unk_4EA;
    /* 0x4EC */ MsgScriptCallback msgScriptCallback;
    /* 0x4F0 */ s32 unk_4F0;
    /* 0x4F4 */ s32 unk_4F4;
    /* 0x4F8 */ s32 animIndex;
    /* 0x4FC */ UNK_TYPE1 unk4FC[0x4];
} EnAl; // size = 0x500

#endif // Z_EN_AL_H
