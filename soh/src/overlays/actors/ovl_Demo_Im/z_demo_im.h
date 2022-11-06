#ifndef Z_DEMO_IM_H
#define Z_DEMO_IM_H

#include "ultra64.h"
#include "global.h"

struct DemoIm;

typedef void (*DemoImActionFunc)(struct DemoIm*, PlayState*);
typedef void (*DemoImDrawFunc)(struct DemoIm*, PlayState*);

typedef enum {
    /* 0x00 */ IMPA_LIMB_NONE,
    /* 0x01 */ IMPA_LIMB_ROOT,
    /* 0x02 */ IMPA_LIMB_TORSO,
    /* 0x03 */ IMPA_LIMB_LEFT_THIGH,
    /* 0x04 */ IMPA_LIMB_LEFT_LEG,
    /* 0x05 */ IMPA_LIMB_LEFT_FOOT,
    /* 0x06 */ IMPA_LIMB_RIGHT_THIGH,
    /* 0x07 */ IMPA_LIMB_RIGHT_LEG,
    /* 0x08 */ IMPA_LIMB_RIGHT_FOOT,
    /* 0x09 */ IMPA_LIMB_CHEST,
    /* 0x0A */ IMPA_LIMB_LEFT_SHOULDER,
    /* 0x0B */ IMPA_LIMB_LEFT_ARM,
    /* 0x0C */ IMPA_LIMB_LEFT_HAND,
    /* 0x0D */ IMPA_LIMB_RIGHT_SHOULDER,
    /* 0x0E */ IMPA_LIMB_RIGHT_ARM,
    /* 0x0F */ IMPA_LIMB_RIGHT_HAND,
    /* 0x10 */ IMPA_LIMB_HEAD,
    /* 0x11 */ IMPA_LIMB_MAX
} ImpaLimb;

typedef struct DemoIm {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;
    /* 0x0190 */ Vec3s jointTable[IMPA_LIMB_MAX];
    /* 0x01F6 */ Vec3s morphTable[IMPA_LIMB_MAX];
    /* 0x025C */ s16 eyeIndex;
    /* 0x025E */ s16 blinkTimer;
    /* 0x0260 */ s32 action;
    /* 0x0264 */ s32 drawConfig;
    /* 0x0268 */ f32 unk_268;
    /* 0x026C */ s32 alpha;
    /* 0x0270 */ s32 unk_270;
    /* 0x0274 */ s32 unk_274;
    /* 0x0278 */ f32 unk_278;
    /* 0x027C */ s32 unk_27C;
    /* 0x0280 */ s32 unk_280;
    /* 0x0284 */ ColliderCylinder collider;
    /* 0x02D0 */ s32 unk_2D0;
    /* 0x02D4 */ struct_80034A14_arg1 unk_2D4;
} DemoIm; // size = 0x02FC

#endif
