#ifndef Z_DM_STK_H
#define Z_DM_STK_H

#include "global.h"
#include "objects/object_stk/object_stk.h"

struct DmStk;

typedef void (*DmStkActionFunc)(struct DmStk*, PlayState*);

#define DM_STK_GET_TYPE(thisx) ((thisx)->params)

typedef enum {
    /* 0 */ DM_STK_TYPE_SKULL_KID,
    /* 1 */ DM_STK_TYPE_MAJORAS_MASK
} DmStkType;

typedef struct DmStk {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ Vec3s jointTable[SKULL_KID_LIMB_MAX]; // Not used, since it's allocated dynamically instead.
    /* 0x20C */ Vec3s morphTable[SKULL_KID_LIMB_MAX]; // Not used, since it's allocated dynamically instead.
    /* 0x290 */ DmStkActionFunc actionFunc;
    /* 0x294 */ ColliderCylinder collider;
    /* 0x2E0 */ s16 animIndex;
    /* 0x2E4 */ s32 alpha;
    /* 0x2E8 */ u32 fogR;
    /* 0x2EC */ u32 fogG;
    /* 0x2F0 */ u32 fogB;
    /* 0x2F4 */ s32 fogA;
    /* 0x2F8 */ s32 fogN;
    /* 0x2FC */ s32 fogF;
    /* 0x300 */ f32 fogScale;
    /* 0x304 */ Vec3f headPos; // set but never used
    /* 0x310 */ Vec3f oathToOrderCutsceneVoicePos;
    /* 0x31C */ UNK_TYPE1 unk31C[0xC];
    /* 0x328 */ u16 tatlMessageTimer;
    /* 0x32A */ u16 bobPhase;
    /* 0x32C */ u8 maskType;
    /* 0x32D */ u8 handType;
    /* 0x32E */ u8 fadeInState;
    /* 0x32F */ u8 fadeOutState;
    /* 0x330 */ s32 fadeOutTimer;
    /* 0x334 */ u8 cueId;
    /* 0x335 */ u8 hasBeenHit; // set but never used
    /* 0x336 */ s8 objectStkObjectSlot;
    /* 0x337 */ s8 objectStk2ObjectSlot;
    /* 0x338 */ s8 objectStk3ObjectSlot;
    /* 0x339 */ u8 deflectCount;
    /* 0x33A */ u8 dekuPipesCutsceneState;
    /* 0x33B */ u8 shouldDraw;
} DmStk; // size = 0x33C

#endif // Z_DM_STK_H
