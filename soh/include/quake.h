#ifndef QUAKE_H
#define QUAKE_H

#include "z64camera.h"
#include "z64math.h"

#include "extern_c_helper.h"

typedef struct ShakeInfo {
    /* 0x00 */ Vec3f vec1;
    /* 0x0C */ Vec3f vec2;
    /* 0x18 */ s16 rotZ;
    /* 0x1A */ s16 unk_1A;
    /* 0x1C */ s16 zoom;
} ShakeInfo; // size = 0x1E

// TODO: Add QuakeType enum

// TODO: Rename all of these functions
EXTERN_C s16 Quake_Add(Camera* cam, u32 callbackIdx);

EXTERN_C u32 Quake_SetSpeed(s16 idx, s16 value);
EXTERN_C u32 Quake_SetQuakeValues(s16 idx, s16 y, s16 x, s16 zoom, s16 rotZ);
EXTERN_C u32 Quake_SetCountdown(s16 idx, s16 value);

typedef struct SubQuakeRequest14 {
    /* 0x00 */ s16 unk_00;
    /* 0x02 */ s16 unk_02;
    /* 0x04 */ s16 unk_04;
} SubQuakeRequest14; // size = 0x06

EXTERN_C u32 Quake_SetUnkValues(s16 idx, s16 arg1, SubQuakeRequest14 arg2);

EXTERN_C s16 Quake_GetCountdown(s16 idx);
EXTERN_C u32 Quake_RemoveFromIdx(s16 idx);

typedef struct QuakeCamCalc {
    /* 0x00 */ Vec3f atOffset;
    /* 0x0C */ Vec3f eyeOffset;
    /* 0x18 */ s16 rotZ;
    /* 0x1A */ s16 unk_1A;
    /* 0x1C */ s16 zoom;
    /* 0x20 */ f32 unk_20;
} QuakeCamCalc; // size = 0x24

EXTERN_C void Quake_Init(void);
EXTERN_C s16 Quake_Calc(Camera* camera, QuakeCamCalc* camData);

#endif