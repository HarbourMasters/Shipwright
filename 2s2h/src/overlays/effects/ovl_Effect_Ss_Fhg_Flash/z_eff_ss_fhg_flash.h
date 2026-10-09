#ifndef _Z64_EFFECT_SS_FHG_FLASH_H_
#define _Z64_EFFECT_SS_FHG_FLASH_H_

#include "global.h"

typedef enum {
    /* 0 */ FHGFLASH_LIGHTBALL,
    /* 1 */ FHGFLASH_SHOCK
} FhgFlashType;

typedef enum {
    /* 0 */ FHGFLASH_LIGHTBALL_GREEN,
    /* 1 */ FHGFLASH_LIGHTBALL_LIGHTBLUE,
    /* 2 */ FHGFLASH_LIGHTBALL_RED,
    /* 3 */ FHGFLASH_LIGHTBALL_YELLOW,
    /* 4 */ FHGFLASH_LIGHTBALL_BLUE,
    /* 5 */ FHGFLASH_LIGHTBALL_PURPLE,
    /* 6 */ FHGFLASH_LIGHTBALL_ORANGE,
    /* 7 */ FHGFLASH_LIGHTBALL_WHITE1,
    /* 8 */ FHGFLASH_LIGHTBALL_WHITE2
} FhgFlashLightBallParam;

typedef enum {
    /* 0 */ FHGFLASH_SHOCK_NO_ACTOR,
    /* 1 */ FHGFLASH_SHOCK_PLAYER,
    /* 2 */ FHGFLASH_SHOCK_GOHT_BACK_LEFT_THIGH,
    /* 3 */ FHGFLASH_SHOCK_GOHT_BACK_RIGHT_PASTERN,
    /* 4 */ FHGFLASH_SHOCK_GOHT_JAW_ROOT,
    /* 5 */ FHGFLASH_SHOCK_GOHT_FRONT_LEFT_LOWER_LEG,
    /* 6 */ FHGFLASH_SHOCK_GOHT_FRONT_RIGHT_UPPER_LEG,
    /* 7 */ FHGFLASH_SHOCK_GOHT_MAX
} FhgFlashLightningParam;

typedef struct {
    /* 0x00 */ Vec3f pos;
    /* 0x0C */ Vec3f velocity;
    /* 0x18 */ Vec3f accel;
    /* 0x24 */ s16 scale;
    /* 0x26 */ u8 params;
    /* 0x28 */ Actor* actor;
    /* 0x2C */ u8 type;
} EffectSsFhgFlashInitParams; // size = 0x30

#endif
