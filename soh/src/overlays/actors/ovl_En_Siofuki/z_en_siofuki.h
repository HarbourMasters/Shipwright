#ifndef Z_EN_SIOFUKI_H
#define Z_EN_SIOFUKI_H

#include "ultra64.h"
#include "global.h"

typedef enum {
    /* 0x00 */ EN_SIOFUKI_RAISING,
    /* 0x01 */ EN_SIOFUKI_LOWERING
} EnSiofukiType;

struct EnSiofuki;

typedef void (*EnSiofukiActionFunc)(struct EnSiofuki*, PlayState*);

typedef struct EnSiofuki {
    /* 0x0000 */ DynaPolyActor dyna;
    /* 0x0164 */ EnSiofukiActionFunc actionFunc;
    /* 0x0168 */ s32 timer;
    /* 0x016C */ f32 initPosY;
    /* 0x0170 */ f32 unk_170;
    /* 0x0174 */ f32 unk_174;
    /* 0x0178 */ f32 oscillation;
    /* 0x017C */ f32 targetHeight;
    /* 0x0180 */ f32 currentHeight;
    /* 0x0184 */ s32 splashTimer;
    /* 0x0188 */ s32 applySpeed;
    /* 0x018C */ f32 appliedSpeed;
    /* 0x0190 */ f32 targetAppliedSpeed;
    /* 0x0194 */ s16 appliedYaw;
    /* 0x0196 */ s16 activeTime;
    /* 0x0198 */ f32 maxHeight;
    /* 0x019C */ u8 sfxFlags;
} EnSiofuki; // size = 0x01A0

#endif
