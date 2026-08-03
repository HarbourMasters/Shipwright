#ifndef Z_EN_BIRD_H
#define Z_EN_BIRD_H

#include <libultraship/libultra.h>
#include "global.h"

struct EnBird;

typedef void (*EnBirdActionFunc)(struct EnBird*, PlayState*);

typedef struct EnBird {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;
    /* 0x0190 */ EnBirdActionFunc actionFunc;
    /* 0x0194 */ u32 unk_194;
    /* 0x0198 */ s32 timer;
    /* 0x019C */ s16 scaleAnimSpeed;
    /* 0x019E */ char unk_19E[0x2];
    /* 0x01A0 */ f32 posYMag;
    /* 0x01A4 */ f32 rotYMag;
    /* 0x01A8 */ f32 speedTarget;
    /* 0x01AC */ f32 speedStep;
    /* 0x01B0 */ f32 flightDistance;
    /* 0x01B4 */ f32 posYPhase;
    /* 0x01B8 */ f32 posYPhaseStep;
    /* 0x01BC */ f32 unk_1BC;
    /* 0x01C0 */ s16 rotYStep;
    /* 0x01C2 */ char unk_1C2[0x1A];
} EnBird; // size = 0x01DC

#endif
