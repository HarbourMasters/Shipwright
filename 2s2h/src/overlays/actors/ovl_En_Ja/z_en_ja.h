#ifndef Z_EN_JA_H
#define Z_EN_JA_H

#include "global.h"
#include "objects/object_boj/object_boj.h"

struct EnJa;

typedef void (*EnJaActionFunc)(struct EnJa*, PlayState*);

#define ENJA_GET_3(thisx) (((thisx)->params & 3) & 0xFF)

typedef struct {
    /* 0x0 */ Vec3f unk_00;
    /* 0xC */ s16 unk_0C;
    /* 0xE */ s16 unk_0E;
} EnJaStruct; // size = 0x10

typedef struct EnJa {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ EnJaActionFunc actionFunc;
    /* 0x18C */ ColliderCylinder collider;
    /* 0x1D8 */ u8 scheduleResult;
    /* 0x1DC */ s32 msgScriptPos;
    /* 0x1E0 */ Actor* unk_1E0;
    /* 0x1E4 */ Vec3s unk_1E4;
    /* 0x1EC */ Vec3f unk_1EC;
    /* 0x1F8 */ Vec3f unk_1F8;
    /* 0x204 */ Vec3f unk_204;
    /* 0x210 */ Vec3f unk_210;
    /* 0x21C */ Vec3f unk_21C;
    /* 0x228 */ Vec3f unk_228;
    /* 0x234 */ EnJaStruct unk_234[4];
    /* 0x274 */ Vec3s unk_274;
    /* 0x27A */ Vec3s unk_27A;
    /* 0x280 */ Vec3s jointTable[OBJECT_BOJ_LIMB_MAX];
    /* 0x2E0 */ Vec3s morphTable[OBJECT_BOJ_LIMB_MAX];
    /* 0x340 */ u16 unk_340;
    /* 0x344 */ f32 animPlaySpeed;
    /* 0x348 */ f32 unk_348;
    /* 0x34C */ f32 unk_34C;
    /* 0x350 */ UNK_TYPE1 unk350[0x4];
    /* 0x354 */ s16 unk_354;
    /* 0x356 */ s16 unk_356;
    /* 0x358 */ s16 unk_358;
    /* 0x35A */ s16 unk_35A;
    /* 0x35C */ s16 unk_35C;
    /* 0x35E */ s16 unk_35E;
    /* 0x360 */ s16 unk_360;
    /* 0x362 */ s16 unk_362;
    /* 0x364 */ s16 unk_364;
    /* 0x366 */ s16 unk_366;
    /* 0x368 */ MsgScriptCallback msgScriptCallback;
    /* 0x36C */ s32 animIndex;
    /* 0x370 */ UNK_TYPE1 unk_370[4];
    /* 0x374 */ s32 prevTalkState;
} EnJa; // size = 0x378

#endif // Z_EN_JA_H
