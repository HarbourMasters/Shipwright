#include "vr/vr_manager.h"
#include <string.h>

bool VRManager_InitVR(VRManager* manager) {
    EVRInitError eError = EVRInitError_VRInitError_None;
    
    // Initialize OpenVR
    VR_InitInternal(&eError, EVRApplicationType_VRApplication_Scene);
    if (eError != EVRInitError_VRInitError_None) {
        return false;
    }

    // Get system interface
    manager->pHMD = VR_GetGenericInterface(IVRSystem_Version, &eError);
    if (eError != EVRInitError_VRInitError_None || !manager->pHMD) {
        VR_ShutdownInternal();
        return false;
    }

    // Get compositor interface
    manager->pCompositor = VR_GetGenericInterface(IVRCompositor_Version, &eError);
    if (eError != EVRInitError_VRInitError_None || !manager->pCompositor) {
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
    if (!manager->pHMD)
        return result;  // Return identity matrix if no HMD

    HmdMatrix44_t mat = manager->pHMD->GetProjectionMatrix(eye, 0.1f, 100.0f);
    // Convert HmdMatrix44_t to MtxF
    memcpy(&result, &mat, sizeof(MtxF));
    return result;
}

MtxF VRManager_GetHMDMatrixPoseEye(VRManager* manager, EVREye eye) {
    MtxF result;
    if (!manager->pHMD)
        return result;  // Return identity matrix if no HMD

    HmdMatrix34_t mat = manager->pHMD->GetEyeToHeadTransform(eye);
    // Convert HmdMatrix34_t to MtxF and handle the conversion from 3x4 to 4x4
    // Implementation depends on your matrix format
    return result;
}

Vec3f VRManager_GetHMDRotation(VRManager* manager) {
    Vec3f rotation = {0.0f, 0.0f, 0.0f};
    if (!manager->pHMD || !manager->rTrackedDevicePose[k_unTrackedDeviceIndex_Hmd].bPoseIsValid)
        return rotation;

    HmdMatrix34_t mat = manager->rTrackedDevicePose[k_unTrackedDeviceIndex_Hmd].mDeviceToAbsoluteTracking;
    // Extract rotation from the matrix
    // Implementation depends on your rotation representation
    return rotation;
}

bool VRManager_IsHMDPresent(void) {
    return VR_IsHmdPresent();
}

bool VRManager_IsControllerActive(VRManager* manager, ETrackedControllerRole role) {
    if (!manager->pHMD)
        return false;

    TrackedDeviceIndex_t deviceIndex = manager->pHMD->GetTrackedDeviceIndexForControllerRole(role);
    return deviceIndex != k_unTrackedDeviceIndexInvalid && 
           manager->rTrackedDevicePose[deviceIndex].bPoseIsValid;
}

Vec3f VRManager_GetControllerDirection(VRManager* manager, ETrackedControllerRole role) {
    Vec3f direction = {0.0f, 0.0f, -1.0f};  // Default forward direction
    if (!manager->pHMD)
        return direction;

    TrackedDeviceIndex_t deviceIndex = manager->pHMD->GetTrackedDeviceIndexForControllerRole(role);
    if (deviceIndex != k_unTrackedDeviceIndexInvalid && 
        manager->rTrackedDevicePose[deviceIndex].bPoseIsValid) {
        HmdMatrix34_t mat = manager->rTrackedDevicePose[deviceIndex].mDeviceToAbsoluteTracking;
        // Extract direction from the matrix
        // Implementation depends on your vector representation
    }
    return direction;
} 