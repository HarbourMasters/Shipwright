#ifndef Z_EN_TEST7_H
#define Z_EN_TEST7_H

#include "global.h"

struct EnTest7;

typedef void (*EnTest7PlayerCamFunc)(struct EnTest7*, PlayState*);
typedef void (*EnTest7ActionFunc)(struct EnTest7*, PlayState*);

#define OWL_WARP_CS_GET_OCARINA_MODE(thisx) ((thisx)->params)

#define ENTEST7_ARRIVE -1

typedef struct {
    /* 0x00 */ f32 unk_00;
    /* 0x04 */ f32 unk_04;
    /* 0x08 */ f32 xzScale;
    /* 0x0C */ f32 yScale;
    /* 0x10 */ s16 yaw;
} OwlWarpWindCapsule; // size = 0x14

typedef enum OwlWarpFeatherType {
    /* 0 */ OWL_WARP_FEATHER_TYPE_DISABLED,
    /* 1 */ OWL_WARP_FEATHER_TYPE_1,
    /* 2 */ OWL_WARP_FEATHER_TYPE_2
} OwlWarpFeatherType;

typedef struct {
    /* 0x00 */ OwlWarpFeatherType type;
    /* 0x04 */ s32 unk_04;
    /* 0x08 */ Vec3f pos;
    /* 0x14 */ Vec3f velocity;
    /* 0x20 */ Vec3f accel;
    /* 0x2C */ f32 scale;
    /* 0x30 */ Vec3s rot;
    /* 0x36 */ Vec3s angularVelocity;
} OwlWarpFeather; // size = 0x3C

#define OWL_WARP_NUM_FEATHERS 100

#define OWL_WARP_FLAGS_DRAW_WINGS (1 << 0)
#define OWL_WARP_FLAGS_DRAW_WIND_CAPSULE (1 << 1)
#define OWL_WARP_FLAGS_DRAW_LENS_FLARE (1 << 2)
#define OWL_WARP_FLAGS_8 (1 << 3)
#define OWL_WARP_FLAGS_10 (1 << 4)
#define OWL_WARP_FLAGS_20 (1 << 5)
#define OWL_WARP_FLAGS_40 (1 << 6)
#define OWL_WARP_FLAGS_80 (1 << 7)

typedef struct EnTest7 {
    /* 0x0000 */ Actor actor;
    /* 0x0144 */ s32 flags;
    /* 0x0148 */ OwlWarpWindCapsule windCapsule;
    /* 0x015C */ OwlWarpFeather feathers[OWL_WARP_NUM_FEATHERS];
    /* 0x18CC */ KFSkelAnimeFlex kfSkelAnime;
    /* 0x18FC */ Vec3s jointTable[114];
    /* 0x1BA8 */ Vec3s morphTable[114];
    /* 0x1E54 */ s32 timer;
    /* 0x1E58 */ EnTest7ActionFunc actionFunc;
    /* 0x1E5C */ EnTest7PlayerCamFunc playerCamFunc;
    /* 0x1E60 */ Vec3f subCamEye;
    /* 0x1E6C */ Vec3f subCamAt;
    /* 0x1E78 */ f32 subCamFov;
    /* 0x1E7C */ LightNode* lightNode;
    /* 0x1E80 */ LightInfo lightInfo;
    /* 0x1E8E */ s16 playerYaw;
    /* 0x1E90 */ f32 playerScaleX;
    /* 0x1E94 */ f32 playerScaleZ;
    /* 0x1E98 */ ActorFunc playerDrawFunc;
} EnTest7; // size = 0x1E9C

#endif // Z_EN_TEST7_H
