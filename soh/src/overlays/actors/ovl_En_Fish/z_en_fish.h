#ifndef Z_EN_FISH_H
#define Z_EN_FISH_H

#include "ultra64.h"
#include "global.h"

struct EnFish;

typedef void (*EnFishActionFunc)(struct EnFish*, PlayState*);

typedef struct EnFish {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ ColliderJntSph collider;
    /* 0x016C */ ColliderJntSphElement colliderItems[1];
    /* 0x01AC */ SkelAnime skelAnime;
    /* 0x01F0 */ Vec3s jointTable[7];
    /* 0x021A */ Vec3s morphTable[7];
    /* 0x0244 */ EnFishActionFunc actionFunc;
    /* 0x0248 */ s16 timer;
    /* 0x024A */ s16 respawnTimer;
    /* 0x024C */ s16 slowPhase;
    /* 0x024E */ s16 fastPhase;
    /* 0x0250 */ s32 unk_250; // Set to 0 or 5, arg5 of Actor_UpdateBgCheckInfo
} EnFish; // size = 0x0254

typedef enum {
    /* -1 */ FISH_SWIMMING_RESPAWNING = -1, // Used in Zora's Domain; code only uses not 0 or 1, runs away from Player
    /*  0 */ FISH_DROPPED,
    /*  1 */ FISH_SWIMMING_UNIQUE // Used in grottos
} EnFishType;

#endif
