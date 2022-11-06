#ifndef Z_ARMS_HOOK_H
#define Z_ARMS_HOOK_H

#include "ultra64.h"
#include "global.h"

struct ArmsHook;

typedef void (*ArmsHookActionFunc)(struct ArmsHook*, PlayState*);

typedef struct ArmsHook {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ ColliderQuad collider;
    /* 0x01CC */ WeaponInfo hookInfo;
    /* 0x01E8 */ Vec3f unk_1E8;
    /* 0x01F4 */ Vec3f unk_1F4;
    /* 0x0200 */ Actor* grabbed;
    /* 0x0204 */ Vec3f grabbedDistDiff;
    /* 0x0210 */ s16 timer;
    /* 0x0214 */ ArmsHookActionFunc actionFunc;
} ArmsHook; // size = 0x0218

#endif
