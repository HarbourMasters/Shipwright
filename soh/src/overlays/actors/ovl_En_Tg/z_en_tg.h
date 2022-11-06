#ifndef Z_EN_TG_H
#define Z_EN_TG_H

#include "ultra64.h"
#include "global.h"

struct EnTg;

typedef void (*EnTgActionFunc)(struct EnTg*, PlayState*);

typedef struct EnTg {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;
    /* 0x0190 */ EnTgActionFunc actionFunc;
    /* 0x0194 */ ColliderCylinder collider;
    /* 0x01E0 */ s16 isTalking;
    /* 0x01E2 */ char unk_1E2[0x26];
    /* 0x0208 */ u8 nextDialogue;
} EnTg; // size = 0x020C

#endif
