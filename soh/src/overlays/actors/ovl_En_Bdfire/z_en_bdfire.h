#ifndef Z_EN_BDFIRE_H
#define Z_EN_BDFIRE_H

#include "ultra64.h"
#include "global.h"
#include "overlays/actors/ovl_Boss_Dodongo/z_boss_dodongo.h"

struct EnBdfire;

typedef void (*EnBdfireActionFunc)(struct EnBdfire*, PlayState*);
typedef void (*EnBdfireDrawFunc)(struct EnBdfire*, PlayState*);

typedef struct EnBdfire {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ EnBdfireActionFunc actionFunc;
    /* 0x0150 */ EnBdfireDrawFunc drawFunc;
    /* 0x0154 */ s16 unk_154;
    /* 0x0156 */ s16 unk_156;
    /* 0x0158 */ s16 unk_158;
    /* 0x015A */ char unk_15A[0x2E];
    /* 0x0188 */ f32 unk_188;
    /* 0x018C */ f32 unk_18C;
    /* 0x0190 */ f32 unk_190;
    /* 0x0194 */ f32 unk_194;
    /* 0x0198 */ char unk_198[0x38];
    /* 0x01D0 */ LightNode* lightNode;
    /* 0x01D4 */ LightInfo lightInfoNoGlow;
} EnBdfire; // size = 0x01E4

#endif
