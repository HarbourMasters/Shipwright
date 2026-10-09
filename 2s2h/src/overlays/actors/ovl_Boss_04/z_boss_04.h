#ifndef Z_BOSS_04_H
#define Z_BOSS_04_H

#include "global.h"
#include "objects/object_boss04/object_boss04.h"

struct Boss04;

typedef void (*Boss04ActionFunc)(struct Boss04*, PlayState*);

typedef struct Boss04 {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ Vec3s jointTable[WART_LIMB_MAX];
    /* 0x1BE */ Vec3s morphTable[WART_LIMB_MAX];
    /* 0x1F4 */ s16 unk_1F4;
    /* 0x1F6 */ u8 unk_1F6;
    /* 0x1F7 */ u8 unk_1F7;
    /* 0x1F8 */ s16 unk_1F8;
    /* 0x1FA */ s16 unk_1FA;
    /* 0x1FC */ UNK_TYPE1 unk_1FC[2];
    /* 0x1FE */ s16 unk_1FE;
    /* 0x200 */ s16 unk_200;
    /* 0x204 */ Boss04ActionFunc actionFunc;
    /* 0x208 */ ColliderJntSph collider1;
    /* 0x228 */ ColliderJntSphElement collider1Elements[1];
    /* 0x268 */ ColliderJntSph collider2;
    /* 0x288 */ ColliderJntSphElement collider2Elements[1];
    /* 0x2C8 */ s16 unk_2C8;
    /* 0x2CC */ f32 unk_2CC;
    /* 0x2D0 */ f32 unk_2D0;
    /* 0x2D4 */ s16 unk_2D4;
    /* 0x2D6 */ s16 unk_2D6;
    /* 0x2D8 */ s16 unk_2D8;
    /* 0x2DA */ s16 unk_2DA;
    /* 0x2DC */ s16 unk_2DC;
    /* 0x2DE */ s16 unk_2DE;
    /* 0x2E0 */ s16 unk_2E0;
    /* 0x2E2 */ s16 unk_2E2;
    /* 0x2E4 */ Vec3f unk_2E4[16];
    /* 0x3A4 */ Vec3f unk_3A4[15];
    /* 0x458 */ Vec3f unk_458[15];
    /* 0x50C */ Vec3f unk_50C[11];
    /* 0x590 */ Vec3f unk_590[11];
    /* 0x614 */ Vec3f unk_614[6];
    /* 0x65C */ Vec3f unk_65C[6];
    /* 0x6A4 */ Vec3f unk_6A4;
    /* 0x6B0 */ Vec3f unk_6B0;
    /* 0x6BC */ Vec3f unk_6BC;
    /* 0x6C8 */ Vec3f unk_6C8;
    /* 0x6D4 */ f32 unk_6D4;
    /* 0x6D8 */ f32 unk_6D8;
    /* 0x6DC */ f32 unk_6DC;
    /* 0x6E0 */ f32 unk_6E0;
    /* 0x6E4 */ f32 unk_6E4;
    /* 0x6E8 */ f32 unk_6E8;
    /* 0x6EC */ UNK_TYPE1 unk_6EC[4];
    /* 0x6F0 */ f32 unk_6F0;
    /* 0x6F4 */ s16 unk_6F4;
    /* 0x6F6 */ s16 unk_6F6;
    /* 0x6F8 */ f32 unk_6F8;
    /* 0x6FC */ f32 unk_6FC;
    /* 0x700 */ f32 unk_700;
    /* 0x704 */ u32 unk_704;
    /* 0x708 */ s16 unk_708;
    /* 0x70A */ s16 subCamId;
    /* 0x70C */ Vec3f subCamEye;
    /* 0x718 */ Vec3f subCamAt;
    /* 0x724 */ UNK_TYPE1 unk_724[4];
    /* 0x728 */ f32 unk_728;
    /* 0x72C */ UNK_TYPE1 unk_72C[0x10];
    /* 0x73C */ f32 unk_73C;
    /* 0x740 */ UNK_TYPE1 unk_740[4];
    /* 0x744 */ f32 subCamFov;
    /* 0x748 */ s16 subCamAtOscillator;
    /* 0x74A */ u8 unk_74A;
} Boss04; // size = 0x74C

#endif // Z_BOSS_04_H
