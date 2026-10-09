#ifndef Z_EN_TRU_H
#define Z_EN_TRU_H

#include "global.h"
#include "objects/object_tru/object_tru.h"

struct EnTru;

typedef void (*EnTruActionFunc)(struct EnTru*, PlayState*);

#define ENTRU_GET_PATH_INDEX(thisx) ((thisx)->params & 0xFF)

#define ENTRU_PATH_INDEX_NONE 0xFF

typedef struct {
    /* 0x00 */ u8 unk_00;
    /* 0x01 */ u8 unk_01;
    /* 0x02 */ u8 unk_02;
    /* 0x04 */ Vec3f unk_04;
    /* 0x10 */ Vec3f unk_10;
    /* 0x1C */ Vec3f unk_1C;
    /* 0x28 */ f32 unk_28;
    /* 0x2C */ f32 unk_2C;
} EnTruUnkStruct; // size = 0x30

typedef struct EnTru {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ EnTruActionFunc actionFunc;
    /* 0x18C */ ColliderSphere collider;
    /* 0x1E4 */ Path* path;
    /* 0x1E8 */ s32 msgScriptPos;
    /* 0x1EC */ Vec3f unk_1EC;
    /* 0x1F8 */ Vec3f unk_1F8;
    /* 0x204 */ Vec3s unk_204;
    /* 0x20A */ Vec3s jointTable[KOUME_LIMB_MAX];
    /* 0x2AC */ Vec3s morphTable[KOUME_LIMB_MAX];
    /* 0x34E */ u16 unk_34E;
    /* 0x350 */ UNK_TYPE1 unk350[0x4];
    /* 0x354 */ MsgScript* msgScript;
    /* 0x358 */ f32 animPlaySpeed;
    /* 0x35C */ f32 unk_35C;
    /* 0x360 */ s16 unk_360;
    /* 0x362 */ s16 unk_362;
    /* 0x364 */ s16 unk_364;
    /* 0x366 */ s16 unk_366;
    /* 0x368 */ s16 unk_368;
    /* 0x36A */ s16 unk_36A;
    /* 0x36C */ s16 blinkTimer;
    /* 0x36E */ s16 eyeTexIndex;
    /* 0x370 */ s16 unk_370;
    /* 0x372 */ s16 unk_372;
    /* 0x374 */ s16 csId;
    /* 0x378 */ MsgScriptCallback msgScriptCallback;
    /* 0x37C */ s32 animIndex;
    /* 0x380 */ UNK_TYPE1 unk380[0x4];
    /* 0x384 */ s32 unk_384;
    /* 0x388 */ s32 unk_388;
    /* 0x38C */ s32 unk_38C;
    /* 0x390 */ s32 unk_390;
    /* 0x394 */ EnTruUnkStruct unk_394[30];
} EnTru; // size = 0x934

#endif // Z_EN_TRU_H
