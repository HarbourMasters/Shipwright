#ifndef Z_EN_FAMOS_H
#define Z_EN_FAMOS_H

#include "global.h"
#include "objects/object_famos/object_famos.h"

struct EnFamos;

typedef void (*EnFamosActionFunc)(struct EnFamos*, PlayState*);

typedef struct EnFamosRock {
    /* 0x00 */ Vec3f pos;
    /* 0x0C */ Vec3f velocity;
    /* 0x18 */ Vec3s rot;
    /* 0x20 */ f32 scale;
} EnFamosRock; // size = 0x24

#define FAMOS_GET_PATH_INDEX(thisx) ((thisx)->params)
#define FAMOS_GET_AGGRO_DISTANCE(thisx) ((thisx)->shape.rot.x)

#define FAMOS_PATH_INDEX_NONE 0xFF

// stateTimer gets reused:
//   after spotting player, counts frames until first attack (8)
//   after lost aggro, measures frames looking around before returning to home (60)
//   before smash attack, counts frames before disabling emblem colider (4)
//   after smash attack collision, counts frames before rising (3)
//   after smash attack finish, counts frames rising above ground before next attempt (20)

typedef struct EnFamos {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ Vec3s jointTable[FAMOS_LIMB_MAX];
    /* 0x1AC */ Vec3s morphTable[FAMOS_LIMB_MAX];
    /* 0x1D0 */ EnFamosActionFunc actionFunc;
    /* 0x1D4 */ u8 animatedMaterialIndex;
    /* 0x1D5 */ u8 hasFinishedRotating; // stable up or down
    /* 0x1D6 */ u8 pathCount;
    /* 0x1D7 */ u8 waypointIndex;
    /* 0x1D8 */ u8 isCalm; // is NOT aware of player
    /* 0x1DA */ s16 hoverTimer; // start 30, decr to 0, repeat: for trig height adjustment when hovering
    /* 0x1DC */ s16 stateTimer; // reused for different actionFunc
    /* 0x1DE */ s16 debrisTimer; // also counts frames until despawning bom after death
    /* 0x1E0 */ s16 flippedTimer; // frames until it flips right side up
    /* 0x1E2 */ s16 cratorDespawnTimer;
    /* 0x1E4 */ s16 targetYaw;
    /* 0x1E6 */ s16 flipRot; // is used to detect if the actor has been flipped upside down by light arrows
    /* 0x1E8 */ Vec3s* pathPoints;
    /* 0x1EC */ f32 baseHeight; // because hover can hit the floor, we need to keep track so famos cannot by adjusted/moved by terain
    /* 0x1F0 */ f32 aggroDistance;
    /* 0x1F4 */ Vec3f targetDest;
    /* 0x200 */ Vec3f calmPos;
    // havent figured out what the difference between the colliders are, only one cylinder is visible in KZ
    /* 0x20C */ ColliderCylinder collider1;
    /* 0x258 */ ColliderCylinder collider2;
    /* 0x2A4 */ ColliderJntSph   emblemCollider;
    /* 0x2C4 */ ColliderJntSphElement emblemColliderElements[2];
    /* 0x344 */ EnFamosRock rocks[20];
} EnFamos; // size = 0x614

#endif // Z_EN_FAMOS_H
