#ifndef Z_EN_TRU_MT_H
#define Z_EN_TRU_MT_H

#include "global.h"
#include "objects/object_tru/object_tru.h"

struct EnTruMt;

typedef void (*EnTruMtActionFunc)(struct EnTruMt*, PlayState*);

#define ENTRUMT_GET_FF(thisx) ((thisx)->params & 0xFF)
#define ENTRUMT_GET_FC00(thisx) (((thisx)->params & 0xFC00) >> 0xA)

#define ENTRUMT_PATH_INDEX_NONE 0x3F

typedef struct EnTruMt {
    /* 0x000 */ Actor actor;
    /* 0x144 */ EnTruMtActionFunc actionFunc;
    /* 0x148 */ SkelAnime skelAnime;
    /* 0x18C */ ColliderSphere collider;
    /* 0x1E4 */ Vec3s jointTable[KOUME_LIMB_MAX];
    /* 0x286 */ Vec3s morphTable[KOUME_LIMB_MAX];
    /* 0x328 */ u16 unk_328;
    /* 0x32A */ UNK_TYPE1 unk32A[0xC];
    /* 0x336 */ Vec3s unk_336;
    /* 0x33C */ Vec3f unk_33C;
    /* 0x348 */ s16 unk_348;
    /* 0x34A */ s16 unk_34A;
    /* 0x34C */ s16 eyeTexIndex;
    /* 0x34E */ s16 blinkTimer;
    /* 0x350 */ Vec3f unk_350;
    /* 0x35C */ Vec3f unk_35C;
    /* 0x368 */ Path* path;
    /* 0x36C */ s32 unk_36C;
    /* 0x370 */ Vec3f unk_370;
    /* 0x37C */ UNK_TYPE1 unk37C[0xC];
    /* 0x388 */ s16 unk_388;
    /* 0x38A */ s16 unk_38A;
    /* 0x38C */ s16 unk_38C;
    /* 0x38E */ Vec3s unk_38E;
    /* 0x394 */ s16 unk_394;
    /* 0x398 */ Vec3f unk_398;
    /* 0x3A4 */ u8 unk_3A4;
} EnTruMt; // size = 0x3A8

#endif // Z_EN_TRU_MT_H
