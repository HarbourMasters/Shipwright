#ifndef Z_EN_STOP_HEISHI_H
#define Z_EN_STOP_HEISHI_H

#include "global.h"
#include "objects/object_sdn/object_sdn.h"

struct EnStopheishi;

typedef void (*EnStopheishiActionFunc)(struct EnStopheishi*, PlayState*);

#define ENSTOPHEISHI_GET_SWITCH_FLAG(thisx) (((thisx)->params) & 0x3F)
#define ENSTOPHEISHI_GET_F000(thisx) ((((thisx)->params) >> 0xC) & 0xF)

#define ENSTOPHEISHI_SWITCH_FLAG_NONE 0x7F

typedef struct EnStopheishi {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ Vec3s jointTable[SOLDIER_LIMB_MAX];
    /* 0x1EE */ Vec3s morphTable[SOLDIER_LIMB_MAX];
    /* 0x254 */ EnStopheishiActionFunc actionFunc;
    /* 0x258 */ s16 headRotZ;
    /* 0x25A */ s16 headRotX;
    /* ox25C */ UNK_TYPE1 unk_25C[2];
    /* 0x25E */ s16 pitchToPlayer;
    /* 0x260 */ s16 headRotXTarget;
    /* 0x262 */ s16 unk_262; // Unused
    /* 0x264 */ u8 unk_264; // Locking value at `0` prevents leaving the guard pose, however locking to 1 doesn't prevent guarding.
    /* 0x265 */ u8 unk_265;
    /* 0x266 */ s16 unk_266;
    /* 0x268 */ s32 animIndex;
    /* 0x26C */ f32 animEndFrame;
    /* 0x270 */ s16 timer;
    /* 0x272 */ s16 headTurnTimer1;
    /* 0x274 */ s16 unk_274;
    /* 0x276 */ s16 unk_276;
    /* 0x278 */ s16 disableCollider;
    /* 0x27A */ s16 switchFlag;
    /* 0x27C */ s16 messageIndex;
    /* 0x27E */ s16 unk_27E;
    /* 0x280 */ s16 rotYTarget;
    /* 0x284 */ s32 headTurnTimer2;
    /* 0x288 */ f32 unk_288;
    /* 0x28C */ f32 maxMoveStep;
    /* 0x28C */ Vec3f headWorldPos;
    /* 0x29C */ ColliderCylinder collider;
} EnStopheishi; // size = 0x2E8

#endif // Z_EN_STOP_HEISHI_H
