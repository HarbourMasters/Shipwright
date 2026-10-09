#ifndef Z_BG_HAKUGIN_POST_H
#define Z_BG_HAKUGIN_POST_H

#include "global.h"

struct BgHakuginPost;

typedef void (*BgHakuginPostActionFunc)(struct BgHakuginPost*, PlayState*);
typedef void (*BgHakuginPostFunc)(struct BgHakuginPost*);

#define BGHAKUGINPOST_GET_7(thisx) ((thisx)->params & 7)
#define BGHAKUGINPOST_GET_SWITCH_FLAG(thisx) (((thisx)->params >> 8) & 0x7F)
#define BGHAKUGINPOST_GET_SWITCH_FLAG_2(thisx) (((thisx)->home.rot.x) & 0x7F)

typedef struct {
    /* 0x00 */ s32 unk_00;
    /* 0x04 */ ColliderCylinder* collider;
    /* 0x08 */ Vec3f unk_08;
    /* 0x14 */ Vec3f unk_14;
    /* 0x20 */ f32 unk_20;
    /* 0x24 */ f32 unk_24;
    /* 0x28 */ s16 unk_28;
    /* 0x2A */ s16 csId;
    /* 0x2C */ s16 additionalCsId;
    /* 0x2E */ u8 switchFlag;
    /* 0x2F */ u8 unk_2F;
    /* 0x30 */ s8 unk_30;
    /* 0x34 */ s32 unk_34;
} BgHakuginPostUnkStruct1; // size = 0x38

typedef struct {
    /* 0x00 */ f32 unk_00;
    /* 0x04 */ Vec3f unk_04;
    /* 0x10 */ Vec3f unk_10;
    /* 0x1C */ f32 unk_1C;
    /* 0x20 */ Vec3s unk_20;
    /* 0x26 */ s16 unk_26;
    /* 0x28 */ s16 unk_28;
    /* 0x2A */ s16 unk_2A;
    /* 0x2C */ s8 unk_2C;
    /* 0x2D */ s8 unk_2D;
} BgHakuginPostUnkStruct2; // size = 0x30

typedef struct {
    /* 0x0000 */ BgHakuginPostUnkStruct1 unk_0000[12];
    /* 0x02A0 */ s32 count;
    /* 0x02A4 */ BgHakuginPostUnkStruct2 unk_02A4[120];
    /* 0x1924 */ u8 unk_1924;
} BgHakuginPostUnkStruct; // size = 0x1928

typedef struct {
    /* 0x000 */ ColliderCylinder colliders[8];
    /* 0x260 */ s32 count;
} BgHakuginPostColliders; // size = 0x264

typedef struct BgHakuginPost {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ BgHakuginPostActionFunc actionFunc;
    /* 0x160 */ f32 unk_160;
    /* 0x164 */ f32 unk_164;
    /* 0x168 */ f32 unk_168;
    /* 0x16C */ f32 unk_16C;
    /* 0x170 */ s32 unk_170;
    /* 0x174 */ s32 unk_174;
    /* 0x178 */ s8 unk_178;
    /* 0x179 */ s8 unk_179;
    /* 0x17C */ BgHakuginPostFunc unkFunc;
    /* 0x180 */ s16 csLength;
    /* 0x182 */ s16 additionalCsLength;
    /* 0x184 */ s16 csId;
    /* 0x186 */ s16 additionalCsId;
} BgHakuginPost; // size = 0x188

#endif // Z_BG_HAKUGIN_POST_H
