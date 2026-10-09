#ifndef Z_EFF_KAMEJIMA_WAVE_H
#define Z_EFF_KAMEJIMA_WAVE_H

#include "global.h"

#define EFFKAMEJIMAWAVE_GET_F(thisx) ((thisx)->params & 0xF)

struct EffKamejimaWave;

typedef void (*EffKamejimaWaveActionFunc)(struct EffKamejimaWave*, PlayState*);

typedef struct EffKamejimaWave {
    /* 0x000 */ Actor actor;
    /* 0x144 */ f32 scaleOffset;
    /* 0x148 */ UNK_TYPE1 pad148[4];
    /* 0x14C */ EffKamejimaWaveActionFunc actionFunc;
} EffKamejimaWave; // size = 0x150

typedef enum EffKamejimaWaveParam {
    /* 0 */ EFFKAMEJIMAWAVE_PARAM_0,
    /* 1 */ EFFKAMEJIMAWAVE_PARAM_1
} EffKamejimaWaveParam;

#endif // Z_EFF_KAMEJIMA_WAVE_H
