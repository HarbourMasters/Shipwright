#ifndef Z_EN_MA2_H
#define Z_EN_MA2_H

#include "ultra64.h"
#include "global.h"

struct EnMa2;

typedef void (*EnMa2ActionFunc)(struct EnMa2*, PlayState*);

typedef enum {
    /* 0x00 */ MALON_ADULT_LIMB_NONE,
    /* 0x01 */ MALON_ADULT_ROOT_LIMB,
    /* 0x02 */ MALON_ADULT_LOWER_CONTROL_LIMB,
    /* 0x03 */ MALON_ADULT_LEFT_THIGH_LIMB,
    /* 0x04 */ MALON_ADULT_LEFT_LEG_LIMB,
    /* 0x05 */ MALON_ADULT_LEFT_FOOT_LIMB,
    /* 0x06 */ MALON_ADULT_RIGHT_THIGH_LIMB,
    /* 0x07 */ MALON_ADULT_RIGHT_LEG_LIMB,
    /* 0x08 */ MALON_ADULT_RIGHT_FOOT_LIMB,
    /* 0x09 */ MALON_ADULT_TORSO_LIMB,
    /* 0x0A */ MALON_ADULT_DRESS_LIMB,
    /* 0x0B */ MALON_ADULT_CHEST_AND_NECK_LIMB,
    /* 0x0C */ MALON_ADULT_LEFT_SHOULDER_LIMB,
    /* 0x0D */ MALON_ADULT_LEFT_ARM_LIMB,
    /* 0x0E */ MALON_ADULT_LEFT_HAND_LIMB,
    /* 0x0F */ MALON_ADULT_RIGHT_SHOULDER_LIMB,
    /* 0x10 */ MALON_ADULT_RIGHT_ARM_LIMB,
    /* 0x11 */ MALON_ADULT_RIGHT_HAND_LIMB,
    /* 0x12 */ MALON_ADULT_HEAD_LIMB,
    /* 0x13 */ MALON_ADULT_LIMB_MAX
} AdultMalonLimb;

typedef struct EnMa2 {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;
    /* 0x0190 */ EnMa2ActionFunc actionFunc;
    /* 0x0194 */ ColliderCylinder collider;
    /* 0x01E0 */ struct_80034A14_arg1 unk_1E0;
    /* 0x0208 */ s16 unk_208;
    /* 0x020A */ s16 unk_20A;
    /* 0x020C */ s16 blinkTimer;
    /* 0x020E */ s16 eyeIndex;
    /* 0x0210 */ s16 mouthIndex;
    /* 0x0212 */ Vec3s unk_212[MALON_ADULT_LIMB_MAX];
} EnMa2; // size = 0x0284

#endif
