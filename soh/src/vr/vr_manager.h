#pragma once

#include <stddef.h>     // for size_t
#include <stdint.h>     // for standard integer types
#include <stdlib.h>     // for standard library functions
#include <math.h>       // for math functions
#include <openvr_capi.h>
#include "global.h"

// OpenVR constants
#define k_unMaxTrackedDeviceCount 64
#define k_unTrackedDeviceIndex_Hmd 0
#define k_unTrackedDeviceIndexInvalid 0xFFFFFFFF

typedef struct {
    struct VR_IVRSystem_FnTable* pHMD;
    struct VR_IVRCompositor_FnTable* pCompositor;
    TrackedDevicePose_t rTrackedDevicePose[k_unMaxTrackedDeviceCount];
    MtxF mat4ProjectionLeft;
    MtxF mat4ProjectionRight;
    MtxF mat4eyePosLeft;
    MtxF mat4eyePosRight;
} VRManager;

extern VRManager *gVRManager;

// Function declarations
bool VRManager_InitVR(VRManager* manager);
void VRManager_ShutdownVR(VRManager* manager);
void VRManager_UpdateHMDMatrixPose(VRManager* manager);
MtxF VRManager_GetHMDMatrixProjectionEye(VRManager* manager, EVREye eye);
MtxF VRManager_GetHMDMatrixPoseEye(VRManager* manager, EVREye eye);
Vec3f VRManager_GetHMDRotation(VRManager* manager);
bool VRManager_IsHMDPresent(void);
bool VRManager_IsControllerActive(VRManager* manager, ETrackedControllerRole role);
Vec3f VRManager_GetControllerDirection(VRManager* manager, ETrackedControllerRole role);
void VRManager_SubmitTestFrame(VRManager* manager); 