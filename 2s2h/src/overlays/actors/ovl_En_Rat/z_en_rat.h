#ifndef Z_EN_RAT_H
#define Z_EN_RAT_H

#include "global.h"
#include "objects/object_rat/object_rat.h"

#define EN_RAT_IS_OVERWORLD_TYPE(thisx) ((thisx)->params & 0x8000)
#define EN_RAT_GET_TYPE(thisx) ((thisx)->params)
#define EN_RAT_GET_ATTACK_RANGE(thisx) ((thisx)->params & 0xFF)

/**
 * There are many differences between the two types of Real Bombchu:
 * - Dungeon-type don't respawn, while Overworld-type do.
 * - Dungeon-type leave a blure trail behind them as they chase the player,
 *   while Overworld-type spawn dust around themselves instead.
 * - Dungeon-type won't detonate automatically, while Overworld-type will
 *   detonate after 150 frames of chasing the player.
 * - So long as EN_RAT_GET_ATTACK_RANGE returns something other than 0 or 255,
 *   Dungeon-type have 1/5th of the attack range compared to Overworld-type.
 * - Dungeon-type do not drop items upon defeat, while Overworld-type do.
 */
typedef enum {
    /* 0 */ EN_RAT_TYPE_DUNGEON,
    /* 1 */ EN_RAT_TYPE_OVERWORLD
} EnRatType;

struct EnRat;

typedef void (*EnRatActionFunc)(struct EnRat*, PlayState*);

typedef struct EnRat {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ EnRatActionFunc actionFunc;
    /* 0x18C */ u8 hasLostTrackOfPlayer;
    /* 0x18D */ u8 shouldRotateOntoSurfaces;
    /* 0x18E */ s16 animLoopCounter;
    /* 0x190 */ s16 timer; // Used for both exploding and reviving
    /* 0x192 */ union {
                    s16 stunTimer;
                    s16 hookedState;
                } damageReaction;
    /* 0x194 */ Vec3s jointTable[REAL_BOMBCHU_LIMB_MAX];
    /* 0x1D0 */ Vec3s morphTable[REAL_BOMBCHU_LIMB_MAX];
    /* 0x20C */ Vec3f axisForwards;
    /* 0x218 */ Vec3f axisUp;
    /* 0x224 */ Vec3f axisLeft;
    /* 0x230 */ Vec3f smokePos;
    /* 0x23C */ Vec3f sparkOffsets[2];
    /* 0x254 */ f32 visualJitter;
    /* 0x258 */ f32 attackRange;
    /* 0x25C */ f32 revivePosY;
    /* 0x260 */ s32 blure1Index;
    /* 0x264 */ s32 blure2Index;
    /* 0x268 */ ColliderSphere collider;
} EnRat; // size = 0x2C0

#endif // Z_EN_RAT_H
