#ifndef Z_EN_CLEAR_TAG_H
#define Z_EN_CLEAR_TAG_H

#include "global.h"

struct EnClearTag;

#define CLEAR_TAG_PARAMS(type) (type)

typedef enum {
    /* 0x00 */ CLEAR_TAG_SMALL_EXPLOSION,
    /* 0x01 */ CLEAR_TAG_LARGE_EXPLOSION,
    /* 0x02 */ CLEAR_TAG_POP,
    /* 0x03 */ CLEAR_TAG_SMALL_LIGHT_RAYS,
    /* 0x04 */ CLEAR_TAG_LARGE_LIGHT_RAYS,
    /* 0x23 */ CLEAR_TAG_SPLASH = 35,
    /* 0xC8 */ CLEAR_TAG_SMOKE = 200
} ClearTagType;

typedef struct EnClearTagEffect {
    /* 0x00 */ u8 type;
    /* 0x01 */ u8 actionTimer;
    /* 0x04 */ Vec3f pos;
    /* 0x10 */ Vec3f velocity;
    /* 0x1C */ Vec3f accel;
    /* 0x28 */ Color_RGBAf primColor;
    /* 0x38 */ Color_RGBAf envColor;
    /* 0x48 */ s16 bounces;
    /* 0x4A */ s16 effectsTimer;
    /* 0x4C */ s16 lightRayAlpha;
    /* 0x4E */ s16 lightRayAlphaDecrementSpeed;
    /* 0x50 */ f32 scale;
    /* 0x54 */ f32 maxScale;
    /* 0x58 */ f32 rotationY;
    /* 0x5C */ f32 rotationX;
    /* 0x60 */ f32 rotationZ;
    /* 0x64 */ f32 maxScaleTarget;
    /* 0x68 */ f32 smokeScaleY;
    /* 0x6C */ f32 smokeScaleX;
} EnClearTagEffect; // size = 0x70

#define EN_CLEAR_TAG_EFFECT_COUNT 103

typedef struct EnClearTag {
    /* 0x0000 */ Actor actor;
    /* 0x0144 */ EnClearTagEffect effect[EN_CLEAR_TAG_EFFECT_COUNT];
    /* 0x2E54 */ u8 cameraState;
    /* 0x2E56 */ s16 activeTimer; // Actor Marked for Death when timer runs out
    /* 0x2E58 */ UNK_TYPE1 unk2E58[0xC];
    /* 0x2E64 */ s16 subCamId;
    /* 0x2E66 */ Color_RGBA8 flashEnvColor;
    /* 0x2E6C */ Vec3f subCamEye;
    /* 0x2E78 */ Vec3f subCamAt;
} EnClearTag; // size = 0x2E84

#endif // Z_EN_CLEAR_TAG_H
