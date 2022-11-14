#ifndef Z_BG_JYA_MEGAMI_H
#define Z_BG_JYA_MEGAMI_H

#include "ultra64.h"
#include "global.h"

struct BgJyaMegami;

typedef void (*BgJyaMegamiActionFunc)(struct BgJyaMegami*, PlayState*);

typedef struct {
    /* 0x00 */ Vec3f pos;
    /* 0x0C */ Vec3f vel;
    /* 0x18 */ s16 rotVelX;
    /* 0x1A */ s16 rotVelY;
    /* 0x1C */ u32 epoch;
} BgJyaMegamiPiece; // size = 0x20

typedef struct BgJyaMegami {
    /* 0x0000 */ DynaPolyActor dyna;
    /* 0x0164 */ BgJyaMegamiActionFunc actionFunc;
    /* 0x0168 */ ColliderJntSph collider;
    /* 0x0188 */ ColliderJntSphElement colliderItem;
    /* 0x01C8 */ s16 lightTimer;
    /* 0x01CA */ s16 explosionTimer;
    /* 0x01CC */ s16 crumbleIndex;
    /* 0x01D0 */ BgJyaMegamiPiece pieces[13];
} BgJyaMegami; // size = 0x033C

#endif
