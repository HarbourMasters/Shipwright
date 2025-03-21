#include "vr_manager.h"

bool VRManager::InitVR() {
    vr::EVRInitError eError = vr::VRInitError_None;
    m_pHMD = vr::VR_Init(&eError, vr::VRApplication_Scene);
    
    if (eError != vr::VRInitError_None) {
        m_pHMD = nullptr;
        return false;
    }
    
    // Get projection matrices for both eyes
    float nearClip = 0.1f;
    float farClip = 30.0f;
    
    m_mat4ProjectionLeft = GetHMDMatrixProjectionEye(vr::Eye_Left);
    m_mat4ProjectionRight = GetHMDMatrixProjectionEye(vr::Eye_Right);
    
    return true;
}

void VRManager::ShutdownVR() {
    if (m_pHMD) {
        vr::VR_Shutdown();
        m_pHMD = nullptr;
    }
}

void VRManager::UpdateHMDMatrixPose() {
    if (!m_pHMD)
        return;
        
    vr::VRCompositor()->WaitGetPoses(m_rTrackedDevicePose, 
        vr::k_unMaxTrackedDeviceCount, nullptr, 0);
        
    if (m_rTrackedDevicePose[vr::k_unTrackedDeviceIndex_Hmd].bPoseIsValid) {
        m_mat4eyePosLeft = GetHMDMatrixPoseEye(vr::Eye_Left);
        m_mat4eyePosRight = GetHMDMatrixPoseEye(vr::Eye_Right);
    }
}

Matrix4 VRManager::GetHMDMatrixProjectionEye(vr::Hmd_Eye eye) {
    if (!m_pHMD)
        return Matrix4();
        
    vr::HmdMatrix44_t mat = m_pHMD->GetProjectionMatrix(eye, 0.1f, 30.0f);
    Matrix4 result;
    
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

Matrix4 VRManager::GetHMDMatrixPoseEye(vr::Hmd_Eye eye) {
    if (!m_pHMD)
        return Matrix4();
        
    vr::HmdMatrix34_t mat = m_pHMD->GetEyeToHeadTransform(eye);
    Matrix4 result;
    
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

Vec3f VRManager::GetHMDRotation() {
    if (!m_pHMD)
        return Vec3f{0,0,0};
        
    vr::HmdMatrix34_t mat = 
        m_rTrackedDevicePose[vr::k_unTrackedDeviceIndex_Hmd].mDeviceToAbsoluteTracking;
        
    // Convert SteamVR rotation matrix to Euler angles
    Vec3f rotation;
    rotation.y = atan2f(mat.m[0][2], mat.m[2][2]); // Yaw
    rotation.x = -asinf(mat.m[1][2]); // Pitch 
    rotation.z = atan2f(mat.m[1][0], mat.m[1][1]); // Roll
    
    return rotation;
}

bool VRManager::IsControllerActive(vr::ETrackedControllerRole role) {
    if (!m_pHMD)
        return false;
        
    vr::TrackedDeviceIndex_t deviceIndex = m_pHMD->GetTrackedDeviceIndexForControllerRole(role);
    return deviceIndex != vr::k_unTrackedDeviceIndexInvalid &&
           m_rTrackedDevicePose[deviceIndex].bPoseIsValid;
}

Vec3f VRManager::GetControllerDirection(vr::ETrackedControllerRole role) {
    if (!m_pHMD)
        return Vec3f{0,0,0};
        
    vr::TrackedDeviceIndex_t deviceIndex = m_pHMD->GetTrackedDeviceIndexForControllerRole(role);
    if (deviceIndex == vr::k_unTrackedDeviceIndexInvalid ||
        !m_rTrackedDevicePose[deviceIndex].bPoseIsValid)
        return Vec3f{0,0,0};
        
    vr::HmdMatrix34_t mat = m_rTrackedDevicePose[deviceIndex].mDeviceToAbsoluteTracking;
    
    // Forward vector is -Z in OpenVR
    Vec3f direction;
    direction.x = -mat.m[0][2];
    direction.y = -mat.m[1][2];
    direction.z = -mat.m[2][2];
    
    return direction;
} 