#ifndef Z_ARMS_HOOK_H
#define Z_ARMS_HOOK_H

#include "global.h"

struct ArmsHook;

typedef void (*ArmsHookActionFunc)(struct ArmsHook*, PlayState*);

typedef struct ArmsHook {
    /* 0x000 */ Actor actor;
    /* 0x144 */ ColliderQuad collider;
    /* 0x1C4 */ WeaponInfo weaponInfo;
    /* 0x1E0 */ Vec3f unk1E0;
    /* 0x1EC */ Vec3f unk1EC;
    /* 0x1F8 */ Actor* attachedActor;
    /* 0x1FC */ Vec3f attachPointOffset; // Distance from the hookshot attach point to world pos of `attachedActor`
    /* 0x208 */ s8 unk_208;
    /* 0x20A */ s16 timer;
    /* 0x20C */ ArmsHookActionFunc actionFunc;
} ArmsHook; // size = 0x210

#endif // Z_ARMS_HOOK_H
