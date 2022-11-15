#ifndef Z_EN_RL_H
#define Z_EN_RL_H

#include "ultra64.h"
#include "global.h"

struct EnRl;

typedef void (*EnRlActionFunc)(struct EnRl*, PlayState*);
typedef void (*EnRlDrawFunc)(struct EnRl*, PlayState*);

typedef struct EnRl {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;
    /* 0x0190 */ s16 eyeTextureIndex;
    /* 0x0192 */ s16 timer;
    /* 0x0194 */ s32 action;
    /* 0x0198 */ s32 drawConfig;
    /* 0x019C */ f32 unk_19C;
    /* 0x01A0 */ s32 alpha;
    /* 0x01A4 */ u32 lightBallSpawned;
    /* 0x01A8 */ s32 lightMedallionGiven;
} EnRl; // size = 0x01AC

#endif
