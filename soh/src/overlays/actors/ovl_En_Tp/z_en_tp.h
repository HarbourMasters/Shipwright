#ifndef Z_EN_TP_H
#define Z_EN_TP_H

#include "ultra64.h"
#include "global.h"

struct EnTp;

typedef void (*EnTpActionFunc)(struct EnTp*, PlayState*);

typedef struct EnTp {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ s32 actionIndex;
    /* 0x0150 */ s32 unk_150; // Some kind of state indicator
    /* 0x0154 */ EnTpActionFunc actionFunc;
    /* 0x0158 */ u8 damageEffect; // Used to propagate the effect to the other segments' actors
    /* 0x015A */ s16 timer;
    /* 0x015C */ s16 unk_15C; // Multipurpose, used to change the horizontal position of tail segments 
    /* 0x015E */ s16 alpha; // The dying types fade away
    /* 0x0160 */ s16 red;
    /* 0x0162 */ s16 kiraSpawnTimer;
    /* 0x0164 */ s16 unk_164; // Used once, never set to nonzero
    /* 0x0168 */ f32 heightPhase;
    /* 0x016C */ f32 horizontalVariation;
    /* 0x0170 */ f32 extraHeightVariation;
    /* 0x0174 */ ColliderJntSph collider;
    /* 0x0194 */ ColliderJntSphElement colliderItems[1];
    /* 0x01D4 */ struct EnTp* head;
} EnTp; // size = 0x01D8

typedef enum {
    /* -1 */ TAILPASARAN_HEAD = -1, // Used when scenes spawn them: code only cares about < 0
    /*  0 */ TAILPASARAN_TAIL,
    /* 10 */ TAILPASARAN_FRAGMENT = 10,
    /* 11 */ TAILPASARAN_TAIL_DYING,
    /* 12 */ TAILPASARAN_HEAD_DYING
} EnTpType;

#endif
