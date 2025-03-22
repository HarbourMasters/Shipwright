#pragma once

#include <stddef.h>     // for size_t
#include <stdint.h>     // for standard integer types
#include <stdlib.h>     // for standard library functions
#include <openvr/openvr.h>
#include "global.h"

typedef struct {
    vr::IVRSystem* pHMD;
    vr::TrackedDevicePose_t rTrackedDevicePose[vr::k_unMaxTrackedDeviceCount];
    MtxF mat4ProjectionLeft;
    MtxF mat4ProjectionRight;
    MtxF mat4eyePosLeft;
    MtxF mat4eyePosRight;
} VRManager;

// Function declarations
bool VRManager_InitVR(VRManager* manager);
void VRManager_ShutdownVR(VRManager* manager);
void VRManager_UpdateHMDMatrixPose(VRManager* manager);
MtxF VRManager_GetHMDMatrixProjectionEye(VRManager* manager, vr::Hmd_Eye eye);
MtxF VRManager_GetHMDMatrixPoseEye(VRManager* manager, vr::Hmd_Eye eye);
Vec3f VRManager_GetHMDRotation(VRManager* manager);
bool VRManager_IsHMDPresent(void);
bool VRManager_IsControllerActive(VRManager* manager, vr::ETrackedControllerRole role);
Vec3f VRManager_GetControllerDirection(VRManager* manager, vr::ETrackedControllerRole role); 