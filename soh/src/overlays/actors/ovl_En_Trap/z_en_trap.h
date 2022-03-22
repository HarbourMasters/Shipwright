#ifndef Z_EN_TRAP_H
#define Z_EN_TRAP_H

#include "ultra64.h"
#include "global.h"

// Different movement modes (set in params):
#define SPIKETRAP_MODE_LINEAR 0x10
#define SPIKETRAP_MODE_CIRCULAR 0x20
#define SPIKETRAP_MODE_FOUR_WAY 0x30

// When four-way mode is set, these flags decide on which directions are allowed:
#define SPIKETRAP_FOURWAY_FWD_ALLOWED (1 << 0)
#define SPIKETRAP_FOURWAY_BACK_ALLOWED (1 << 1)
#define SPIKETRAP_FOURWAY_LEFT_ALLOWED (1 << 2)
#define SPIKETRAP_FOURWAY_RIGHT_ALLOWED (1 << 3)

struct EnTrap;

typedef struct EnTrap {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ s32 playerDmgTimer;
    /* 0x0150 */ s16 upperParams;
    /* 0x0152 */ s16 genericVar1;
    /* 0x0154 */ f32 genericVar2;
    /* 0x0158 */ Vec3f targetPosLeft;
    /* 0x0164 */ Vec3f targetPosRight;
    /* 0x0170 */ Vec3f targetPosFwd;
    /* 0x017C */ Vec3f targetPosBack;
    /* 0x0188 */ Vec3f moveSpeedLeftRight;
    /* 0x0194 */ Vec3f moveSpeedForwardBack;
    /* 0x01A0 */ ColliderCylinder collider;
} EnTrap; // size = 0x01EC

#endif
