#include "vr/vr_manager.h"
#include <string.h>
#include <math.h>

// Global VR manager instance
static VRManager* sVrManager = NULL;

bool VRManager_InitVR(VRManager* manager) {
    EVRInitError eError = EVRInitError_VRInitError_None;
    
    // Initialize OpenVR
    VR_InitInternal(&eError, EVRApplicationType_VRApplication_Scene);
    if (eError != EVRInitError_VRInitError_None) {
        return false;
    }

    // Get system interface
    manager->pHMD = (struct VR_IVRSystem_FnTable*)VR_GetGenericInterface(IVRSystem_Version, &eError);
    if (eError != EVRInitError_VRInitError_None || manager->pHMD == NULL) {
        VR_ShutdownInternal();
        return false;
    }

    // Get compositor interface
    manager->pCompositor = (struct VR_IVRCompositor_FnTable*)VR_GetGenericInterface(IVRCompositor_Version, &eError);
    if (eError != EVRInitError_VRInitError_None || manager->pCompositor == NULL) {
        VR_ShutdownInternal();
        return false;
    }

    return true;
}

void VRManager_ShutdownVR(VRManager* manager) {
    if (manager->pHMD != NULL) {
        VR_ShutdownInternal();
        manager->pHMD = NULL;
        manager->pCompositor = NULL;
    }
}

void VRManager_UpdateHMDMatrixPose(VRManager* manager) {
    if (manager->pCompositor != NULL) {
        manager->pCompositor->GetLastPoses(manager->rTrackedDevicePose, k_unMaxTrackedDeviceCount, NULL, 0);
    }
}

MtxF VRManager_GetHMDMatrixProjectionEye(VRManager* manager, EVREye eye) {
    MtxF result;
    memset(&result, 0, sizeof(MtxF));
    
    if (manager->pHMD == NULL) {
        return result;
    }

    HmdMatrix44_t mat = manager->pHMD->GetProjectionMatrix(eye, 0.1f, 100.0f);
    memcpy(&result, &mat.m[0][0], sizeof(MtxF));
    return result;
}

MtxF VRManager_GetHMDMatrixPoseEye(VRManager* manager, EVREye eye) {
    MtxF result;
    memset(&result, 0, sizeof(MtxF));
    
    if (manager->pHMD == NULL) {
        return result;
    }

    HmdMatrix34_t mat = manager->pHMD->GetEyeToHeadTransform(eye);
    // Convert 3x4 matrix to 4x4
    result.xx = mat.m[0][0]; result.xy = mat.m[0][1]; result.xz = mat.m[0][2]; result.xw = mat.m[0][3];
    result.yx = mat.m[1][0]; result.yy = mat.m[1][1]; result.yz = mat.m[1][2]; result.yw = mat.m[1][3];
    result.zx = mat.m[2][0]; result.zy = mat.m[2][1]; result.zz = mat.m[2][2]; result.zw = mat.m[2][3];
    result.wx = 0.0f;        result.wy = 0.0f;        result.wz = 0.0f;        result.ww = 1.0f;
    return result;
}

Vec3f VRManager_GetHMDRotation(VRManager* manager) {
    Vec3f rotation = {0.0f, 0.0f, 0.0f};
    
    if (manager->pHMD == NULL || !manager->rTrackedDevicePose[k_unTrackedDeviceIndex_Hmd].bPoseIsValid) {
        return rotation;
    }

    HmdMatrix34_t mat = manager->rTrackedDevicePose[k_unTrackedDeviceIndex_Hmd].mDeviceToAbsoluteTracking;
    
    // Convert rotation matrix to Euler angles
    rotation.y = atan2f(mat.m[0][2], mat.m[2][2]); // Yaw
    rotation.x = -asinf(mat.m[1][2]); // Pitch
    rotation.z = atan2f(mat.m[1][0], mat.m[1][1]); // Roll
    
    return rotation;
}

bool VRManager_IsHMDPresent(void) {
    return VR_IsHmdPresent();
}

bool VRManager_IsControllerActive(VRManager* manager, ETrackedControllerRole role) {
    if (manager->pHMD == NULL) {
        return false;
    }

    TrackedDeviceIndex_t deviceIndex = manager->pHMD->GetTrackedDeviceIndexForControllerRole(role);
    return deviceIndex != k_unTrackedDeviceIndexInvalid && 
           manager->rTrackedDevicePose[deviceIndex].bPoseIsValid;
}

Vec3f VRManager_GetControllerDirection(VRManager* manager, ETrackedControllerRole role) {
    Vec3f direction = {0.0f, 0.0f, -1.0f};  // Default forward direction
    
    if (manager->pHMD == NULL) {
        return direction;
    }

    TrackedDeviceIndex_t deviceIndex = manager->pHMD->GetTrackedDeviceIndexForControllerRole(role);
    if (deviceIndex != k_unTrackedDeviceIndexInvalid && 
        manager->rTrackedDevicePose[deviceIndex].bPoseIsValid) {
        HmdMatrix34_t mat = manager->rTrackedDevicePose[deviceIndex].mDeviceToAbsoluteTracking;
        // Forward vector is -Z in OpenVR
        direction.x = -mat.m[0][2];
        direction.y = -mat.m[1][2];
        direction.z = -mat.m[2][2];
    }
    return direction;
} 