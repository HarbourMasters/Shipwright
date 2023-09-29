#ifndef Z_EN_GELDB_H
#define Z_EN_GELDB_H

#include <libultraship/libultra.h>
#include "global.h"

struct EnGeldB;

typedef enum {
    /* 0x00 */ GELDB_LIMB_NONE,
    /* 0x01 */ GELDB_LIMB_ROOT,
    /* 0x02 */ GELDB_LIMB_TORSO,
    /* 0x03 */ GELDB_LIMB_NECK,
    /* 0x04 */ GELDB_LIMB_PONYTAIL,
    /* 0x05 */ GELDB_LIMB_VEIL,
    /* 0x06 */ GELDB_LIMB_HEAD,
    /* 0x07 */ GELDB_LIMB_R_UPPER_ARM,
    /* 0x08 */ GELDB_LIMB_R_FOREARM,
    /* 0x09 */ GELDB_LIMB_R_WRIST,
    /* 0x0A */ GELDB_LIMB_R_HAND,
    /* 0x0B */ GELDB_LIMB_R_SWORD,
    /* 0x0C */ GELDB_LIMB_L_UPPER_ARM,
    /* 0x0D */ GELDB_LIMB_L_FOREARM,
    /* 0x0E */ GELDB_LIMB_L_WRIST,
    /* 0x0F */ GELDB_LIMB_L_HAND,
    /* 0x10 */ GELDB_LIMB_L_SWORD,
    /* 0x11 */ GELDB_LIMB_L_THIGH,
    /* 0x12 */ GELDB_LIMB_L_SHIN,
    /* 0x13 */ GELDB_LIMB_L_FOOT,
    /* 0x14 */ GELDB_LIMB_R_THIGH,
    /* 0x15 */ GELDB_LIMB_R_SHIN,
    /* 0x16 */ GELDB_LIMB_R_FOOT,
    /* 0x17 */ GELDB_LIMB_WAIST,
    /* 0x18 */ GELDB_LIMB_MAX
} EnGeldBLimb;

typedef void (*EnGeldBActionFunc)(struct EnGeldB*, PlayState*);

typedef struct EnGeldB {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ Vec3s bodyPartsPos[10];
    /* 0x0188 */ SkelAnime skelAnime;
    /* 0x01CC */ Vec3s jointTable[GELDB_LIMB_MAX];
    /* 0x025C */ Vec3s morphTable[GELDB_LIMB_MAX];
    /* 0x02EC */ s32 action;
    /* 0x02F0 */ char unk_2F0[4];
    /* 0x02F4 */ EnGeldBActionFunc actionFunc;
    /* 0x02F8 */ s16 unkTimer;
    /* 0x02FA */ s16 lookTimer;
    /* 0x02FC */ s16 iceTimer;
    /* 0x02FE */ u8 damageEffect;
    /* 0x0300 */ s32 timer;
    /* 0x0304 */ f32 approachRate;
    /* 0x0308 */ char unk_308[4];
    /* 0x030C */ f32 unkFloat;
    /* 0x0310 */ s16 meleeWeaponState;
    /* 0x0312 */ s16 spinAttackState;
    /* 0x0314 */ s16 keyFlag;
    /* 0x0316 */ char unk_316[2];
    /* 0x0318 */ s16 invisible;
    /* 0x031A */ u8 blinkState;
    /* 0x031C */ s32 blureIndex;
    /* 0x0320 */ ColliderCylinder bodyCollider;
    /* 0x036C */ ColliderQuad swordCollider;
    /* 0x03EC */ ColliderTris blockCollider;
    /* 0x040C */ ColliderTrisElement blockElements[2];
    /* 0x04C4 */ Vec3f rightFootPos;
    /* 0x04D0 */ Vec3f leftFootPos;
    /* 0x04DC */ Vec3s headRot;
} EnGeldB; // size = 0x04E4

#endif
