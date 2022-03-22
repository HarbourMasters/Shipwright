#ifndef Z_EN_MAG_H
#define Z_EN_MAG_H

#include "ultra64.h"
#include "global.h"

struct EnMag;

typedef struct EnMag {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ char unk_14C[0x0004];
    /* 0x0150 */ Font font;
    /* 0xE2D8 */ s16 effectFadeInState;
    /* 0xE2DA */ s16 effectFadeInTimer;
    /* 0xE2DC */ s16 globalState;
    /* 0xE2E0 */ f32 effectPrimLodFrac;
    /* 0xE2E4 */ f32 effectPrimColor[3];
    /* 0xE2F0 */ f32 effectEnvColor[3];
    /* 0xE2FC */ f32 effectAlpha;
    /* 0xE300 */ f32 mainAlpha;
    /* 0xE304 */ f32 subAlpha;
    /* 0xE308 */ f32 copyrightAlpha;
    /* 0xE30C */ s16 unk_E30C;
    /* 0xE30E */ s16 effectScroll;
    /* 0xE310 */ char unk_E310[0x0002];
    /* 0xE312 */ s16 copyrightAlphaStep;
    /* 0xE314 */ s16 fadeOutAlphaStep;
    /* 0xE316 */ s16 unk_E316;
    /* 0xE318 */ s16 unk_E318;
    /* 0xE31C */ s32 unk_E31C;
    /* 0xE320 */ s32 unk_E320;
    /* 0xE324 */ char unk_E324[0x0004];
} EnMag; // size = 0xE328

typedef enum {
    /* 0x00 */ MAG_STATE_INITIAL,
    /* 0x01 */ MAG_STATE_FADE_IN,
    /* 0x02 */ MAG_STATE_DISPLAY,
    /* 0x03 */ MAG_STATE_FADE_OUT,
    /* 0x04 */ MAG_STATE_POST_DISPLAY
} EnMagGlobalState;

#endif
