#ifndef Z_EN_BSB_H
#define Z_EN_BSB_H

#include "global.h"
#include "objects/object_bsb/object_bsb.h"

struct EnBsb;

typedef void (*EnBsbActionFunc)(struct EnBsb*, PlayState*);

#define ENBSB_GET_PARAMS_8000(thisx) (((thisx)->params) & 0x8000)

#define ENBSB_GET_SWITCH_FLAG1(thisx) (((thisx)->params) & 0x7F)
#define ENBSB_GET_SWITCH_FLAG2(thisx) (((thisx)->world.rot.z) & 0x7F)
#define ENBSB_GET_PATH_INDEX(thisx) ((((thisx)->params) >> 7) & 0x1F)

#define ENBSB_PATH_INDEX_NONE 0x1F

#define ENBSB_PARAMS(switchFlag1, pathIndex) (((switchFlag1) & 0x7F) | (((pathIndex) & 0x1F) << 7))

#define ENBSB_GET_LIMB_INDEX(thisx) (((thisx)->params) & 0xFF)

#define ENBSB_PARAMS2(limbIndex) ((limbIndex) + 0x8000)


typedef enum EnBsbBodyPart {
    /*  0 */ ENBSB_BODYPART_0,
    /*  1 */ ENBSB_BODYPART_1,
    /*  2 */ ENBSB_BODYPART_2,
    /*  3 */ ENBSB_BODYPART_3,
    /*  4 */ ENBSB_BODYPART_4,
    /*  5 */ ENBSB_BODYPART_5,
    /*  6 */ ENBSB_BODYPART_6,
    /*  7 */ ENBSB_BODYPART_7,
    /*  8 */ ENBSB_BODYPART_9,
    /*  9 */ ENBSB_BODYPART_8,
    /* 10 */ ENBSB_BODYPART_10,
    /* 11 */ ENBSB_BODYPART_11,
    /* 12 */ ENBSB_BODYPART_12,
    /* 13 */ ENBSB_BODYPART_13,
    /* 14 */ ENBSB_BODYPART_14,
    /* 15 */ ENBSB_BODYPART_15,
    /* 16 */ ENBSB_BODYPART_16,
    /* 17 */ ENBSB_BODYPART_MAX
} EnBsbBodyPart;

typedef struct EnBsbEffect {
    /* 0x00 */ u8 isEnabled;
    /* 0x04 */ Vec3f pos;
    /* 0x10 */ Vec3f velocity;
    /* 0x1C */ Vec3f accel;
    /* 0x28 */ Vec3s rot;
    /* 0x30 */ f32 scale;
    /* 0x34 */ s16 life;
} EnBsbEffect; // size = 0x38

#define ENBSB_EFFECT_COUNT 50

typedef struct EnBsb {
    /* 0x0000 */ Actor actor;
    /* 0x0144 */ SkelAnime skelAnime;
    /* 0x0188 */ Vec3s jointTable[OBJECT_BSB_LIMB_MAX];
    /* 0x0206 */ Vec3s morphTable[OBJECT_BSB_LIMB_MAX];
    /* 0x0284 */ EnBsbActionFunc actionFunc;
    /* 0x0288 */ Path* path;
    /* 0x028C */ s32 waypoint;
    /* 0x0290 */ u8 playedSfx;
    /* 0x0292 */ s16 unk_0292;
    /* 0x0294 */ s16 unk_0294;
    /* 0x0296 */ s16 cueId;
    /* 0x0298 */ Vec3f unk_0298;
    /* 0x02A4 */ s32 unk_02A4;
    /* 0x02A8 */ s32 unk_02A8;
    /* 0x02AC */ s16 unk_02AC;
    /* 0x02AE */ u8 unk_02AE;
    /* 0x02AF */ u8 unk_02AF;
    /* 0x02B0 */ s32 unk_02B0;
    /* 0x02B4 */ s16 unk_02B4;
    /* 0x02B6 */ s16 pathIndex;
    /* 0x02B8 */ s16 switchFlag1;
    /* 0x02BA */ s16 switchFlag2;
    /* 0x02BC */ s16 unk_02BC;
    /* 0x02C0 */ f32 unk_02C0;
    /* 0x02C4 */ f32 animEndFrame;
    /* 0x02C8 */ UNK_TYPE1 unk_02C8[0x2];
    /* 0x02CA */ s16 alpha;
    /* 0x02CC */ s16 csIdList[5];
    /* 0x02D8 */ s32 animIndex;
    /* 0x02DC */ s32 unk_02DC;
    /* 0x02E0 */ Vec3f unk_02E0;
    /* 0x02EC */ Vec3f unk_02EC;
    /* 0x02F8 */ Vec3f unk_02F8;
    /* 0x0304 */ Vec3f unk_0304;
    /* 0x0310 */ Vec3s unk_0310;
    /* 0x0316 */ Vec3s unk_0316;
    /* 0x031C */ Vec3s unk_031C;
    /* 0x0322 */ s16 drawDmgEffTimer;
    /* 0x0324 */ s16 drawDmgEffType;
    /* 0x0328 */ f32 drawDmgEffScale;
    /* 0x032C */ f32 drawDmgEffFrozenSteamScale;
    /* 0x0330 */ Vec3f bodyPartsPos[ENBSB_BODYPART_MAX];
    /* 0x03FC */ s32 unk_03FC[ENBSB_BODYPART_MAX];
    /* 0x0440 */ s16 bodyPartIndex;
    /* 0x0444 */ EnBsbEffect effects[ENBSB_EFFECT_COUNT];
    /* 0x0F34 */ ColliderJntSph collider;
    /* 0x0F54 */ ColliderJntSphElement colliderElements[7];
    /* 0x1114 */ UNK_TYPE1 unk_1114[0x4];
    /* 0x1118 */ s16 unk_1118;
    /* 0x111A */ s16 subCamId;
    /* 0x111C */ s16 unk_111C;
    /* 0x1120 */ f32 subCamFov;
    /* 0x1124 */ f32 subCamFovNext;
    /* 0x1128 */ Vec3f subCamEye;
    /* 0x1134 */ Vec3f subCamAt;
    /* 0x1140 */ Vec3f subCamEyeNext;
    /* 0x114C */ Vec3f subCamAtNext;
} EnBsb; // size = 0x1158

#endif // Z_EN_BSB_H
