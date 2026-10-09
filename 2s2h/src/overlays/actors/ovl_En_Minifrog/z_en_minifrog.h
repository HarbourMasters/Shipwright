#ifndef Z_EN_MINIFROG_H
#define Z_EN_MINIFROG_H

#include "global.h"
#include "objects/object_fr/object_fr.h"

struct EnMinifrog;

typedef void (*EnMinifrogActionFunc)(struct EnMinifrog*, PlayState*);

#define EN_FROG_IS_RETURNED(thisx) ((((thisx)->params) & 0xF0) >> 4)

typedef enum {
    /* 0x00 */ FROG_YELLOW, // Mountain Village
    /* 0x01 */ FROG_CYAN, // Woodfall Temple
    /* 0x02 */ FROG_PINK, // Great Bay Temple
    /* 0x03 */ FROG_BLUE, // Southern Swamp
    /* 0x04 */ FROG_WHITE, // Laundry Pool
    /* 0xFF */ FROG_NONE = 0xFF // -1
} FrogType;

typedef enum {
    /* 0 */ FROG_STATE_JUMP,
    /* 1 */ FROG_STATE_AIR,
    /* 2 */ FROG_STATE_GROUND
} FrogJumpState;

typedef struct EnMinifrog {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ Vec3s jointTable[FROG_LIMB_MAX];
    /* 0x218 */ Vec3s morphTable[FROG_LIMB_MAX];
    /* 0x2A8 */ EnMinifrogActionFunc actionFunc;
    /* 0x2AC */ struct EnMinifrog* frog;
    /* 0x2B0 */ s16 frogIndex;
    /* 0x2B2 */ s16 jumpState;
    /* 0x2B4 */ s16 timer;
    /* 0x2B6 */ u16 flags;
    /* 0x2B8 */ ColliderCylinder collider;
} EnMinifrog; // size = 0x304

#endif // Z_EN_MINIFROG_H
