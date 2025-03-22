#include "vr_manager.h"

bool VRManager_InitVR(VRManager* manager) {
    vr::EVRInitError eError = vr::VRInitError_None;
    manager->pHMD = vr::VR_Init(&eError, vr::VRApplication_Scene);
    
    if (eError != vr::VRInitError_None) {
        manager->pHMD = NULL;
        return false;
    }
    
    // Get projection matrices for both eyes
    float nearClip = 0.1f;
    float farClip = 30.0f;
    
    manager->mat4ProjectionLeft = VRManager_GetHMDMatrixProjectionEye(manager, vr::Eye_Left);
    manager->mat4ProjectionRight = VRManager_GetHMDMatrixProjectionEye(manager, vr::Eye_Right);
    
    return true;
}

void VRManager_ShutdownVR(VRManager* manager) {
    if (manager->pHMD) {
        vr::VR_Shutdown();
        manager->pHMD = NULL;
    }
}

void VRManager_UpdateHMDMatrixPose(VRManager* manager) {
    if (!manager->pHMD)
        return;
        
    vr::VRCompositor()->WaitGetPoses(manager->rTrackedDevicePose, 
        vr::k_unMaxTrackedDeviceCount, NULL, 0);
        
    if (manager->rTrackedDevicePose[vr::k_unTrackedDeviceIndex_Hmd].bPoseIsValid) {
        manager->mat4eyePosLeft = VRManager_GetHMDMatrixPoseEye(manager, vr::Eye_Left);
        manager->mat4eyePosRight = VRManager_GetHMDMatrixPoseEye(manager, vr::Eye_Right);
    }
}

Matrix4 VRManager_GetHMDMatrixProjectionEye(VRManager* manager, vr::Hmd_Eye eye) {
    Matrix4 result = {0};
    if (!manager->pHMD)
        return result;
        
    vr::HmdMatrix44_t mat = manager->pHMD->GetProjectionMatrix(eye, 0.1f, 30.0f);
    
    result.m[0][0] = mat.m[0][0]; result.m[0][1] = mat.m[0][1];
    result.m[0][2] = mat.m[0][2]; result.m[0][3] = mat.m[0][3];
    result.m[1][0] = mat.m[1][0]; result.m[1][1] = mat.m[1][1];
    result.m[1][2] = mat.m[1][2]; result.m[1][3] = mat.m[1][3];
    result.m[2][0] = mat.m[2][0]; result.m[2][1] = mat.m[2][1];
    result.m[2][2] = mat.m[2][2]; result.m[2][3] = mat.m[2][3];
    result.m[3][0] = mat.m[3][0]; result.m[3][1] = mat.m[3][1];
    result.m[3][2] = mat.m[3][2]; result.m[3][3] = mat.m[3][3];
    
    return result;
}

Matrix4 VRManager_GetHMDMatrixPoseEye(VRManager* manager, vr::Hmd_Eye eye) {
    Matrix4 result = {0};
    if (!manager->pHMD)
        return result;
        
    vr::HmdMatrix34_t mat = manager->pHMD->GetEyeToHeadTransform(eye);
    
    result.m[0][0] = mat.m[0][0]; result.m[0][1] = mat.m[0][1];
    result.m[0][2] = mat.m[0][2]; result.m[0][3] = mat.m[0][3];
    result.m[1][0] = mat.m[1][0]; result.m[1][1] = mat.m[1][1];
    result.m[1][2] = mat.m[1][2]; result.m[1][3] = mat.m[1][3];
    result.m[2][0] = mat.m[2][0]; result.m[2][1] = mat.m[2][1];
    result.m[2][2] = mat.m[2][2]; result.m[2][3] = mat.m[2][3];
    result.m[3][0] = 0.0f; result.m[3][1] = 0.0f;
    result.m[3][2] = 0.0f; result.m[3][3] = 1.0f;
    
    return result;
}

Vec3f VRManager_GetHMDRotation(VRManager* manager) {
    Vec3f rotation = {0, 0, 0};
    if (!manager->pHMD)
        return rotation;
        
    vr::HmdMatrix34_t mat = 
        manager->rTrackedDevicePose[vr::k_unTrackedDeviceIndex_Hmd].mDeviceToAbsoluteTracking;
        
    // Convert SteamVR rotation matrix to Euler angles
    rotation.y = atan2f(mat.m[0][2], mat.m[2][2]); // Yaw
    rotation.x = -asinf(mat.m[1][2]); // Pitch 
    rotation.z = atan2f(mat.m[1][0], mat.m[1][1]); // Roll
    
    return rotation;
}

bool VRManager_IsHMDPresent(void) {
    return vr::VR_IsHMDPresent();
}

bool VRManager_IsControllerActive(VRManager* manager, vr::ETrackedControllerRole role) {
    if (!manager->pHMD)
        return false;
        
    vr::TrackedDeviceIndex_t deviceIndex = manager->pHMD->GetTrackedDeviceIndexForControllerRole(role);
    return deviceIndex != vr::k_unTrackedDeviceIndexInvalid &&
           manager->rTrackedDevicePose[deviceIndex].bPoseIsValid;
}

Vec3f VRManager_GetControllerDirection(VRManager* manager, vr::ETrackedControllerRole role) {
    Vec3f direction = {0, 0, 0};
    if (!manager->pHMD)
        return direction;
        
    vr::TrackedDeviceIndex_t deviceIndex = manager->pHMD->GetTrackedDeviceIndexForControllerRole(role);
    if (deviceIndex == vr::k_unTrackedDeviceIndexInvalid ||
        !manager->rTrackedDevicePose[deviceIndex].bPoseIsValid)
        return direction;
        
    vr::HmdMatrix34_t mat = manager->rTrackedDevicePose[deviceIndex].mDeviceToAbsoluteTracking;
    
    // Forward vector is -Z in OpenVR
    direction.x = -mat.m[0][2];
    direction.y = -mat.m[1][2];
    direction.z = -mat.m[2][2];
    
    return direction;
} 