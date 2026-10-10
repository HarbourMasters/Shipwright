#ifndef Z_EN_MA_YTO_H
#define Z_EN_MA_YTO_H

#include "global.h"
#include "objects/object_ma2/object_ma2.h"

struct EnMaYto;

typedef void (*EnMaYtoActionFunc)(struct EnMaYto*, PlayState*);

typedef struct EnMaYto {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ EnMaYtoActionFunc actionFunc;
    /* 0x18C */ ColliderCylinder collider;
    /* 0x1D8 */ NpcInteractInfo interactInfo;
    /* 0x200 */ s32 unk200; // unused
    /* 0x204 */ s32 type;
    /* 0x208 */ Vec3s jointTable[CREMIA_LIMB_MAX];
    /* 0x28C */ Vec3s morphTable[CREMIA_LIMB_MAX];
    /* 0x310 */ s16 unk310; // state?
    /* 0x312 */ UNK_TYPE1 unk_312[0x2];
    /* 0x314 */ s16 animIndex; // Used only in DefaultWait
    /* 0x316 */ s16 overrideEyeTexIndex; // If non-zero, then this index will be used instead of eyeTexIndex
    /* 0x318 */ s16 mouthTexIndex;
    /* 0x31A */ s16 eyeTexIndex;
    /* 0x31C */ s16 blinkTimer;
    /* 0x31E */ s16 unk31E;
    /* 0x320 */ s16 unk320;
    /* 0x322 */ u16 textId;
} EnMaYto; // size = 0x324

typedef enum {
    /* 0 */ MA_YTO_TYPE_DEFAULT, // Strokes cow on day 1
    /* 1 */ MA_YTO_TYPE_BARN,
    /* 2 */ MA_YTO_TYPE_DINNER,
    /* 3 */ MA_YTO_TYPE_AFTERMILKRUN, // After Milk Run minigame
    /* 4 */ MA_YTO_TYPE_4  // HugCutscene? Doesn't seem to work properly in-game
} EnMaYtoType;

#define EN_MA_YTO_GET_TYPE(thisx) (((thisx)->params & 0xF000) >> 12)
#define EN_MA_YTO_PARAM(enMaYtsType, arg1) (((enMaYtsType) << 12) | ((arg1) << 8))

#endif // Z_EN_MA_YTO_H
