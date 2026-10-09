#ifndef Z_EN_AZ_H
#define Z_EN_AZ_H

#include "global.h"
#include "objects/object_az/object_az.h"

struct EnAz;

typedef void (*EnAzActionFunc)(struct EnAz*, PlayState*);

#define BEAVER_GET_PARAM_F00(thisx) (((thisx)->params >> 8) & 0xF)
#define BEAVER_GET_PATH_INDEX(thisx) ((thisx)->params & 0xFF)

typedef struct EnAz {
    /* 0x000 */ Actor actor;
    /* 0x144 */ EnAzActionFunc actionFunc;
    /* 0x148 */ SkelAnime skelAnime;
    /* 0x18C */ ColliderCylinder collider;
    /* 0x1D8 */ Vec3s jointTable[BEAVER_OLDER_BROTHER_LIMB_MAX]; // Also BEAVER_YOUNGER_BROTHER_LIMB_MAX
    /* 0x268 */ Vec3s morphTable[BEAVER_OLDER_BROTHER_LIMB_MAX]; // Also BEAVER_YOUNGER_BROTHER_LIMB_MAX
    /* 0x2F8 */ s16 unk_2F8;
    /* 0x2FA */ s16 unk_2FA; // cutscene state?
    /* 0x2FC */ s32 animIndex;
    /* 0x300 */ ActorPathing unk_300;
    /* 0x36C */ f32 unk_36C;
    /* 0x370 */ UNK_TYPE1 unk370[4];
    /* 0x374 */ u16 unk_374; // flags of some sort
    /* 0x376 */ u16 unk_376; // flags of some sort
    /* 0x378 */ u8 unk_378; // cutscene state?
    /* 0x37A */ s16 unk_37A;
    /* 0x37C */ s16 unk_37C;
    /* 0x37E */ s16 unk_37E;
    /* 0x380 */ s16 unk_380;
    /* 0x382 */ s16 unk_382;
    /* 0x384 */ s16 unk_384;
    /* 0x388 */ struct EnAz* brother;
    /* 0x38C */ UNK_TYPE1 unk38C[0x10];
    /* 0x39C */ s16 unk_39C;
    /* 0x39E */ s16 unk_39E; // some sort of rotation
    /* 0x3A0 */ UNK_TYPE1 unk3A0[4];
    /* 0x3A4 */ f32 unk_3A4;
    /* 0x3A8 */ Vec3f unk_3A8;
    /* 0x3B4 */ Vec3f unk_3B4; // translation 
    /* 0x3C0 */ s16 unk_3C0; // seems to do nothing
    /* 0x3C2 */ s16 unk_3C2;
    /* 0x3C4 */ s16 unk_3C4;
    /* 0x3C6 */ UNK_TYPE1 unk3C6[6];
    /* 0x3CC */ s32 getItemId;
    /* 0x3D0 */ s16 csIdList[1];
    /* 0x3D2 */ u16 unk_3D2;
    /* 0x3D4 */ s16 unk_3D4;
    /* 0x3D6 */ s16 unk_3D6;
} EnAz; // size = 0x3D8

#endif // Z_EN_AZ_H
