#ifndef Z_OBJ_MINE_H
#define Z_OBJ_MINE_H

#include "global.h"

struct ObjMine;

typedef void (*ObjMineActionFunc)(struct ObjMine*, PlayState*);

#define OBJMINE_GET_LINK_COUNT(thisx) ((thisx)->params & 0x3F)
#define OBJMINE_GET_PATH_INDEX(thisx) ((thisx)->params & 0xFF)
#define OBJMINE_GET_PATH_SPEED(thisx) (((thisx)->params >> 8) & 7)
#define OBJMINE_GET_TYPE(thisx) (((thisx)->params >> 12) & 3)

#define OBJMINE_PARAMS(type, linkCount, pathIndex, pathSpeed) (((type) << 0xC) | ((type == OBJMINE_TYPE_PATH) ? ((pathIndex) | ((pathSpeed) << 8)) : (linkCount)))
#define OBJMINE_PATH_PARAMS(pathIndex, pathSpeed) OBJMINE_PARAMS(OBJMINE_TYPE_PATH, 0, pathIndex, pathSpeed)
#define OBJMINE_AIR_PARAMS(linkCount) OBJMINE_PARAMS(OBJMINE_TYPE_AIR, linkCount, 0, 0)
#define OBJMINE_WATER_PARAMS(linkCount) OBJMINE_PARAMS(OBJMINE_TYPE_WATER, linkCount, 0, 0)

#define OBJMINE_CHAIN_MAX 63

typedef enum {
    /* 0 */ OBJMINE_TYPE_PATH,
    /* 1 */ OBJMINE_TYPE_AIR,
    /* 2 */ OBJMINE_TYPE_WATER
} ObjMineType;

typedef struct {
    /* 0x00 */ Vec3f x;
    /* 0x0C */ Vec3f y;
    /* 0x18 */ Vec3f z;
} ObjMineMtxF3; // size = 0x24

typedef struct {
    /* 0x0 */ s16 twist;
    /* 0x2 */ s16 spin;
} ObjMineAirLink; // size = 0x4

typedef struct {
    /* 0x00 */ ObjMineMtxF3 basis;
    /* 0x24 */ Vec3f translation; // unused
    /* 0x30 */ Vec2f displacement;
    /* 0x38 */ Vec2f velocity;
    /* 0x40 */ f32 restore;
    /* 0x44 */ f32 drag;
    /* 0x48 */ f32 knockback;
    /* 0x4C */ s16 knockbackAngle;
    /* 0x50 */ f32 swaySize;
    /* 0x54 */ s16 swayPhase;
    /* 0x58 */ f32 swayMax;
    /* 0x5C */ f32 wallCheckDistSq;
    /* 0x60 */ ObjMineAirLink links[OBJMINE_CHAIN_MAX];
} ObjMineAirChain; // size = 0x15C

typedef struct {
    /* 0x00 */ ObjMineMtxF3 basis;
    /* 0x24 */ Vec3f pos; 
    /* 0x30 */ Vec3f velocity;
    /* 0x3C */ Vec3f accel; 
} ObjMineWaterLink; // size = 0x48

typedef struct {
    /* 0x00 */ f32 drag;
    /* 0x04 */ Vec3f knockbackDir;
    /* 0x10 */ f32 knockback;
    /* 0x14 */ f32 swayXZ;
    /* 0x18 */ f32 swayY;
    /* 0x1C */ f32 swayMax;
    /* 0x20 */ s16 swayPhaseVel;
    /* 0x24 */ f32 restoreXZ;
    /* 0x28 */ f32 maxY;
    /* 0x2C */ f32 restY;
    /* 0x30 */ f32 restoreY;
    /* 0x34 */ f32 wallCheckDistSq;
    /* 0x38 */ Vec2f wallEject;
    /* 0x40 */ s8 touchWall;
    /* 0x44 */ ObjMineWaterLink links[OBJMINE_CHAIN_MAX];
} ObjMineWaterChain; // size = 0x11FC

typedef struct ObjMine {
    /* 0x0000 */ Actor actor;
    /* 0x0144 */ ColliderJntSph collider;
    /* 0x0164 */ ColliderJntSphElement colliderElements[1];
    /* 0x01A4 */ ObjMineActionFunc actionFunc;
    /* 0x01A8 */ f32 pathSpeed;
    /* 0x01AC */ s32 waypointCount;
    /* 0x01B0 */ s32 waypointIndex;
    /* 0x01B4 */ Vec3s* waypoints;
    /* 0x01B8 */ union { 
                     ObjMineAirChain air;
                     ObjMineWaterChain water;
                 } chain;
} ObjMine; // size = 0x13B4

#endif // Z_OBJ_MINE_H
