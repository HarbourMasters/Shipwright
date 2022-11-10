#ifndef Z_EN_PEEHAT_H
#define Z_EN_PEEHAT_H

#include "ultra64.h"
#include "global.h"

typedef enum {
    /* -1 */ PEAHAT_TYPE_GROUNDED = -1,
    /*  0 */ PEAHAT_TYPE_FLYING = 0,
    /*  1 */ PEAHAT_TYPE_LARVA = 1
} PeahatType;

struct EnPeehat;

typedef void (*EnPeehatActionFunc)(struct EnPeehat*, PlayState*);

typedef struct EnPeehat {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;
    /* 0x0190 */ Vec3s jointTable[24];
    /* 0x0220 */ Vec3s morphTable[24];
    /* 0x02B0 */ s32 state;
    /* 0x02B4 */ s32 isStateDieFirstUpdate;
    /* 0x02B8 */ EnPeehatActionFunc actionFunc;
    /* 0x02BC */ Vec3f bladeTip[2]; // used to simulate the peahat's blades "digging up" earth
    /* 0x02D4 */ s32 unk_2D4;
    /* 0x02D8 */ f32 xzDistMax; // max xz dist to follow Link (PEAHAT_TYPE_GROUNDED) or spawn larva (PEAHAT_TYPE_FLYING)
    /* 0x02DC */ f32 xzDistToRise; // xz dist needed to rise from the ground
    /* 0x02E0 */ f32 unk_2E0;       // rot?
    /* 0x02E4 */ f32 jiggleRot;
    /* 0x02E8 */ f32 jiggleRotInc;
    /* 0x02EC */ f32 scaleShift;  // 0.0f for no distortion. used for "jiggle" effect when stabbed on ground
    /* 0x02F0 */ s16 bladeRotVel; // spinning blades rotational velocity
    /* 0x02F2 */ s16 bladeRot;    // spinning blades rotation component
    /* 0x02F4 */ s16 unk_2F4;
    /* 0x02F6 */ s16 riseDelayTimer;  // countdown timer until peahat is allowed to rise up from the ground
    /* 0x02F8 */ s16 seekPlayerTimer; // number of frames the peahat should seek the player before landing to rest
    /* 0x02FA */ s16 unk_2FA;          // larva count (PEAHAT_TYPE_FLYING, PEAHAT_TYPE_GROUNDED),
                                      // shape rotation direction (PEAHAT_TYPE_GROUNDED)
    /* 0x02FC */ s16 animTimer;
    /* 0x0300 */ ColliderCylinder colCylinder;
    /* 0x034C */ ColliderJntSph colJntSph;
    /* 0x036C */ ColliderJntSphElement colJntSphItemList[1];
    /* 0x03AC */ ColliderQuad colQuad;
} EnPeehat; // size = 0x042C

#endif
