#ifndef Z_EN_KO_H
#define Z_EN_KO_H

#include "ultra64.h"
#include "global.h"

struct EnKo;

typedef void (*EnKoActionFunc)(struct EnKo*, PlayState*);

typedef struct EnKo {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;
    /* 0x0190 */ EnKoActionFunc actionFunc;
    /* 0x0194 */ s8 headObjectBankIdx;
    /* 0x0195 */ s8 bodyObjectBankIdx;
    /* 0x0196 */ s8 legsObjectBankIdx;
    /* 0x0197 */ s8 osAnimeBankIndex;
    /* 0x0198 */ ColliderCylinder collider;
    /* 0x01E4 */ Path* path;
    /* 0x01E8 */ struct_80034A14_arg1 unk_1E8;
    /* 0x0210 */ u8 unk_210; // block trade quest sfx
    /* 0x0212 */ s16 forestQuestState;
    /* 0x0214 */ s16 blinkTimer;
    /* 0x0216 */ s16 eyeTextureIndex;
    /* 0x0218 */ f32 appearDist;
    /* 0x021C */ f32 lookDist; // distance to start looking at player
    /* 0x0220 */ f32 modelAlpha;
    /* 0x0224 */ Vec3s jointTable[16];
    /* 0x0284 */ Vec3s morphTable[16];
    /* 0x02E4 */ s16 unk_2E4[16];
    /* 0x0304 */ s16 unk_304[16];
} EnKo; // size = 0x0324

typedef enum {
    ENKO_TYPE_CHILD_0,
    ENKO_TYPE_CHILD_1,
    ENKO_TYPE_CHILD_2,
    ENKO_TYPE_CHILD_3,
    ENKO_TYPE_CHILD_4,
    ENKO_TYPE_CHILD_5, // Shop Awning
    ENKO_TYPE_CHILD_6,
    ENKO_TYPE_CHILD_7,
    ENKO_TYPE_CHILD_8,
    ENKO_TYPE_CHILD_9,
    ENKO_TYPE_CHILD_10,
    ENKO_TYPE_CHILD_11,
    ENKO_TYPE_CHILD_FADO,
    ENKO_TYPE_CHILD_MAX
} KokiriChildren;

typedef enum {
    ENKO_FQS_CHILD_START,
    ENKO_FQS_CHILD_STONE,
    ENKO_FQS_CHILD_SARIA,
    ENKO_FQS_ADULT_ENEMY,
    ENKO_FQS_ADULT_SAVED
} KokiriForestQuestState;

#endif
