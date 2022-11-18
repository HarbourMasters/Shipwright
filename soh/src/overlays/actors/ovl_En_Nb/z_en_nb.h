#ifndef Z_EN_NB_H
#define Z_EN_NB_H

#include "ultra64.h"
#include "global.h"

struct EnNb;

typedef enum {
    /* 0x00 */ NB_LIMB_NONE,
    /* 0x01 */ NB_LIMB_ROOT,
    /* 0x02 */ NB_LIMB_L_THIGH,
    /* 0x03 */ NB_LIMB_L_SHIN,
    /* 0x04 */ NB_LIMB_L_FOOT,
    /* 0x05 */ NB_LIMB_R_THIGH,
    /* 0x06 */ NB_LIMB_R_SHIN,
    /* 0x07 */ NB_LIMB_R_FOOT,
    /* 0x08 */ NB_LIMB_TORSO,
    /* 0x09 */ NB_LIMB_L_UPPER_ARM,
    /* 0x0A */ NB_LIMB_L_FOREARM,
    /* 0x0B */ NB_LIMB_L_HAND,
    /* 0x0C */ NB_LIMB_R_UPPER_ARM,
    /* 0x0D */ NB_LIMB_R_FOREARM,
    /* 0x0E */ NB_LIMB_R_HAND,
    /* 0x0F */ NB_LIMB_HEAD,
    /* 0x10 */ NB_LIMB_BLANK,
    /* 0x11 */ NB_LIMB_PONYTAIL,
    /* 0x12 */ NB_LIMB_WAIST,
    /* 0x13 */ NB_LIMB_MAX
} EnNbLimb;

typedef void (*EnNbActionFunc)(struct EnNb*, PlayState*);
typedef void (*EnNbDrawFunc)(struct EnNb*, PlayState*);

typedef struct EnNb {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;
    /* 0x0190 */ Vec3s jointTable[NB_LIMB_MAX];
    /* 0x0202 */ Vec3s morphTable[NB_LIMB_MAX];
    /* 0x0274 */ s16 eyeIdx;
    /* 0x0276 */ s16 blinkTimer;
    /* 0x0278 */ s32 action;
    /* 0x027C */ s32 drawMode;
    /* 0x0280 */ f32 alphaTimer;
    /* 0x0284 */ u32 alpha;
    /* 0x0288 */ s32 flag;
    /* 0x028C */ s32 previousCsAction;
    /* 0x0290 */ f32 timer;
    /* 0x0294 */ ColliderCylinder collider;
    /* 0x02E0 */ s32 headTurnFlag;
    /* 0x02E4 */ Vec3f initialPos;
    /* 0x02F0 */ Vec3f finalPos;
    /* 0x02FC */ s16 pathYaw;
    /* 0x02FE */ u16 movementTimer;
    /* 0x0300 */ struct_80034A14_arg1 unk_300;
} EnNb; // size = 0x0328

typedef enum {
    /* 0x02 */ NB_TYPE_DEMO02 = 2,
    /* 0x03 */ NB_TYPE_KIDNAPPED,
    /* 0x04 */ NB_TYPE_KNUCKLE,
    /* 0x05 */ NB_TYPE_CREDITS,
    /* 0x06 */ NB_TYPE_CRAWLSPACE
} EnNbType;

#endif
