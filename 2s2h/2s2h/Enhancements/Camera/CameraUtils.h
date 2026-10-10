#ifndef CAMERA_UTILS_H
#define CAMERA_UTILS_H
#include "ultratypes.h"

// Camera will reload its paramData. Usually that means setting the read-only data from what is stored in
// CameraModeValue arrays. Although sometimes some read-write data is reset as well
#define RELOAD_PARAMS(camera) ((camera->animState == 0) || (camera->animState == 10) || (camera->animState == 20))

/**
 * Camera data is stored in both read-only data and OREG as s16, and then converted to the appropriate type during
 * runtime. If a small f32 is being stored as an s16, it is common to store that value 100 times larger than the
 * original value. This is then scaled back down during runtime with the CAM_RODATA_UNSCALE macro.
 */
#define CAM_RODATA_SCALE(x) ((x)*100.0f)
#define CAM_RODATA_UNSCALE(x) ((x)*0.01f)

// Load the next value from camera read-only data stored in CameraModeValue
#define GET_NEXT_RO_DATA(values) ((values++)->val)
// Load the next value and scale down from camera read-only data stored in CameraModeValue
#define GET_NEXT_SCALED_RO_DATA(values) CAM_RODATA_UNSCALE(GET_NEXT_RO_DATA(values))

typedef struct {
    /* 0x0 */ s16 val;
    /* 0x2 */ s16 param;
} CameraModeValue; // size = 0x4

typedef struct {
    /* 0x0 */ s16 funcId;
    /* 0x2 */ s16 numValues;
    /* 0x4 */ CameraModeValue* values;
} CameraMode; // size = 0x8

/**
 * Flags:
 * (flags & 0xF): Priority (lower value has higher priority)
 * (flags & 0x40000000): Store previous setting and bgCamData, also ignores water checks
 * (flags & 0x80000000): Set camera setting based on bg/scene data and reset action function state
 */
typedef struct {
    /* 0x0 */ u32 validModes;
    /* 0x4 */ u32 flags;
    /* 0x8 */ CameraMode* cameraModes;
} CameraSetting; // size = 0xC

#endif // CAMERA_UTILS_H
