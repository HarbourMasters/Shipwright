#ifndef Z_EN_BOMBERS2_H
#define Z_EN_BOMBERS2_H

#include "global.h"
#include "objects/object_cs/object_cs.h"

struct EnBombers2;

typedef void (*EnBombers2ActionFunc)(struct EnBombers2*, PlayState*);

typedef struct EnBombers2 {
    /* 0x000 */ Actor actor;
    /* 0x14C */ SkelAnime skelAnime;
    /* 0x188 */ Vec3s jointTable[OBJECT_CS_LIMB_MAX];
    /* 0x206 */ Vec3s morphTable[OBJECT_CS_LIMB_MAX];
    /* 0x284 */ EnBombers2ActionFunc actionFunc;
    /* 0x288 */ s16 unk_288;
    /* 0x28A */ s16 unk_28A;
    /* 0x28C */ UNK_TYPE1 pad_28C[2];
    /* 0x28E */ s16 unk_28E;
    /* 0x290 */ s16 unk_290;
    /* 0x292 */ UNK_TYPE1 pad_292[4];
    /* 0x296 */ s16 unk_296;
    /* 0x298 */ UNK_TYPE1 pad_298[4];
    /* 0x29C */ Vec3f unk_29C;
    /* 0x2A8 */ s32 unk_2A8;
    /* 0x2AC */ u8 unk_2AC;
    /* 0x2AE */ s16 animIndex;
    /* 0x2B0 */ s16 csId;
    /* 0x2B2 */ s16 unk_2B2;
    /* 0x2B4 */ s16 unk_2B4;
    /* 0x2B6 */ s16 unk_2B6;
    /* 0x2B8 */ f32 lastAnimFrame;
    /* 0x2BC */ s16 eyeIndex;
    /* 0x2BE */ s16 unk_2BE;
    /* 0x2C0 */ s16 unk_2C0;
    /* 0x2C2 */ s16 textIdIndex;
    /* 0x2C4 */ s16 correctDigitSlots[5];
    /* 0x2CE */ s16 talkState;
    /* 0x2D0 */ ColliderCylinder collider;
} EnBombers2; // size = 0x31C

#endif // Z_EN_BOMBERS2_H
