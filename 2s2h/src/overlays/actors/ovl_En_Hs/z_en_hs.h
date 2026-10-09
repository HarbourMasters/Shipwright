#ifndef Z_EN_HS_H
#define Z_EN_HS_H

#include "global.h"
#include "objects/object_hs/object_hs.h"

struct EnHs;

typedef void (*EnHsActionFunc)(struct EnHs*, PlayState*);

#define HS_GET_EXIT_INDEX(thisx) ((thisx)->params & 0xF)

#define HS_TYPE_UNK1 1

// params mystery: Vanilla Grog is 0xFE01
//   0xFE00 space is never checked in Grog code
//   at the same time, type UNK1 is only checked directly with params == 1, no &F
//   so HS_TYPE_UNK1 is never valid and is unused, as 0xFE00 is still present even if its not doing anything else

// The count of [chicks following the player]   is stored in [this->actor.home.rot.z] (incremented by 2)
// The count of [chicks transformed into adult] is stored in [this->actor.home.rot.x] (incremented by 2)

typedef struct EnHs {
    /* 0x000 */ Actor actor;
    /* 0x144 */ ColliderCylinder collider;
    /* 0x190 */ SkelAnime skelAnime;
    /* 0x1D4 */ Vec3s jointTable[OBJECT_HS_LIMB_MAX];
    /* 0x234 */ Vec3s morphTable[OBJECT_HS_LIMB_MAX];
    /* 0x294 */ Vec3s headRot;
    /* 0x29A */ Vec3s torsoRot; // Set but unused
    /* 0x2A0 */ u16 stateFlags;
    /* 0x2A2 */ s16 stateTimer; // reused for different actionFunc
    /* 0x2A4 */ Vec3f nwcPos[20]; // actual chick position are even values, odd values seem to be extra values for smoother chain
    /* 0x394 */ EnHsActionFunc actionFunc;
} EnHs; // size = 0x398

#endif // Z_EN_HS_H
