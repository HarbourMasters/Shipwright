#ifndef Z_EN_ZORAEGG_H
#define Z_EN_ZORAEGG_H

#include "global.h"
#include "objects/object_zoraegg/object_zoraegg.h"

struct EnZoraegg;

typedef void (*EnZoraeggActionFunc)(struct EnZoraegg*, PlayState*);

#define ZORA_EGG_GET_TYPE(thisx) ((thisx)->params & 0x1F)
#define ZORA_EGG_GET_SWITCH_FLAG(thisx) (((thisx)->params & 0xFE00) >> 9)

#define ZORA_EGG_PARAMS(type, switchFlag) (((type) & 0x1F) | (((switchFlag) & 0x7F) << 9))

typedef enum {
    /* 0x00 */ ZORA_EGG_TYPE_00,
    /* 0x01 */ ZORA_EGG_TYPE_01,
    /* 0x02 */ ZORA_EGG_TYPE_02,
    /* 0x03 */ ZORA_EGG_TYPE_03,
    /* 0x04 */ ZORA_EGG_TYPE_04,
    /* 0x05 */ ZORA_EGG_TYPE_05,
    /* 0x06 */ ZORA_EGG_TYPE_06,
    /* 0x07 */ ZORA_EGG_TYPE_07,
    /* 0x08 */ ZORA_EGG_TYPE_08,
    /* 0x09 */ ZORA_EGG_TYPE_09,
    /* 0x0A */ ZORA_EGG_TYPE_0A,
    /* 0x0B */ ZORA_EGG_TYPE_0B,
    /* 0x0C */ ZORA_EGG_TYPE_0C,
    /* 0x0D */ ZORA_EGG_TYPE_0D,
    /* 0x0E */ ZORA_EGG_TYPE_0E,
    /* 0x0F */ ZORA_EGG_TYPE_0F,
    /* 0x10 */ ZORA_EGG_TYPE_10,
    /* 0x11 */ ZORA_EGG_TYPE_11,
    /* 0x12 */ ZORA_EGG_TYPE_12,
    /* 0x13 */ ZORA_EGG_TYPE_13,
    /* 0x14 */ ZORA_EGG_TYPE_14,
    /* 0x15 */ ZORA_EGG_TYPE_15,
    /* 0x16 */ ZORA_EGG_TYPE_16,
    /* 0x17 */ ZORA_EGG_TYPE_17,
    /* 0x18 */ ZORA_EGG_TYPE_18,
    /* 0x19 */ ZORA_EGG_TYPE_19
} ZoraEggType;

typedef struct EnZoraegg {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ Vec3s jointTable[ZORA_BABY_LIMB_MAX];
    /* 0x1B2 */ Vec3s morphTable[ZORA_BABY_LIMB_MAX];
    /* 0x1DC */ Actor* unk_1DC;
    /* 0x1E0 */ f32 unk_1E0;
    /* 0x1E4 */ f32 unk_1E4;
    /* 0x1E8 */ s16 unk_1E8;
    /* 0x1EA */ u16 unk_1EA;
    /* 0x1EC */ u8 unk_1EC;
    /* 0x1ED */ u8 unk_1ED;
    /* 0x1EE */ u8 unk_1EE;
    /* 0x1EF */ u8 unk_1EF;
    /* 0x1F0 */ u16 cueType;
    /* 0x1F2 */ s16 eyeIndex;
    /* 0x1F4 */ s16 blinkTimer;
    /* 0x1F8 */ EnZoraeggActionFunc actionFunc;
} EnZoraegg; // size = 0x1FC

#endif // Z_EN_ZORAEGG_H
