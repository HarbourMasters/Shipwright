#ifndef Z_EN_HATA_H
#define Z_EN_HATA_H

#include "ultra64.h"
#include "global.h"


typedef enum {
    /* 0x00 */ FLAGPOLE_LIMB_NONE,
    /* 0x01 */ FLAGPOLE_LIMB_POLE_BASE,
    /* 0x02 */ FLAGPOLE_LIMB_POLE,
    /* 0x03 */ FLAGPOLE_LIMB_FLAG_1_BASE,
    /* 0x04 */ FLAGPOLE_LIMB_FLAG_1_HOIST_END_BASE,
    /* 0x05 */ FLAGPOLE_LIMB_FLAG_1_HOIST_MID_BASE,
    /* 0x06 */ FLAGPOLE_LIMB_FLAG_1_FLY_MID_BASE,
    /* 0x07 */ FLAGPOLE_LIMB_FLAG_1_FLY_END_BASE,
    /* 0x08 */ FLAGPOLE_LIMB_FLAG_1_FLY_END,
    /* 0x09 */ FLAGPOLE_LIMB_FLAG_1_FLY_MID,
    /* 0x0A */ FLAGPOLE_LIMB_FLAG_1_HOIST_MID,
    /* 0x0B */ FLAGPOLE_LIMB_FLAG_1_HOIST_END,
    /* 0x0C */ FLAGPOLE_LIMB_FLAG_2_BASE,
    /* 0x0D */ FLAGPOLE_LIMB_FLAG_2_HOIST_END_BASE,
    /* 0x0E */ FLAGPOLE_LIMB_FLAG_2_HOIST_MID_BASE,
    /* 0x0F */ FLAGPOLE_LIMB_FLAG_2_FLY_MID_BASE,
    /* 0x10 */ FLAGPOLE_LIMB_FLAG_2_FLY_END_BASE,
    /* 0x11 */ FLAGPOLE_LIMB_FLAG_2_FLY_END,
    /* 0x12 */ FLAGPOLE_LIMB_FLAG_2_FLY_MID,
    /* 0x13 */ FLAGPOLE_LIMB_FLAG_2_HOIST_MID,
    /* 0x14 */ FLAGPOLE_LIMB_FLAG_2_HOIST_END,
    /* 0x15 */ FLAGPOLE_LIMB_MAX
} EnHataLimb;

typedef struct {
    /* 0x0000 */ DynaPolyActor dyna;
    /* 0x0164 */ SkelAnime skelAnime;
    /* 0x01A8 */ ColliderCylinder collider; // Unused, but indicated by the form of the unused data
    /* 0x01F4 */ Vec3s limbs[FLAGPOLE_LIMB_MAX];
    /* 0x0272 */ s16 invScale;
    /* 0x0274 */ s16 maxStep;
    /* 0x0276 */ s16 minStep;
    /* 0x0278 */ s16 unk_278;
} EnHata; // size = 0x027C

#endif
