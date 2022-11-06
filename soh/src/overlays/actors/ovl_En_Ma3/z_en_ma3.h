#ifndef Z_EN_MA3_H
#define Z_EN_MA3_H

#include "ultra64.h"
#include "global.h"

struct EnMa3;

typedef void (*EnMa3ActionFunc)(struct EnMa3*, PlayState*);

typedef enum {
    /* 0x00 */ MALON_ADULT_LIMB_NONE,
    /* 0x01 */ MALON_ADULT_LIMB_ROOT,
    /* 0x02 */ MALON_ADULT_LIMB_LOWER_CONTROL,
    /* 0x03 */ MALON_ADULT_LIMB_LEFT_THIGH,
    /* 0x04 */ MALON_ADULT_LIMB_LEFT_LEG,
    /* 0x05 */ MALON_ADULT_LIMB_LEFT_FOOT,
    /* 0x06 */ MALON_ADULT_LIMB_RIGHT_THIGH,
    /* 0x07 */ MALON_ADULT_LIMB_RIGHT_LEG,
    /* 0x08 */ MALON_ADULT_LIMB_RIGHT_FOOT,
    /* 0x09 */ MALON_ADULT_LIMB_TORSO,
    /* 0x0A */ MALON_ADULT_LIMB_DRESS,
    /* 0x0B */ MALON_ADULT_LIMB_CHEST_AND_NECK,
    /* 0x0C */ MALON_ADULT_LIMB_LEFT_SHOULDER,
    /* 0x0D */ MALON_ADULT_LIMB_LEFT_ARM,
    /* 0x0E */ MALON_ADULT_LIMB_LEFT_HAND,
    /* 0x0F */ MALON_ADULT_LIMB_RIGHT_SHOULDER,
    /* 0x10 */ MALON_ADULT_LIMB_RIGHT_ARM,
    /* 0x11 */ MALON_ADULT_LIMB_RIGHT_HAND,
    /* 0x12 */ MALON_ADULT_LIMB_HEAD,
    /* 0x13 */ MALON_ADULT_LIMB_MAX
} AdultMalonLimb;

typedef struct EnMa3 {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;
    /* 0x0190 */ EnMa3ActionFunc actionFunc;
    /* 0x0194 */ ColliderCylinder collider;
    /* 0x01E0 */ struct_80034A14_arg1 unk_1E0;
    /* 0x0208 */ s16 unk_208;
    /* 0x020A */ s16 unk_20A;
    /* 0x020C */ s16 blinkTimer;
    /* 0x020E */ s16 eyeIndex;
    /* 0x0210 */ s16 mouthIndex;
    /* 0x0212 */ Vec3s unk_212[MALON_ADULT_LIMB_MAX];
} EnMa3; // size = 0x0284

#endif
