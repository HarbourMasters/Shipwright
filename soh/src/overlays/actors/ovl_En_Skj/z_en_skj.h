#ifndef Z_EN_SKJ_H
#define Z_EN_SKJ_H

#include "ultra64.h"
#include "global.h"

struct EnSkj;

typedef void (*EnSkjActionFunc)(struct EnSkj*, PlayState*);

typedef struct EnSkj {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;
    /* 0x0190 */ Vec3s jointTable[19];
    /* 0x0202 */ Vec3s morphTable[19];
    /* 0x0274 */ EnSkjActionFunc actionFunc;
    /* 0x0278 */ ColliderCylinder collider;
    /* 0x02C4 */ u16 textId;
    /* 0x02C6 */ s16 timer;
    /* 0x02C8 */ s16 unk_2C8;
    /* 0x02CA */ s16 multiuseTimer;
    /* 0x02CC */ s16 needleShootTimer;
    /* 0x02CE */ s16 dodgeResetTimer;
    /* 0x02D0 */ u8 animIndex;
    /* 0x02D1 */ u8 action;
    /* 0x02D2 */ u8 backflipFlag;
    /* 0x02D3 */ u8 unk_2D3; // Is set to zero when walking to trade for skull mask and set to 1 when dying
    /* 0x02D4 */ u8 needlesToShoot;
    /* 0x02D5 */ u8 hitsUntilDodge; // Upon reaching zero will always backflip to avoid stun lock
    /* 0x02D6 */ u8 unk_2D6;
    /* 0x02D7 */ u8 playerInRange;
    /* 0x02D8 */ u8 minigameState; // Upon reaching zero stops and does another action
    /* 0x02DC */ u32 alpha;
    /* 0x02E0 */ Vec3f center;
    /* 0x02EC */ f32 unk_2EC;
    /* 0x02F0 */ f32 unk_2F0;
    /* 0x02F4 */ Vec3f posCopy;
} EnSkj; // size = 0x0300

#endif
