#ifndef Z_DM_ZL_H
#define Z_DM_ZL_H

#include "global.h"
#include "objects/object_zl4/object_zl4.h"

struct DmZl;

typedef void (*DmZlActionFunc)(struct DmZl*, PlayState*);

typedef struct DmZl {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ Vec3s jointTable[ZL4_LIMB_MAX]; // these tables are not referenced directly, loaded dynamically
    /* 0x1F4 */ Vec3s morphTable[ZL4_LIMB_MAX];
    /* 0x260 */ DmZlActionFunc actionFunc;
    /* 0x264 */ ColliderCylinder collider; // unused, assumed to be here based on OoT documentation
    /* 0x2B0 */ s16 animIndex;
    /* 0x2B2 */ u8 eyeTextureIndexLeft;
    /* 0x2B3 */ u8 eyeTextureIndexRight;
    /* 0x2B4 */ u8 mouthTextureIndex;
    /* 0x2B5 */ u8 nextEyeState;   // used to control eye state, but not set by our actor, outside of actor?
    /* 0x2B6 */ u8 nextMouthState; // used to control mouth state, but not set by our actor, outside of actor?
    /* 0x2B8 */ s16 blinkTimer;
    /* 0x2BA */ s16 unk_2BA; // set but not used by this actor.
    /* 0x2BC */ UNK_TYPE1 pad2BC[0x18]; // unused by DmZl
} DmZl; // size = 0x2D4

#endif // Z_DM_ZL_H
