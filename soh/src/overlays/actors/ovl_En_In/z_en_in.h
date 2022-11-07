#ifndef Z_EN_IN_H
#define Z_EN_IN_H

#include "ultra64.h"
#include "global.h"

struct EnIn;

typedef void (*EnInActionFunc)(struct EnIn*, PlayState*);

typedef enum {
    /* 0x00 */ INGO_LIMB_NONE,
    /* 0x01 */ INGO_LIMB_ROOT,
    /* 0x02 */ INGO_LEFT_THIGH_LIMB,
    /* 0x03 */ INGO_LEFT_LEG_LIMB,
    /* 0x04 */ INGO_LEFT_FOOT_LIMB,
    /* 0x05 */ INGO_RIGHT_THIGH_LIMB,
    /* 0x06 */ INGO_RIGHT_LEG_LIMB,
    /* 0x07 */ INGO_RIGHT_FOOT_LIMB,
    /* 0x08 */ INGO_TORSO_LIMB,
    /* 0x09 */ INGO_CHEST_LIMB,
    /* 0x0A */ INGO_LEFT_SHOULDER_LIMB,
    /* 0x0B */ INGO_LEFT_ARM_LIMB,
    /* 0x0C */ INGO_LEFT_HAND_LIMB,
    /* 0x0D */ INGO_RIGHT_SHOULDER_LIMB,
    /* 0x0E */ INGO_RIGHT_ARM_LIMB,
    /* 0x0F */ INGO_RIGHT_HAND_LIMB,
    /* 0x10 */ INGO_HEAD_LIMB,
    /* 0x11 */ INGO_LEFT_EYEBROW_LIMB,
    /* 0x12 */ INGO_RIGHTEYEBROW_LIMB,
    /* 0x13 */ INGO_MUSTACHE_LIMB,
    /* 0x14 */ INGO_LIMB_MAX
} IngoLimb;

typedef struct EnIn {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;
    /* 0x0190 */ EnInActionFunc actionFunc;
    /* 0x0194 */ ColliderCylinder collider;
    /* 0x01E0 */ f32 unk_1E0;
    /* 0x01E4 */ s8 ingoObjBankIndex;
    /* 0x01E6 */ s16 animationIdx;
    /* 0x01E8 */ s16 unk_1E8;
    /* 0x01EA */ s16 blinkTimer;
    /* 0x01EC */ s16 unk_1EC;
    /* 0x01EE */ s16 eyeIndex;
    /* 0x01F0 */ s16 camId;
    /* 0x01F2 */ s16 activeCamId;
    /* 0x01F4 */ char unk_1F4[0x4];
    /* 0x01F8 */ s16 unk_1F8;
    /* 0x01FA */ s16 unk_1FA;
    /* 0x01FC */ s16 unk_1FC;
    /* 0x01FE */ Vec3s jointTable[INGO_LIMB_MAX];
    /* 0x0276 */ Vec3s morphTable[INGO_LIMB_MAX];
    /* 0x02F0 */ f32 unk_2F0;
    /* 0x02F4 */ f32 unk_2F4;
    /* 0x02F8 */ f32 unk_2F8;
    /* 0x02FC */ f32 unk_2FC;
    /* 0x0300 */ f32 unk_300;
    /* 0x0304 */ f32 unk_304;
    /* 0x0308 */ struct_80034A14_arg1 unk_308;
    /* 0x0330 */ Vec3s unk_330[INGO_LIMB_MAX];
} EnIn; // size = 0x03A8

#endif
